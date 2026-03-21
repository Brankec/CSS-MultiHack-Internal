#pragma once

#include "offsets.h"
#include "../triggerbot_math.h"
#include <thread>
#include <chrono>
#include <cstdint>

namespace triggerbot
{
    const uint32_t MAX_PLAYERS = 64;
    const uint32_t ATTACK_PRESS = 5;
    const uint32_t ATTACK_RELEASE = 4;

    const float HEAD_RADIUS_UNITS = 7.0f;

    void Triggerbot(uintptr_t engine, uintptr_t client, uintptr_t localPlayer)
    {
        // Hardcoded: ALT Button
        if (!(GetAsyncKeyState(VK_MENU) & 0x8000))
        {
            return;
        }

        uint32_t crosshairId = *reinterpret_cast<uint32_t*>(localPlayer + offset::m_iCrosshairId);

        if (crosshairId == 0 || crosshairId > MAX_PLAYERS)
        {
            return;
        }

        uintptr_t targetEntity = *reinterpret_cast<std::uintptr_t*>(client + (offset::m_dwEntityList + (crosshairId - 1) * 0x20));

        if (targetEntity)
        {
            uint32_t targetTeam = *reinterpret_cast<uint32_t*>(targetEntity + offset::m_iTeamNum);
            uint32_t playerTeam = *reinterpret_cast<uint32_t*>(localPlayer + offset::m_iTeamNum);

            if (playerTeam != targetTeam)
            {
                // Read Local Player Eye Position
                Vector3 localOrigin = *reinterpret_cast<Vector3*>(localPlayer + offset::m_vecOrigin);
                Vector3 viewOffset = *reinterpret_cast<Vector3*>(localPlayer + offset::m_vecViewOffset);
                Vector3 localEyes = { localOrigin.x + viewOffset.x, localOrigin.y + viewOffset.y, localOrigin.z + viewOffset.z };

                // Read Target Head Bone Position using BoneID enum
                uintptr_t boneMatrix = *reinterpret_cast<uintptr_t*>(targetEntity + offset::m_dwBoneMatrix);

                Vector3 targetHead;
                targetHead.x = *reinterpret_cast<float*>(boneMatrix + 0x30 * BoneID::HEAD + 0x0C);
                targetHead.y = *reinterpret_cast<float*>(boneMatrix + 0x30 * BoneID::HEAD + 0x1C);
                targetHead.z = *reinterpret_cast<float*>(boneMatrix + 0x30 * BoneID::HEAD + 0x2C);

                // Calculate optimal angle
                Vector3 angleToHead = CalculateAngle(localEyes, targetHead);

                // Read current view angles using multi-level pointer from dump
                uintptr_t clientStateBase = *reinterpret_cast<uintptr_t*>(engine + offset::dwClientState_Base);
                uintptr_t viewAnglesBase = *reinterpret_cast<uintptr_t*>(clientStateBase + 0x8);
                Vector3 currentViewAngles = *reinterpret_cast<Vector3*>(viewAnglesBase + 0x8C);

                // Read aim punch
                Vector3 aimPunchAngles = *reinterpret_cast<Vector3*>(localPlayer + offset::m_aimPunchAngle);

                // Adjust view angles by recoil
                Vector3 trueAimAngles;
                trueAimAngles.x = currentViewAngles.x + (aimPunchAngles.x * 2.0f);
                trueAimAngles.y = currentViewAngles.y + (aimPunchAngles.y * 2.0f);
                trueAimAngles.z = 0.0f;

                // Calculate distance and dynamic radius
                float angularDistance = GetAngleDistance(trueAimAngles, angleToHead);

                float distance3D = Get3DDistance(localEyes, targetHead);
                if (distance3D == 0.0f) distance3D = 1.0f;

                float dynamicHitboxRadius = std::asin(HEAD_RADIUS_UNITS / distance3D) * (180.0f / 3.14159265358979323846f);

                // Attack Execution
                if (angularDistance <= dynamicHitboxRadius)
                {
                    uint32_t attackFlag = *reinterpret_cast<uint32_t*>(client + offset::m_dwForceAttack);

                    if (attackFlag == ATTACK_RELEASE)
                    {
                        *reinterpret_cast<uint32_t*>(client + offset::m_dwForceAttack) = ATTACK_PRESS;
                        std::this_thread::sleep_for(std::chrono::microseconds(1));

                        *reinterpret_cast<uint32_t*>(client + offset::m_dwForceAttack) = ATTACK_RELEASE;
                        std::this_thread::sleep_for(std::chrono::microseconds(1));
                    }
                }
            }
        }
    }
}