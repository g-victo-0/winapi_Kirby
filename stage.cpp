// Stage data, doors, rescue objects, monster placement, and clear dance stage
// Included by game.cpp to keep the existing global-state gameplay unchanged.

// Stage 1 map data and rescue setup
// This file is included by game.cpp. Do not add it to ClCompile separately.

SolidBlock g_solidBlocks[] =
{
    // =========================
    // PNG22 첫 번째 맵 충돌체
    // 예전에 쓰던 충돌체 다시 유지
    // =========================
    { { 0, 545, 289, 613 }, L"MAP1_GROUND_1" },
    { { 132, 512, 237, 620 }, L"MAP1_BLOCK_1" },
    { { 280, 545, 600, 616 }, L"MAP1_GROUND_2" },
    { { 404, 512, 449, 616 }, L"MAP1_PILLAR" },
    //{ { 528, 545, 611, 632 }, L"MAP1_GROUND_3" }, 사용 X 
    { { 594, 545, 1018, 618 }, L"MAP1_MONSTER_AREA" },

    // =========================
    // PNG23 두 번째 맵 충돌체
    // PNG23은 월드 x = 1000부터 시작하므로, 화면에서 보이는 x좌표에 +1000을 해줌
    // 사진 보고 대충 맞춘 값이라 F1 눌러서 좌표 확인하면서 조금씩 조절하면 됨
    // =========================

    // 왼쪽 아래 긴 땅
    { { 1000, 545, 1666, 618 }, L"MAP2_LEFT_GROUND" },

    // 가운데 아래 바위 기둥
    { { 1617, 453, 1777, 618 }, L"MAP2_SMALL_ROCK" },

    // 오른쪽 큰 절벽/벽
    { { 1700, 135, 2000, 618 }, L"MAP2_BIG_CLIFF" },

    // 오른쪽 큰 절벽 위에서 왼쪽으로 튀어나온 발판 부분
    { { 1570, 114, 2000, 195 }, L"MAP2_TOP_LEDGE" },

    // 나무 발판들
    { { 1328, 439, 1750, 470 }, L"MAP2_WOOD_1" },
    { { 1381, 339, 1555, 365 }, L"MAP2_WOOD_2" },
    { { 1381, 239, 1556, 265 }, L"MAP2_WOOD_3" },
    { { 1108, 185, 1339, 210 }, L"MAP2_WOOD_4" },
    { { 1320, 112, 1501, 136 }, L"MAP2_WOOD_5" }
};

int g_solidBlockCount = sizeof(g_solidBlocks) / sizeof(g_solidBlocks[0]);

// =========================
// 2스테이지 충돌체: 88번(0~999), 89번(1000~1999)
// 대충 잡은 값이라 F1 디버그 켜고 조금씩 조정하면 됨
// =========================

void InitStage1RescueObjects()
{
    // 81번 남자 아이: 두 번째 나무 발판 위쪽에 배치
    g_stage1Boy.active = true;
    g_stage1Boy.rescued = false;
    g_stage1Boy.w = 47;
    g_stage1Boy.h = 59;
    g_stage1Boy.x = 1450;
    g_stage1Boy.y = 339 - g_stage1Boy.h + 7;

    // 84~87번 문: 1스테이지 맨 오른쪽 언덕 위에 배치
    g_stage1Door.active = true;
    g_stage1Door.opening = false;
    g_stage1Door.opened = false;
    g_stage1Door.w = 81;
    g_stage1Door.h = 101;
    g_stage1Door.x = 1868;
    g_stage1Door.y = 114 - g_stage1Door.h + 7;
    g_stage1Door.frameIndex = 0;
    g_stage1Door.tick = 0;

    g_stage1ChildTotal = 1;
    g_stage1ChildRescued = 0;
}

// Stage 2 map data and rescue setup
// This file is included by game.cpp. Do not add it to ClCompile separately.

SolidBlock g_stage2SolidBlocks[] =
{
    // PNG88 달 있는 앞쪽 맵
    { { 0, 482, 250, 650 }, L"S2_FRONT_LEFT_GROUND" },
    { { 345, 482, 1000, 650 }, L"S2_FRONT_MIDDLE_PLATFORM" },
    { { 265, 482, 333, 650 }, L"S2_FRONT_BOTTOM_GROUND" },
    { { 424, 329, 685, 650 }, L"S2_FRONT_ROCK" },
    { { 908, 329, 1000, 650 }, L"S2_FRONT_RIGHT_LOW" },

    // 발판 
    { { 76, 225, 180, 261 }, L"MAP_WOOD1" },

    { { 180, 300, 262, 334 }, L"MAP_WOOD2" },
    { { 732, 233, 851, 270 }, L"MAP_WOOD3" },

    // PNG89 달 없는 뒤쪽 맵

    { { 995, 358, 1230, 650 }, L"S2_BACK_LEFT_GROUND" },


    { { 1000, 537, 2000, 650 }, L"S2_BACK_SMALL_PILLAR" },
    { { 1703, 529, 1771, 650 }, L"S2_BACK_LEFT_LOW" },
    { { 1810,529, 1880, 650 }, L"S2_BACK_CENTER_BUILDING" },

    { { 1730, 330, 2000, 391 }, L"S2_BACK_RIGHT_LOW" },

    //발판
    { { 1330, 283, 1581, 337 }, L"MAP_WOOD4" },

};

int g_stage2SolidBlockCount = sizeof(g_stage2SolidBlocks) / sizeof(g_stage2SolidBlocks[0]);

// =========================
// 3스테이지 충돌체: 90번(0~999), 91번(1000~1999)
// 90/91 배경을 BG_PART_W x BG_PART_H로 늘려 그리는 기준 좌표
// =========================

void InitStage2RescueObjects()
{
    for (int i = 0; i < STAGE2_CHILD_COUNT; i++)
    {
        g_stage2Children[i].active = true;
        g_stage2Children[i].rescued = false;
        g_stage2Children[i].w = 47;
        g_stage2Children[i].h = 59;
    }

    // 88번 달 있는 앞쪽 맵
    g_stage2Children[0].x = 105;
    g_stage2Children[0].y = 370 - g_stage2Children[0].h + 7;

    g_stage2Children[1].x = 530;
    g_stage2Children[1].y = 342 - g_stage2Children[1].h + 7;

    // 89번 달 없는 뒤쪽 맵
    g_stage2Children[2].x = 1435;
    g_stage2Children[2].y = 286 - g_stage2Children[2].h + 7;

    g_stage2Children[3].x = 1931;
    g_stage2Children[3].y = 546 - g_stage2Children[3].h + 7;

    // 89번 달 없는 맵의 오른쪽 위 빨간 표시 위치에 문 배치
    g_stage2Door.active = true;
    g_stage2Door.opening = false;
    g_stage2Door.opened = false;
    g_stage2Door.w = 81;
    g_stage2Door.h = 101;
    g_stage2Door.x = 1871;
    g_stage2Door.y = 332 - g_stage2Door.h + 7;
    g_stage2Door.frameIndex = 0;
    g_stage2Door.tick = 0;

    g_stage2ChildTotal = STAGE2_CHILD_COUNT;
    g_stage2ChildRescued = 0;
}

// Stage 3 map data and rescue setup
// This file is included by game.cpp. Do not add it to ClCompile separately.

SolidBlock g_stage3SolidBlocks[] =
{
    // =========================
    // 3스테이지 충돌체 수정본
    // 큰 네모로 막지 않고, 실제 발판/기둥 위주로 작게 잡음
    // F1 디버그 빨간 박스가 너무 커 보이던 부분을 줄임
    // =========================

    // PNG90 첫 번째 구간: x = 0 ~ 999
    { { 0, 585, 265, 650 }, L"S3_90_LEFT_BOTTOM_GROUND" },
    { { 410, 586, 464, 650 }, L"S3_90_LEFT_LEDGE" },
    { { 498, 565, 553, 650 }, L"S3_90_CENTER_LONG_LEDGE" },
    { { 644, 539, 702, 650 }, L"S3_90_CENTER_GROUND" },
    { { 718, 539, 791, 650 }, L"S3_90_CENTER_PILLAR" },
    { { 806, 539, 875, 650 }, L"S3_90_RIGHT_LEDGE" },
    { { 892, 539, 1015, 650 }, L"S3_90_RIGHT_GROUND" },


    // 화면 끝에 걸리는 정도만 막는 얇은 벽
    // 예전처럼 x=0~115, x=945~1000 전체를 막으면 충돌범위가 너무 어색하게 커짐
    { { 0, 0, 12, 650 }, L"S3_90_LEFT_LIMIT" },

    // PNG91 second area: x = 1000 ~ 1999
    { { 1000, 536, 2000, 650 }, L"S3_90_TOP_LEDGE" },
    { { 1182, 406, 2000, 650 }, L"S3_91_LEFT_BOTTOM_GROUND" },
    { { 1137, 476, 1190, 491 }, L"S3_91_MAIN_LEDGE_WOOD" },
    { { 1100, 406, 1187, 424 }, L"S3_91_MAIN_GROUNDWOOD" },
    { { 1175, 406, 2000, 650 }, L"S3_91_CENTER_PILLAR" },

    { { 1852, 280, 1900, 402 }, L"S3_91_RIGHT_LEDGE" },
    { { 1635, 425, 2000, 650 }, L"S3_91_RIGHT_GROUND" },
    { { 1660, 280, 1723, 405 }, L"S3_91_RIGHT_OBSTACLE" },
    { { 1660, 280, 1736, 348 }, L"S3_91_RIGHT_OBSTACLE_2" },
    { { 1761, 280, 1900, 348 }, L"S3_91_RIGHT_WALL" },
    { { 1805, 123, 1890, 146 }, L"S3_91_TOP_LEDGE" },

    // Restored from commit 9a72c7f: upper floors and right-end wall for stage 3.
    { { 1890, 0, 1980, 405 }, L"S3_91_RIGHTEND_WALL" },
    { { 995, 0, 1085, 490 }, L"S3_91_FLOOR1" },
    { { 995, 0, 1115, 460 }, L"S3_91_FLOOR2" },
    { { 1100, 310, 1130, 340 }, L"S3_91_FLOOR3" },
    { { 1125, 230, 1265, 315 }, L"S3_91_FLOOR4" },
    { { 1255, 200, 1295, 245 }, L"S3_91_FLOOR5" },
    { { 1288, 190, 1530, 222 }, L"S3_91_FLOOR6" },
    { { 1520, 170, 1545, 195 }, L"S3_91_FLOOR7" },
    { { 1535, 135, 1730, 175 }, L"S3_91_FLOOR8" },
    { { 1725, 45, 1745, 150 }, L"S3_91_FLOOR9" },
    { { 1735, 0, 1773, 55 }, L"S3_91_FLOOR10" },
    { { 1770, 0, 1910, 35 }, L"S3_91_FLOOR11" }

};

int g_stage3SolidBlockCount = sizeof(g_stage3SolidBlocks) / sizeof(g_stage3SolidBlocks[0]);

// =========================
// 5스테이지 임시 춤 확인 맵
// 보스전 문을 열고 들어가면 여기로 넘어와서 춤만 확인함
// =========================

void InitStage3RescueObjects()
{
    // 3스테이지 오른쪽 위쪽 발판에 문 배치
    g_stage3Door.active = true;
    g_stage3Door.opening = false;
    g_stage3Door.opened = true;
    g_stage3Door.w = 81;
    g_stage3Door.h = 101;
    g_stage3Door.x = 1810;
    g_stage3Door.y = 126 - g_stage3Door.h + 7;
    g_stage3Door.frameIndex = DOOR_FRAME_COUNT - 1;
    g_stage3Door.tick = 0;
}

// Stage 4 boss map data
// This file is included by game.cpp. Do not add it to ClCompile separately.

SolidBlock g_stage4SolidBlocks[] =
{
    // 4스테이지는 92번 배경 하나만 사용하는 보스전 맵이라 x = 0 ~ 1000까지만 사용
    { { 0, 545, 1000, 650 }, L"S4_BOSS_GROUND" },
    { { 0, 0, 12, 650 }, L"S4_LEFT_LIMIT" },
    { { 988, 0, 1000, 650 }, L"S4_RIGHT_LIMIT" }
};

int g_stage4SolidBlockCount = sizeof(g_stage4SolidBlocks) / sizeof(g_stage4SolidBlocks[0]);

// Stage 5 clear map data
// This file is included by game.cpp. Do not add it to ClCompile separately.

SolidBlock g_stage5SolidBlocks[] =
{
    { { 0, DANCE_FLOOR_Y, 1000, 650 }, L"S5_DANCE_GROUND" },
    { { 0, 0, 12, 650 }, L"S5_LEFT_LIMIT" },
    { { 988, 0, 1000, 650 }, L"S5_RIGHT_LIMIT" }
};

int g_stage5SolidBlockCount = sizeof(g_stage5SolidBlocks) / sizeof(g_stage5SolidBlocks[0]);

// =========================
// 4스테이지 충돌체: 92번 보스전 배경
// 보스전용이라 바닥 하나만 길게 깔아둠
// =========================

SolidBlock* GetCurrentSolidBlocks(int* count)
{
    if (g_currentStage == 2)
    {
        *count = g_stage2SolidBlockCount;
        return g_stage2SolidBlocks;
    }

    if (g_currentStage == 3)
    {
        *count = g_stage3SolidBlockCount;
        return g_stage3SolidBlocks;
    }

    if (g_currentStage == 4)
    {
        *count = g_stage4SolidBlockCount;
        return g_stage4SolidBlocks;
    }

    if (g_currentStage == 5)
    {
        *count = g_stage5SolidBlockCount;
        return g_stage5SolidBlocks;
    }

    *count = g_solidBlockCount;
    return g_solidBlocks;
}

int GetCurrentWorldW()
{
    if (g_currentStage == 4 || g_currentStage == 5)
        return BG_PART_W;

    return WORLD_W;
}

RECT GetChildRect(RescueChild child)
{
    RECT rc;
    rc.left = child.x;
    rc.top = child.y;
    rc.right = child.x + child.w;
    rc.bottom = child.y + child.h;
    return rc;
}

RECT GetDoorRect(StageDoor door)
{
    RECT rc;
    rc.left = door.x;
    rc.top = door.y;
    rc.right = door.x + door.w;
    rc.bottom = door.y + door.h;
    return rc;
}

void InitRescueObjects()
{
    if (g_currentStage == 2)
        InitStage2RescueObjects();
    else if (g_currentStage == 3)
        InitStage3RescueObjects();
    else if (g_currentStage == 4)
    {
        // 4스테이지는 보스전 배경만 먼저 사용. 문/학생 없음.
    }
    else if (g_currentStage == 5)
    {
        // 5스테이지는 춤 확인용 임시맵이라 문/학생 없음.
        ResetDanceStage();
    }
    else
        InitStage1RescueObjects();

    g_isChangingMap = false;
    g_rescueAnimTick = 0;
}

void CheckRescueChildTouch()
{
    RECT kirbyRc = GetKirbyBodyRect();

    if (g_currentStage == 1)
    {
        if (!g_stage1Boy.active || g_stage1Boy.rescued)
            return;

        RECT childRc = GetChildRect(g_stage1Boy);

        if (IsRectHit(kirbyRc, childRc))
        {
            g_stage1Boy.active = false;
            g_stage1Boy.rescued = true;
            g_stage1ChildRescued++;
            StartRescueEffect(g_stage1Boy.x + g_stage1Boy.w / 2, g_stage1Boy.y + g_stage1Boy.h / 2);

            if (g_stage1ChildRescued >= g_stage1ChildTotal)
            {
                g_stage1Door.opening = true;
            }
        }
        return;
    }

    if (g_currentStage == 2)
    {
        for (int i = 0; i < STAGE2_CHILD_COUNT; i++)
        {
            if (!g_stage2Children[i].active || g_stage2Children[i].rescued)
                continue;

            RECT childRc = GetChildRect(g_stage2Children[i]);

            if (IsRectHit(kirbyRc, childRc))
            {
                g_stage2Children[i].active = false;
                g_stage2Children[i].rescued = true;
                g_stage2ChildRescued++;
                StartRescueEffect(g_stage2Children[i].x + g_stage2Children[i].w / 2, g_stage2Children[i].y + g_stage2Children[i].h / 2);

                if (g_stage2ChildRescued >= g_stage2ChildTotal)
                {
                    g_stage2Door.opening = true;
                }
            }
        }
    }
}

void UpdateDoorOpen(StageDoor* door)
{
    if (door == NULL)
        return;

    if (door->opening && !door->opened)
    {
        door->tick++;

        if (door->tick >= DOOR_OPEN_FRAME_TICK)
        {
            door->tick = 0;

            if (door->frameIndex < DOOR_FRAME_COUNT - 1)
            {
                door->frameIndex++;
            }
            else
            {
                door->opened = true;
                door->opening = false;
                PlayGameSound(SFX_DOOR);
                StartStageClearMessage();
            }
        }
    }
}

void UpdateRescueObjects()
{
    g_rescueAnimTick++;

    if (g_currentStage == 1)
    {
        if (g_stage1ChildRescued >= g_stage1ChildTotal && !g_stage1Door.opened)
        {
            g_stage1Door.opening = true;
        }

        UpdateDoorOpen(&g_stage1Door);
        return;
    }

    if (g_currentStage == 2)
    {
        if (g_stage2ChildRescued >= g_stage2ChildTotal && !g_stage2Door.opened)
        {
            g_stage2Door.opening = true;
        }

        UpdateDoorOpen(&g_stage2Door);
        return;
    }

    if (g_currentStage == 3)
    {
        UpdateDoorOpen(&g_stage3Door);
    }
}

void ChangeStageNow(HWND hWnd, int targetStage)
{
    if (targetStage == 2)
    {
        g_currentStage = 2;
        kirbyX = 70;
        kirbyY = 330;
        kirbyVY = 0.0f;
        g_lastSafeKirbyX = kirbyX;
        g_lastSafeKirbyY = kirbyY;
        cameraX = 0;

        InitRescueObjects();
        InitMonsters();
        UpdateCamera(hWnd);
        ResetStageGimmicks();
        StartStageTransitionEffect();
        return;
    }

    if (targetStage == 3)
    {
        g_currentStage = 3;
        kirbyX = 145;
        kirbyY = 500;
        kirbyVY = 0.0f;
        g_lastSafeKirbyX = kirbyX;
        g_lastSafeKirbyY = kirbyY;
        cameraX = 0;

        InitRescueObjects();
        InitMonsters();
        UpdateCamera(hWnd);
        ResetStageGimmicks();
        StartStageTransitionEffect();
        return;
    }

    if (targetStage == 4)
    {
        g_currentStage = 4;
        kirbyX = 80;
        kirbyY = 480;
        kirbyVY = 0.0f;
        g_lastSafeKirbyX = kirbyX;
        g_lastSafeKirbyY = kirbyY;
        cameraX = 0;

        InitRescueObjects();
        InitMonsters();
        UpdateCamera(hWnd);
        ResetStageGimmicks();
        StartStageTransitionEffect();
        return;
    }

    if (targetStage == 5)
    {
        g_currentStage = 5;
        g_stageClearTick = 0;
        kirbyX = DANCE_CENTER_X - NORMAL_KIRBY_W / 2;
        kirbyY = DANCE_FLOOR_Y - NORMAL_KIRBY_H;
        kirbyVY = 0.0f;
        g_lastSafeKirbyX = kirbyX;
        g_lastSafeKirbyY = kirbyY;
        cameraX = 0;

        SetKirbyNormalSizeKeepBottom();
        InitRescueObjects();
        InitMonsters();
        UpdateCamera(hWnd);
        ResetStageGimmicks();
        StartStageTransitionEffect();
        return;
    }
}

void GoNextMap(HWND hWnd)
{
    if (g_isChangingMap || g_starTransitionActive)
        return;

    int targetStage = g_currentStage + 1;

    if (targetStage > 5)
        return;

    StartStarStageTransition(hWnd, targetStage);
}

void CheckDoorTouch(HWND hWnd)
{
    RECT kirbyRc = GetKirbyBodyRect();

    if (g_currentStage == 1)
    {
        if (!g_stage1Door.active || !g_stage1Door.opened)
            return;

        RECT doorRc = GetDoorRect(g_stage1Door);
        if (IsRectHit(kirbyRc, doorRc))
        {
            GoNextMap(hWnd);
        }
        return;
    }

    if (g_currentStage == 2)
    {
        if (!g_stage2Door.active || !g_stage2Door.opened)
            return;

        RECT doorRc = GetDoorRect(g_stage2Door);
        if (IsRectHit(kirbyRc, doorRc))
        {
            GoNextMap(hWnd);
        }
        return;
    }

    if (g_currentStage == 3)
    {
        if (!g_stage3Door.active || !g_stage3Door.opened)
            return;

        RECT doorRc = GetDoorRect(g_stage3Door);
        if (IsRectHit(kirbyRc, doorRc))
        {
            GoNextMap(hWnd);
        }
        return;
    }

    if (g_currentStage == 4)
    {
        // 보스 처치 후 열쇠로 문을 연 상태에서 문에 닿으면 춤 테스트 맵으로 이동
        if (!g_rewardDoorActive || !g_rewardDoorOpened)
            return;

        RECT doorRc = MakeRectFromXYWH(g_rewardDoorX, g_rewardDoorY, g_rewardDoorW, g_rewardDoorH);
        if (IsRectHit(kirbyRc, doorRc))
        {
            GoNextMap(hWnd);
        }
    }
}

void DrawDoorObject(Graphics& graphics, StageDoor door)
{
    if (!door.active)
        return;

    int frame = door.frameIndex;
    if (frame < 0) frame = 0;
    if (frame >= DOOR_FRAME_COUNT) frame = DOOR_FRAME_COUNT - 1;

    DrawWorldImage(graphics, g_doorFrames[frame], door.x, door.y, door.w, door.h);
}

void DrawChildObject(Graphics& graphics, RescueChild child, int frameType)
{
    if (!child.active || child.rescued)
        return;

    Image* childFrame = g_studentBoyFrame;
    if (frameType == 83 && g_studentGirlFrame != NULL)
        childFrame = g_studentGirlFrame;

    int shakeX = (g_rescueAnimTick / 6) % 2 == 0 ? -1 : 1;
    int bobY = (g_rescueAnimTick / 10) % 2 == 0 ? 0 : -1;
    DrawWorldImage(graphics, childFrame, child.x + shakeX, child.y + bobY, child.w, child.h);
}

void DrawRescueObjects(Graphics& graphics)
{
    if (g_currentStage == 1)
    {
        DrawDoorObject(graphics, g_stage1Door);
        DrawChildObject(graphics, g_stage1Boy, 81);
        return;
    }

    if (g_currentStage == 2)
    {
        DrawDoorObject(graphics, g_stage2Door);

        for (int i = 0; i < STAGE2_CHILD_COUNT; i++)
        {
            DrawChildObject(graphics, g_stage2Children[i], g_stage2ChildFrameType[i]);
        }
        return;
    }

    if (g_currentStage == 3)
    {
        DrawDoorObject(graphics, g_stage3Door);
    }
}

void InitMonsters()
{
    for (int i = 0; i < MONSTER_COUNT; i++)
    {
        g_monsters[i].active = false;
        g_monsters[i].isDeadEffect = false;
    }

    if (g_currentStage == 1)
    {
        // 1스테이지: 기존 구성 유지, 폭탄병은 생성하지 않음
        g_monsters[0].Init(666, 470, 606, 1015, -1);
        g_monsters[1].Init(1200, 470, 1034, 1611, -1);
        g_monsters[2].Init(1488, 379, 1378, 1666, 1);
        // 불속성 몬스터는 문 앞쪽 빨간 원으로 표시한 구간만 돌아다니게 제한
        g_monsters[3].Init(1805, 100, 1720, 1860, -1, 1);
        g_monsters[4].active = false;
        return;
    }

    if (g_currentStage == 2)
    {
        // 2스테이지: 일반몹 2마리, 불몹 2마리, 폭탄몹 1마리
        g_monsters[0].Init(95, 320, 10, 230, 1, 0);
        g_monsters[1].Init(455, 235, 330, 580, -1, 0);
        g_monsters[2].Init(1450, 285, 1425, 1685, -1, 1);
        g_monsters[3].Init(1815, 360, 1685, 1885, 1, 1);
        g_monsters[4].Init(1780, 145, 1690, 1880, 1, 2);
        return;
    }

    if (g_currentStage == 3)
    {
        // 3스테이지: 날아다니는 폭탄 몬스터 3마리
        g_monsters[0].Init(300, 255, 255, 640, 1, 2);
        g_monsters[1].Init(820, 150, 790, 945, -1, 2);
        g_monsters[2].Init(1320, 250, 1185, 1495, 1, 2);
        g_monsters[3].Init(1770, 150, 1635, 1900, -1, 2);
        g_monsters[3].active = false; // 요청대로 실제 배치는 3마리만 사용
        g_monsters[4].active = false;
        return;
    }

    if (g_currentStage == 4)
    {
        // 4스테이지 보스전: 일반 몬스터는 쓰지 않고 보스만 초기화
        InitBossObjects();
        return;
    }

    if (g_currentStage == 5)
    {
        // 5스테이지는 춤 확인용이라 몬스터 없음
        return;
    }
}

struct DanceFrame
{
    int frameIndex;
    int duration;
    bool flipX;
    int xOffset;
    int yOffset;
};

// frameIndex is zero-based: 0 means "커비 댄스1.png", 22 means "커비 댄스23.png".
// The base Y is fixed; yOffset is used only on jump/bounce frames and stays 0 on still poses.
const DanceFrame g_danceSequence[] =
{
    // Opening beat: front pose, squash, then rise. These stay grounded.
    { 0, 10, false, 0, 0 },
    { 1, 10, false, 0, 0 },
    { 14, 8, false, 0, 0 },
    { 9, 7, false, 0, 0 },
    { 10, 7, false, 0, 0 },
    { 11, 8, false, 0, 0 },
    { 12, 10, false, 0, 0 },

    // Clockwise turn: rise and land like the reference dance.
    { 15, 5, false, 4, -3 },
    { 16, 5, false, 9, -8 },
    { 17, 5, false, 14, -14 },
    { 18, 5, false, 19, -18 },
    { 19, 5, false, 24, -14 },
    { 20, 5, false, 29, -8 },
    { 21, 8, false, 34, -3 },

    // Counter-turn: same turn frames, mirrored on draw only, with the same bounce arc.
    { 20, 5, true, 29, -3 },
    { 19, 5, true, 24, -8 },
    { 18, 5, true, 19, -14 },
    { 17, 5, true, 14, -18 },
    { 16, 5, true, 9, -14 },
    { 15, 8, true, 4, -6 },

    // Right-side dance accents: grounded poses, with only the step frames bouncing.
    { 4, 7, false, 44, 0 },
    { 5, 7, false, 52, -4 },
    { 6, 7, false, 58, -7 },
    { 7, 8, false, 52, -4 },
    { 11, 8, false, 44, 0 },
    { 12, 10, false, 36, 0 },
    { 14, 8, false, 30, 0 },
    { 10, 8, false, 24, 0 },

    // Left-side dance accents: same rhythm mirrored; still frames keep yOffset at 0.
    { 4, 7, true, -44, 0 },
    { 5, 7, true, -52, -4 },
    { 6, 7, true, -58, -7 },
    { 7, 8, true, -52, -4 },
    { 11, 8, true, -44, 0 },
    { 12, 10, true, -36, 0 },
    { 14, 8, true, -30, 0 },
    { 10, 8, true, -24, 0 },

    // Return to center: one small hop, then grounded finish.
    { 0, 8, false, -14, -3 },
    { 1, 8, false, -8, -5 },
    { 4, 8, false, 0, -2 },
    { 11, 9, false, 0, 0 },
    { 14, 9, false, 0, 0 },
    { 22, 28, false, 0, 0 }
};
const int g_danceSequenceCount = sizeof(g_danceSequence) / sizeof(g_danceSequence[0]);

int GetDanceSequenceTotalTicks()
{
    int total = 0;

    for (int i = 0; i < g_danceSequenceCount; i++)
    {
        total += g_danceSequence[i].duration;
    }

    return total;
}

const DanceFrame* GetDanceFrameByTick(int tick)
{
    int elapsed = 0;

    for (int i = 0; i < g_danceSequenceCount; i++)
    {
        int next = elapsed + g_danceSequence[i].duration;

        if (tick < next)
        {
            g_danceFrameTick = tick - elapsed;
            return &g_danceSequence[i];
        }

        elapsed = next;
    }

    g_danceFrameTick = 0;
    return &g_danceSequence[g_danceSequenceCount - 1];
}

void ApplyDanceFrameState()
{
    const DanceFrame* frame = GetDanceFrameByTick(g_danceTick);

    g_danceFrameIndex = frame->frameIndex;
    g_danceX = DANCE_CENTER_X + frame->xOffset;
    g_danceY = DANCE_FLOOR_Y - DANCE_DRAW_H + frame->yOffset;

    kirbyX = g_danceX - kirbyW / 2;
    kirbyY = DANCE_FLOOR_Y - kirbyH;
    cameraX = 0;
}

void ResetDanceStage()
{
    g_danceFrameIndex = 0;
    g_danceFrameTick = 0;
    g_danceTick = 0;
    g_danceAngle = 0.0f;
    g_danceFinished = false;

    ApplyDanceFrameState();
    StopMove();
}

void UpdateDanceStage()
{
    if (g_currentStage != 5)
        return;

    StopMove();

    if (g_danceFinished)
    {
        g_danceTick = GetDanceSequenceTotalTicks() - 1;
        ApplyDanceFrameState();
        return;
    }

    g_danceTick++;

    int totalTicks = GetDanceSequenceTotalTicks();

    if (g_danceTick >= totalTicks)
    {
        g_danceTick = totalTicks - 1;
        g_danceFinished = true;
    }

    ApplyDanceFrameState();
}

void DrawDanceKirby(Graphics& graphics)
{
    if (g_currentStage != 5)
        return;

    const DanceFrame* danceFrame = GetDanceFrameByTick(g_danceTick);

    int frameIndex = danceFrame->frameIndex;
    if (frameIndex < 0)
        frameIndex = 0;
    if (frameIndex >= DANCE_FRAME_COUNT)
        frameIndex = DANCE_FRAME_COUNT - 1;

    int drawX = g_danceX - DANCE_DRAW_W / 2;
    int drawY = DANCE_FLOOR_Y - DANCE_DRAW_H + danceFrame->yOffset;

    Image* frame = g_idleFrame;

    if (g_danceFrames[frameIndex] != NULL)
        frame = g_danceFrames[frameIndex];

    if (danceFrame->flipX)
    {
        DrawImageFlipX(graphics, frame, drawX, drawY, DANCE_DRAW_W, DANCE_DRAW_H);
        return;
    }

    DrawWorldImage(graphics, frame, drawX, drawY, DANCE_DRAW_W, DANCE_DRAW_H);
}
