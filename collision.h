#pragma once

#include <windows.h>

struct SolidBlock
{
    RECT rc;
    LPCWSTR name;
};

RECT MakeRectFromXYWH(int x, int y, int w, int h);
bool IsRectHit(RECT a, RECT b);
bool HitSolidBlock(RECT rc, RECT* hitBlock);
bool HitSolidBlockForBalloon(RECT rc, RECT* hitBlock);
bool FindGroundUnderHitBox(RECT hitBox, int* groundY);
RECT GetKirbyHitBox(int x, int y);
RECT GetMonsterHitBox(int x, int y, int w, int h);
