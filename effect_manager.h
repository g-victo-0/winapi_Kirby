#pragma once

#include <windows.h>
#include <gdiplus.h>

void ResetStageAtmosphereEffects();
void UpdateStageAtmosphereEffects(int currentStage);
void DrawStageAtmosphereEffects(
    Gdiplus::Graphics& graphics,
    int currentStage,
    int cameraX,
    int screenW,
    int screenH
);
