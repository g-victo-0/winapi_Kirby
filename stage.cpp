// 스테이지 데이터, 문, 구출 오브젝트, 몬스터 배치, 클리어 춤 스테이지
// 기존 전역 상태 게임 구조를 유지하기 위해 game.cpp에서 포함해서 사용함.

// 1스테이지 맵 데이터와 구출 설정
// 이 파일은 game.cpp에서 포함해서 사용함. Visual Studio 컴파일 항목에 따로 넣지 말 것.

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
    { { 1575, 453, 1777, 618 }, L"MAP2_SMALL_ROCK" },

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

// 2스테이지 맵 데이터와 구출 설정
// 이 파일은 game.cpp에서 포함해서 사용함. Visual Studio 컴파일 항목에 따로 넣지 말 것.

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


    { { 1000, 537, 1710, 650 }, L"S2_BACK_SMALL_PILLAR" },
    { { 1703, 529, 1771, 650 }, L"S2_BACK_LEFT_LOW" },
    { { 1810,529, 1880, 650 }, L"S2_BACK_CENTER_BUILDING" },
    { { 1910,546, 2000, 650 }, L"S2_StudentGround" },
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
    g_stage2Children[0].y = 225 - g_stage2Children[0].h + 7;

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

// 3스테이지 맵 데이터와 구출 설정
// 이 파일은 game.cpp에서 포함해서 사용함. Visual Studio 컴파일 항목에 따로 넣지 말 것.

SolidBlock g_stage3SolidBlocks[] =
{
    // =========================
    // 3스테이지 충돌체 수정본
    // 큰 네모로 막지 않고, 실제 발판/기둥 위주로 작게 잡음
    // F1 디버그 빨간 박스가 너무 커 보이던 부분을 줄임
    // =========================

    // PNG90 첫 번째 구간: x = 0 ~ 999
    { { 0, 585, 265, 650 }, L"S3_90_LEFT_BOTTOM_GROUND" },
    { { 410, 586, 475, 650 }, L"S3_90_LEFT_LEDGE" },
    { { 498, 565, 563, 650 }, L"S3_90_CENTER_LONG_LEDGE" },
    { { 650, 539, 712, 650 }, L"S3_90_CENTER_GROUND" },
    { { 735, 539, 800, 650 }, L"S3_90_CENTER_PILLAR" },
    { { 825, 539, 895, 650 }, L"S3_90_RIGHT_LEDGE" },
    { { 920, 539, 1015, 650 }, L"S3_90_RIGHT_GROUND" },


    // 화면 끝에 걸리는 정도만 막는 얇은 벽
    // 예전처럼 x=0~115, x=945~1000 전체를 막으면 충돌범위가 너무 어색하게 커짐
    { { 0, 0, 12, 650 }, L"S3_90_LEFT_LIMIT" },

    // PNG91 두 번째 구간: x = 1000 ~ 1999
    { { 1000, 539, 2000, 650 }, L"S3_90_TOP_LEDGE" },
    { { 1182, 406, 2000, 650 }, L"S3_91_LEFT_BOTTOM_GROUND" },
    { { 1137, 476, 1190, 491 }, L"S3_91_MAIN_LEDGE_WOOD" },
    { { 1100, 406, 1187, 424 }, L"S3_91_MAIN_GROUNDWOOD" },
    // { { 1175, 406, 2000, 650 }, L"S3_91_CENTER_PILLAR" } 필요 없어서 없앰,

     { { 1852, 285, 1900, 402 }, L"S3_91_RIGHT_LEDGE" },
     { { 1635, 425, 2000, 650 }, L"S3_91_RIGHT_GROUND" },
     { { 1660, 285, 1723, 405 }, L"S3_91_RIGHT_OBSTACLE" },
     { { 1660, 285, 1736, 348 }, L"S3_91_RIGHT_OBSTACLE_2" },
     { { 1761, 285, 1900, 348 }, L"S3_91_RIGHT_WALL" },
     { { 1805, 123, 1890, 146 }, L"S3_91_TOP_LEDGE" },

     // 예전 커밋 9a72c7f 기준으로 3스테이지 위쪽 발판과 오른쪽 끝 벽을 복구함.
     { { 1890, 0, 1980, 405 }, L"S3_91_RIGHTEND_WALL" },
     { { 995, 0, 1085, 490 }, L"S3_91_FLOOR1" },
     { { 995, 0, 1115, 460 }, L"S3_91_FLOOR2" },
     { { 1100, 0, 1130, 340 }, L"S3_91_FLOOR3" },
     { { 1125, 0, 1265, 315 }, L"S3_91_FLOOR4" },
     { { 1255, 0, 1295, 245 }, L"S3_91_FLOOR5" },
     { { 1288, 0, 1530, 222 }, L"S3_91_FLOOR6" },
     { { 1520, 0, 1545, 195 }, L"S3_91_FLOOR7" },
     { { 1535, 0, 1730, 175 }, L"S3_91_FLOOR8" },
     { { 1725, 0, 1745, 150 }, L"S3_91_FLOOR9" },
     { { 1735, 0, 1773, 55 }, L"S3_91_FLOOR10" },
     { { 1770, 0, 1910, 35 }, L"S3_91_FLOOR11" }

};

int g_stage3SolidBlockCount = sizeof(g_stage3SolidBlocks) / sizeof(g_stage3SolidBlocks[0]);


void InitStage3RescueObjects()
{
    g_stage3Child.active = true;
    g_stage3Child.rescued = false;
    g_stage3Child.w = 47;
    g_stage3Child.h = 59;
    g_stage3Child.x = 1810;
    g_stage3Child.y = 360;

    // 학생 구출 + 몬스터 전멸 후 열리는 3스테이지 문
    g_stage3Door.active = true;
    g_stage3Door.opening = false;
    g_stage3Door.opened = false;
    g_stage3Door.w = 81;
    g_stage3Door.h = 101;
    g_stage3Door.x = 1810;
    g_stage3Door.y = 126 - g_stage3Door.h + 7;
    g_stage3Door.frameIndex = 0;
    g_stage3Door.tick = 0;

    g_stage3ChildTotal = 1;
    g_stage3ChildRescued = 0;
}
SolidBlock g_stage4SolidBlocks[] =
{
    { { 0, 545, 1000, 650 }, L"S4_BOSS_GROUND" },
    { { 0, 0, 12, 650 }, L"S4_LEFT_LIMIT" },
    { { 988, 0, 1000, 650 }, L"S4_RIGHT_LIMIT" }
};

int g_stage4SolidBlockCount = sizeof(g_stage4SolidBlocks) / sizeof(g_stage4SolidBlocks[0]);

SolidBlock g_stage5SolidBlocks[] =
{
    { { 0, DANCE_FLOOR_Y, 1000, 650 }, L"S5_DANCE_GROUND" },
    { { 0, 0, 12, 650 }, L"S5_LEFT_LIMIT" },
    { { 988, 0, 1000, 650 }, L"S5_RIGHT_LIMIT" }
};

int g_stage5SolidBlockCount = sizeof(g_stage5SolidBlocks) / sizeof(g_stage5SolidBlocks[0]);

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
    {
        InitStage2RescueObjects();
    }
    else if (g_currentStage == 3)
    {
        InitStage3RescueObjects();
    }
    else if (g_currentStage == 4)
    {
    }
    else if (g_currentStage == 5)
    {
        ResetDanceStage();
    }
    else
    {
        InitStage1RescueObjects();
    }

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
            g_totalStudentsRescued++;
            AddGameScore(1000);
            StartRescueEffect(g_stage1Boy.x + g_stage1Boy.w / 2, g_stage1Boy.y + g_stage1Boy.h / 2);
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
                g_totalStudentsRescued++;
                AddGameScore(1000);
                StartRescueEffect(g_stage2Children[i].x + g_stage2Children[i].w / 2, g_stage2Children[i].y + g_stage2Children[i].h / 2);
            }
        }
        return;
    }

    if (g_currentStage == 3)
    {
        if (!g_stage3Child.active || g_stage3Child.rescued)
            return;

        RECT childRc = GetChildRect(g_stage3Child);

        if (IsRectHit(kirbyRc, childRc))
        {
            g_stage3Child.active = false;
            g_stage3Child.rescued = true;
            g_stage3ChildRescued++;
            g_totalStudentsRescued++;
            AddGameScore(1000);
            StartRescueEffect(g_stage3Child.x + g_stage3Child.w / 2, g_stage3Child.y + g_stage3Child.h / 2);
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
        if (g_stage1ChildRescued >= g_stage1ChildTotal && !g_stage1Door.opened && !g_stage1Door.opening)
        {
            g_stage1Door.opening = true;
        }

        UpdateDoorOpen(&g_stage1Door);
        return;
    }

    if (g_currentStage == 2)
    {
        if (g_stage2ChildRescued >= g_stage2ChildTotal && !g_stage2Door.opened && !g_stage2Door.opening)
        {
            g_stage2Door.opening = true;
        }

        UpdateDoorOpen(&g_stage2Door);
        return;
    }

    if (g_currentStage == 3)
    {
        if (g_stage3ChildRescued >= g_stage3ChildTotal && !g_stage3Door.opened && !g_stage3Door.opening)
        {
            g_stage3Door.opening = true;
        }

        UpdateDoorOpen(&g_stage3Door);
    }
}

// 스테이지 시작 위치 설정 함수: 새 스테이지에서 커비 좌표, 세로 속도, 안전 위치, 카메라를 초기화한다.
void SetKirbyStageMovePosition(int x, int y)
{
    kirbyX = x;
    kirbyY = y;
    kirbyVY = 0.0f;
    g_lastSafeKirbyX = kirbyX;
    g_lastSafeKirbyY = kirbyY;
    cameraX = 0;
}

// 스테이지 전환 마무리 함수: 구출 대상, 몬스터, 카메라, 스테이지 장치, 전환 효과를 다시 준비한다.
void FinishStageChange(HWND hWnd)
{
    InitRescueObjects();
    InitMonsters();
    UpdateCamera(hWnd);
    ResetStageGimmicks();
    StartStageTransitionEffect();
}

// 스테이지 즉시 변경 함수: 목표 스테이지 번호에 맞춰 현재 스테이지와 커비 시작 위치를 바꾼다.
void ChangeStageNow(HWND hWnd, int targetStage)
{
    // 발표용 순서: 현재 스테이지 변경 -> 커비 시작 위치 지정 -> 스테이지 요소 초기화
    if (targetStage == 2)
    {
        g_currentStage = 2;
        SetKirbyStageMovePosition(70, 330);
        FinishStageChange(hWnd);
        return;
    }

    if (targetStage == 3)
    {
        g_currentStage = 3;
        SetKirbyStageMovePosition(145, 500);
        FinishStageChange(hWnd);
        return;
    }

    if (targetStage == 4)
    {
        g_currentStage = 4;
        SetKirbyStageMovePosition(80, 480);
        FinishStageChange(hWnd);
        return;
    }

    if (targetStage == 5)
    {
        g_currentStage = 5;
        g_stageClearTick = 0;
        SetKirbyStageMovePosition(DANCE_CENTER_X - NORMAL_KIRBY_W / 2, DANCE_FLOOR_Y - NORMAL_KIRBY_H);
        SetKirbyNormalSizeKeepBottom();
        FinishStageChange(hWnd);
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


// 일반 문 접촉 확인 함수: 문이 활성화되고 열린 상태일 때 커비와 문이 겹치는지 확인한다.
bool IsKirbyTouchOpenedDoor(RECT kirbyRc, StageDoor door)
{
    if (!door.active || !door.opened)
        return false;

    RECT doorRc = GetDoorRect(door);
    return IsRectHit(kirbyRc, doorRc);
}

// 보스 보상 문 접촉 확인 함수: 보스 클리어 후 열린 보상 문에 커비가 닿았는지 확인한다.
bool IsKirbyTouchBossRewardDoor(RECT kirbyRc)
{
    if (!g_rewardDoorActive || !g_rewardDoorOpened)
        return false;

    RECT doorRc = MakeRectFromXYWH(g_rewardDoorX, g_rewardDoorY, g_rewardDoorW, g_rewardDoorH);
    return IsRectHit(kirbyRc, doorRc);
}

// 문 이동 확인 함수: 현재 스테이지의 열린 문에 커비가 닿으면 다음 맵으로 이동시킨다.
void CheckDoorTouch(HWND hWnd)
{
    RECT kirbyRc = GetKirbyBodyRect();

    if (g_currentStage == 1 && IsKirbyTouchOpenedDoor(kirbyRc, g_stage1Door))
    {
        GoNextMap(hWnd);
        return;
    }

    if (g_currentStage == 2 && IsKirbyTouchOpenedDoor(kirbyRc, g_stage2Door))
    {
        GoNextMap(hWnd);
        return;
    }

    if (g_currentStage == 3 && IsKirbyTouchOpenedDoor(kirbyRc, g_stage3Door))
    {
        GoNextMap(hWnd);
        return;
    }

    // 보스 처치 후 열쇠로 문을 연 상태에서 문에 닿으면 춤 테스트 맵으로 이동
    if (g_currentStage == 4 && IsKirbyTouchBossRewardDoor(kirbyRc))
    {
        GoNextMap(hWnd);
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
        DrawChildObject(graphics, g_stage3Child, 83);
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
        g_monsters[4].Init(1125, 470, 1025, 1305, -1, 1);
        g_monsters[5].active = false;
        return;
    }

    if (g_currentStage == 2)
    {
        // 2스테이지: 일반몹 2마리, 불몹 2마리, 폭탄몹 2마리, 망치 1마리, 스파크 1마리
        g_monsters[0].Init(20, 320, 10, 230, 1, 0);
        g_monsters[1].Init(500, 235, 440, 675, -1, 0);
        g_monsters[2].Init(790, 448, 695, 890, -1, 3);
        g_monsters[3].Init(1450, 285, 1425, 1685, -1, 1);
        g_monsters[4].Init(1690, 360, 1425, 1685, 1, 1);
        g_monsters[5].Init(1780, 145, 1690, 1880, 1, 2);
        g_monsters[6].Init(1080, 316, 1000, 1230, -1, 4);
        g_monsters[7].Init(1840, 430, 1735, 1980, -1, 2);
        g_monsters[8].active = false;
        return;
    }

    if (g_currentStage == 3)
    {
        // 3스테이지: 모든 몬스터를 없애고 학생을 구출해야 문이 열림
        g_monsters[0].Init(50, 255, 0, 250, 1, 2);
        g_monsters[1].Init(820, 150, 790, 945, -1, 2);
        g_monsters[2].Init(1320, 250, 1270, 1495, 1, 2);
        g_monsters[3].Init(930, 503, 892, 1015, -1, 3);
        g_monsters[4].Init(1240, 374, 1182, 1520, 1, 4);
        g_monsters[5].Init(1820, 280, 1770, 1880, 1, 4);
        g_monsters[6].active = false;
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

// frameIndex는 0부터 시작함. 0은 "커비 댄스1.png", 22는 "커비 댄스23.png"를 뜻함.
// 기본 Y좌표는 고정하고, yOffset은 점프/통통 튀는 프레임에서만 사용함.
const DanceFrame g_danceSequence[] =
{
    // 클리어 춤이 멈춰 보이지 않도록 박자마다 X좌표를 좌우로 움직임.
    { 0, 6, false, 0, 0 },
    { 1, 6, false, 6, 0 },
    { 4, 6, false, 14, 0 },
    { 5, 6, false, 22, -2 },
    { 6, 6, false, 30, -4 },
    { 7, 6, false, 38, -2 },
    { 11, 7, false, 46, 0 },
    { 10, 6, false, 36, 0 },
    { 9, 5, false, 24, 0 },
    { 10, 6, false, 12, 0 },
    { 1, 6, false, 0, 0 },
    { 11, 6, true, -10, 0 },
    { 7, 6, true, -20, -2 },
    { 6, 6, true, -30, -4 },
    { 5, 6, true, -40, -2 },
    { 4, 7, true, -48, 0 },
    { 10, 6, true, -36, 0 },
    { 9, 5, true, -24, 0 },
    { 10, 6, true, -12, 0 },
    { 1, 6, false, 0, 0 }
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

    int totalTicks = GetDanceSequenceTotalTicks();
    if (totalTicks <= 0)
        return;

    if (g_danceFinished) // 엔딩 춤은 한 바퀴를 다 돈 뒤 마지막 자세에서 멈춤.
    {
        g_danceTick = totalTicks - 1;
        ApplyDanceFrameState();
        return;
    }

    g_danceTick++;

    if (g_danceTick >= totalTicks) // 클리어 춤을 반복하지 않고 마지막 프레임을 유지함.
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

    Image* frame = g_idleFrame;

    if (g_danceFrames[frameIndex] != NULL)
        frame = g_danceFrames[frameIndex];

    int drawW = DANCE_DRAW_W;
    int drawH = DANCE_DRAW_H;

    // PNG 프레임 크기가 달라도 커비 크기가 흔들리지 않도록 고정 박스에 그림.
    int drawX = g_danceX - drawW / 2;
    int drawY = DANCE_FLOOR_Y - drawH + danceFrame->yOffset;

    if (danceFrame->flipX)
        DrawImageFlipX(graphics, frame, drawX, drawY, drawW, drawH);
    else
        DrawWorldImage(graphics, frame, drawX, drawY, drawW, drawH);

    if (g_danceFinished && g_clearTrophyFrame != NULL)
    {
        const int trophyW = 42;
        const int trophyH = 34;
        int trophyX = g_danceX - trophyW/2;
        int trophyY = drawY - 20;
        DrawWorldImage(graphics, g_clearTrophyFrame, trophyX, trophyY, trophyW, trophyH);
    }
}
