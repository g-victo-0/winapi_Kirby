#pragma once

#include <windows.h>
#include <gdiplus.h>
#include "collision.h"

struct RescueChild
{
    bool active;
    bool rescued;
    int x;
    int y;
    int w;
    int h;
};

struct StageDoor
{
    bool active;
    bool opening;
    bool opened;
    int x;
    int y;
    int w;
    int h;
    int frameIndex;
    int tick;
};

SolidBlock* GetCurrentSolidBlocks(int* count);
int GetCurrentWorldW();
void InitRescueObjects();
void CheckRescueChildTouch();
void UpdateRescueObjects();
void CheckDoorTouch(HWND hWnd);
void TryStageDoorInteraction();
void DrawRescueObjects(Gdiplus::Graphics& graphics);
void DrawStageKeyObjects(Gdiplus::Graphics& graphics);
void InitMonsters();
void ResetDanceStage();
void UpdateDanceStage();
void DrawDanceKirby(Gdiplus::Graphics& graphics);
