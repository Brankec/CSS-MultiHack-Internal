#pragma once

#include <thread>
#include <chrono>
#include <cstdint>
#include "globals.h"
#include "../ext/imgui/imgui.h"

namespace crosshair
{
    void DrawCenterCrosshair()
    {
        if (!globals::bCrosshair)
        {
            return;
        }

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();

        ImVec2 screenSize = ImGui::GetIO().DisplaySize;
        ImVec2 screenCenter = ImVec2(screenSize.x / 2.0f, screenSize.y / 2.0f);

        // Define circle properties
        float radius = 15.0f;
        float thickness = 2.0f;
        ImU32 color = IM_COL32(255, 0, 0, 255); // Red color (RGBA)
        int segments = 12; // Smoothness of the circle

        drawList->AddCircle(screenCenter, radius, color, segments, thickness);

        // Draw a black outline for better visibility against bright backgrounds
        drawList->AddCircle(screenCenter, radius + 1.0f, IM_COL32(0, 0, 0, 255), segments, 1.0f);
    }

}