#pragma once

#include "offsets.h"
#include <Windows.h>

namespace bunnyhop
{
    const uint32_t FL_ONGROUND = (1 << 0);

    const uint32_t JUMP_PRESS = 5;
    const uint32_t JUMP_RELEASE = 4;

    void Bunnyhop(uintptr_t client, uintptr_t localPlayer)
    {
        if (!(GetAsyncKeyState(VK_SPACE) & 0x8000))
        {
            return;
        }

        uint32_t flag = *reinterpret_cast<std::uint32_t*>(localPlayer + offset::m_fFlags);

        // 2. Use bitwise AND to check for ground contact, ignoring crouch/water flags
        if (flag & FL_ONGROUND)
        {
            // We are on the ground. Send the jump command.
            *reinterpret_cast<uint32_t*>(client + offset::m_dwForceJump) = JUMP_PRESS;
        }
        else
        {
            // We are in the air. Force the release state so the game registers the next jump.
            *reinterpret_cast<uint32_t*>(client + offset::m_dwForceJump) = JUMP_RELEASE;
        }
    }
}