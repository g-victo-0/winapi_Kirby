// PNG and image resource loading/cleanup
// Included by game.cpp to keep the existing global-state gameplay unchanged.

Image* LoadPNGFromResource(HINSTANCE hInst, int resourceID)
{
    HRSRC hResource = FindResource(hInst, MAKEINTRESOURCE(resourceID), L"PNG");

    if (hResource == NULL)
        return NULL;

    DWORD imageSize = SizeofResource(hInst, hResource);

    if (imageSize == 0)
        return NULL;

    HGLOBAL hGlobal = LoadResource(hInst, hResource);

    if (hGlobal == NULL)
        return NULL;

    void* pResourceData = LockResource(hGlobal);

    if (pResourceData == NULL)
        return NULL;

    HGLOBAL hBuffer = GlobalAlloc(GMEM_MOVEABLE, imageSize);

    if (hBuffer == NULL)
        return NULL;

    void* pBuffer = GlobalLock(hBuffer);

    if (pBuffer == NULL)
    {
        GlobalFree(hBuffer);
        return NULL;
    }

    CopyMemory(pBuffer, pResourceData, imageSize);
    GlobalUnlock(hBuffer);

    IStream* pStream = NULL;

    if (CreateStreamOnHGlobal(hBuffer, TRUE, &pStream) != S_OK)
    {
        GlobalFree(hBuffer);
        return NULL;
    }

    Image* image = Image::FromStream(pStream);

    pStream->Release();

    if (image == NULL || image->GetLastStatus() != Ok)
    {
        delete image;
        return NULL;
    }

    return image;
}

void LoadAllImages(HWND hWnd)
{
    g_openingFrame = LoadPNGFromResource(g_hInst, IDB_PNG72);

    g_storyFrames[0] = LoadPNGFromResource(g_hInst, IDB_PNG73);
    g_storyFrames[1] = LoadPNGFromResource(g_hInst, IDB_PNG74);
    g_storyFrames[2] = LoadPNGFromResource(g_hInst, IDB_PNG75);
    g_storyFrames[3] = LoadPNGFromResource(g_hInst, IDB_PNG76);
    g_storyFrames[4] = LoadPNGFromResource(g_hInst, IDB_PNG77);
    g_storyFrames[5] = LoadPNGFromResource(g_hInst, IDB_PNG78);
    g_storyFrames[6] = LoadPNGFromResource(g_hInst, IDB_PNG79);

    g_studentBoyFrame = LoadPNGFromResource(g_hInst, IDB_PNG81);
    g_studentGirlFrame = LoadPNGFromResource(g_hInst, IDB_PNG83);
    g_doorFrames[0] = LoadPNGFromResource(g_hInst, IDB_PNG84);
    g_doorFrames[1] = LoadPNGFromResource(g_hInst, IDB_PNG85);
    g_doorFrames[2] = LoadPNGFromResource(g_hInst, IDB_PNG86);
    g_doorFrames[3] = LoadPNGFromResource(g_hInst, IDB_PNG87);

    g_idleFrame = LoadPNGFromResource(g_hInst, IDB_PNG1);

    g_walkFrames[0] = LoadPNGFromResource(g_hInst, IDB_PNG2);
    g_walkFrames[1] = LoadPNGFromResource(g_hInst, IDB_PNG3);
    g_walkFrames[2] = LoadPNGFromResource(g_hInst, IDB_PNG4);
    g_walkFrames[3] = LoadPNGFromResource(g_hInst, IDB_PNG5);

    g_spaceFrames[0] = LoadPNGFromResource(g_hInst, IDB_PNG6);
    g_spaceFrames[1] = LoadPNGFromResource(g_hInst, IDB_PNG7);
    g_spaceFrames[2] = LoadPNGFromResource(g_hInst, IDB_PNG8);

    g_absorbFrames[0] = LoadPNGFromResource(g_hInst, IDB_PNG9);
    g_absorbFrames[1] = LoadPNGFromResource(g_hInst, IDB_PNG10);
    g_absorbFrames[2] = LoadPNGFromResource(g_hInst, IDB_PNG11);

    g_crouchFrame = LoadPNGFromResource(g_hInst, IDB_PNG12);

    g_spaceReleaseEffect = LoadPNGFromResource(g_hInst, IDB_PNG13);

    g_absorbFrontEffectFrames[0] = LoadPNGFromResource(g_hInst, IDB_PNG14);
    g_absorbFrontEffectFrames[1] = LoadPNGFromResource(g_hInst, IDB_PNG15);

    g_monsterJumpFrames[0] = LoadPNGFromResource(g_hInst, IDB_PNG16);
    g_monsterJumpFrames[1] = LoadPNGFromResource(g_hInst, IDB_PNG17);

    g_monsterFrames[0] = LoadPNGFromResource(g_hInst, IDB_PNG18);
    g_monsterFrames[1] = LoadPNGFromResource(g_hInst, IDB_PNG19);
    g_monsterFrames[2] = LoadPNGFromResource(g_hInst, IDB_PNG20);
    g_monsterFrames[3] = LoadPNGFromResource(g_hInst, IDB_PNG21);

    g_monsterDeadFrame = LoadPNGFromResource(g_hInst, IDB_PNG35);
    g_fireMonsterDeadFrame = LoadPNGFromResource(g_hInst, IDB_PNG71);

    g_background = LoadPNGFromResource(g_hInst, IDB_PNG22);
    g_background2 = LoadPNGFromResource(g_hInst, IDB_PNG23);
    g_stage2BackgroundFront = LoadPNGFromResource(g_hInst, IDB_PNG88);
    g_stage2BackgroundBack = LoadPNGFromResource(g_hInst, IDB_PNG89);

    // 3스테이지 배경.
    // 여기 빠져 있으면 90/91 리소스를 넣어도 게임에서는 NULL이 돼서 파란 fallback 배경만 나옴.
    g_stage3BackgroundFront = LoadPNGFromResource(g_hInst, IDB_PNG90);
    g_stage3BackgroundBack = LoadPNGFromResource(g_hInst, IDB_PNG91);
    g_stage3RockFrame = LoadPNGFromResource(g_hInst, IDB_PNG139);

    // 4스테이지 보스전 배경: 92번 프레임
    g_stage4Background = LoadPNGFromResource(g_hInst, IDB_PNG92);

    // 138번: 마지막 클리어 스테이지 배경
    g_stage5ClearBackground = LoadPNGFromResource(g_hInst, IDB_PNG138);

    // 4스테이지 보스전 프레임 93~101
    g_bossMissilePoseFrame = LoadPNGFromResource(g_hInst, IDB_PNG93);
    g_bossDashFrame = LoadPNGFromResource(g_hInst, IDB_PNG94);
    g_bossIdleMoveFrame = LoadPNGFromResource(g_hInst, IDB_PNG95);
    g_bossMissileFrame = LoadPNGFromResource(g_hInst, IDB_PNG96);
    g_bossMouthBombFrame = LoadPNGFromResource(g_hInst, IDB_PNG97);
    g_bossTopAttackFrame = LoadPNGFromResource(g_hInst, IDB_PNG98);
    g_bossPhase2Frame = LoadPNGFromResource(g_hInst, IDB_PNG99);
    g_bossRainAttackFrame = LoadPNGFromResource(g_hInst, IDB_PNG100);
    g_bossRainBombFrame = LoadPNGFromResource(g_hInst, IDB_PNG101);
    g_bossDeathFrame1 = LoadPNGFromResource(g_hInst, IDB_PNG102);
    g_bossDeathFrame2 = LoadPNGFromResource(g_hInst, IDB_PNG103);
    g_bossPatternRedBallFrame = LoadPNGFromResource(g_hInst, IDB_PNG104);
    g_bossPatternBlueBallFrame = LoadPNGFromResource(g_hInst, IDB_PNG105);
    g_bossHalfFloorWarnFrame = LoadPNGFromResource(g_hInst, IDB_PNG106);
    g_bossHalfFloorBoomFrame = LoadPNGFromResource(g_hInst, IDB_PNG107);
    g_bossDoorFrames[0] = LoadPNGFromResource(g_hInst, IDB_PNG108);
    g_bossDoorFrames[1] = LoadPNGFromResource(g_hInst, IDB_PNG109);
    g_bossDoorFrames[2] = LoadPNGFromResource(g_hInst, IDB_PNG110);
    g_bossDoorFrames[3] = LoadPNGFromResource(g_hInst, IDB_PNG111);
    g_bossKeyFrame = LoadPNGFromResource(g_hInst, IDB_PNG112);
    g_bossChestClosedFrame = LoadPNGFromResource(g_hInst, IDB_PNG113);
    g_bossChestOpenFrame = LoadPNGFromResource(g_hInst, IDB_PNG114);
    g_bossBerserkAbsorbFrame1 = LoadPNGFromResource(g_hInst, IDB_PNG140);
    g_bossBerserkAbsorbFrame2 = LoadPNGFromResource(g_hInst, IDB_PNG141);
    g_bossBerserkEnergyBallFrame = LoadPNGFromResource(g_hInst, IDB_PNG142);
    g_recoveryItemFrames[0] = LoadPNGFromResource(g_hInst, IDB_PNG144);
    g_recoveryItemFrames[1] = LoadPNGFromResource(g_hInst, IDB_PNG145);
    g_recoveryItemFrames[2] = LoadPNGFromResource(g_hInst, IDB_PNG146);
    g_recoveryItemFrames[3] = LoadPNGFromResource(g_hInst, IDB_PNG147);

    g_danceFrames[0] = LoadPNGFromResource(g_hInst, IDB_PNG115);
    g_danceFrames[1] = LoadPNGFromResource(g_hInst, IDB_PNG116);
    g_danceFrames[2] = LoadPNGFromResource(g_hInst, IDB_PNG117);
    g_danceFrames[3] = LoadPNGFromResource(g_hInst, IDB_PNG118);
    g_danceFrames[4] = LoadPNGFromResource(g_hInst, IDB_PNG119);
    g_danceFrames[5] = LoadPNGFromResource(g_hInst, IDB_PNG120);
    g_danceFrames[6] = LoadPNGFromResource(g_hInst, IDB_PNG121);
    g_danceFrames[7] = LoadPNGFromResource(g_hInst, IDB_PNG122);
    g_danceFrames[8] = LoadPNGFromResource(g_hInst, IDB_PNG123);
    g_danceFrames[9] = LoadPNGFromResource(g_hInst, IDB_PNG124);
    g_danceFrames[10] = LoadPNGFromResource(g_hInst, IDB_PNG125);
    g_danceFrames[11] = LoadPNGFromResource(g_hInst, IDB_PNG126);
    g_danceFrames[12] = LoadPNGFromResource(g_hInst, IDB_PNG127);
    g_danceFrames[13] = LoadPNGFromResource(g_hInst, IDB_PNG128);
    g_danceFrames[14] = LoadPNGFromResource(g_hInst, IDB_PNG129);
    g_danceFrames[15] = LoadPNGFromResource(g_hInst, IDB_PNG130);
    g_danceFrames[16] = LoadPNGFromResource(g_hInst, IDB_PNG131);
    g_danceFrames[17] = LoadPNGFromResource(g_hInst, IDB_PNG132);
    g_danceFrames[18] = LoadPNGFromResource(g_hInst, IDB_PNG133);
    g_danceFrames[19] = LoadPNGFromResource(g_hInst, IDB_PNG134);
    g_danceFrames[20] = LoadPNGFromResource(g_hInst, IDB_PNG135);
    g_danceFrames[21] = LoadPNGFromResource(g_hInst, IDB_PNG136);
    g_danceFrames[22] = LoadPNGFromResource(g_hInst, IDB_PNG137);

    g_backgroundScaled = CreateScaledBitmap(g_background, BG_PART_W, BG_PART_H);
    g_background2Scaled = CreateScaledBitmap(g_background2, BG_PART_W, BG_PART_H);
    g_stage2BackgroundFrontScaled = CreateScaledBitmap(g_stage2BackgroundFront, BG_PART_W, BG_PART_H);
    g_stage2BackgroundBackScaled = CreateScaledBitmap(g_stage2BackgroundBack, BG_PART_W, BG_PART_H);
    g_stage3BackgroundFrontScaled = CreateScaledBitmap(g_stage3BackgroundFront, BG_PART_W, BG_PART_H);
    g_stage3BackgroundBackScaled = CreateScaledBitmap(g_stage3BackgroundBack, BG_PART_W, BG_PART_H);
    g_stage4BackgroundScaled = CreateScaledBitmap(g_stage4Background, BG_PART_W, BG_PART_H);
    g_stage5ClearBackgroundScaled = CreateScaledBitmap(g_stage5ClearBackground, BG_PART_W, BG_PART_H);

    g_powerIdleFrame = LoadPNGFromResource(g_hInst, IDB_PNG24);

    g_powerWalkFrames[0] = LoadPNGFromResource(g_hInst, IDB_PNG25);
    g_powerWalkFrames[1] = LoadPNGFromResource(g_hInst, IDB_PNG26);
    g_powerWalkFrames[2] = LoadPNGFromResource(g_hInst, IDB_PNG30);
    g_powerWalkFrames[3] = LoadPNGFromResource(g_hInst, IDB_PNG31);
    g_powerWalkFrames[4] = LoadPNGFromResource(g_hInst, IDB_PNG32);

    g_powerAttackFrame = LoadPNGFromResource(g_hInst, IDB_PNG33);
    g_powerProjectileFrame = LoadPNGFromResource(g_hInst, IDB_PNG34);
    g_powerDigestFrame = LoadPNGFromResource(g_hInst, IDB_PNG36);
    g_kirbyHitFrame = LoadPNGFromResource(g_hInst, IDB_PNG37);
    g_bombKirbyHitFrame = LoadPNGFromResource(g_hInst, IDB_PNG69);
    g_fireKirbyHitFrame = LoadPNGFromResource(g_hInst, IDB_PNG70);
    g_hpBarFrame = LoadPNGFromResource(g_hInst, IDB_PNG38);

    g_fireTransformFrame = LoadPNGFromResource(g_hInst, IDB_PNG39);
    g_fireIdleFrame = LoadPNGFromResource(g_hInst, IDB_PNG40);
    g_fireWalkFrames[0] = LoadPNGFromResource(g_hInst, IDB_PNG41);
    g_fireWalkFrames[1] = LoadPNGFromResource(g_hInst, IDB_PNG42);
    g_fireWalkFrames[2] = LoadPNGFromResource(g_hInst, IDB_PNG43);
    g_fireWalkFrames[3] = LoadPNGFromResource(g_hInst, IDB_PNG44);
    g_fireAttackKirbyFrame = LoadPNGFromResource(g_hInst, IDB_PNG45);
    g_fireBreathFrame = LoadPNGFromResource(g_hInst, IDB_PNG46);
    g_fireBallFrame = LoadPNGFromResource(g_hInst, IDB_PNG47);
    g_fireMonsterFrame = LoadPNGFromResource(g_hInst, IDB_PNG48);
    g_fireMonsterAttackFrame = LoadPNGFromResource(g_hInst, IDB_PNG49);

    // 불 커비 풍선 프레임
    // SPACE 시작/해제: 50번
    // SPACE 유지: 51번, 52번 반복
    g_fireBalloonStartFrame = LoadPNGFromResource(g_hInst, IDB_PNG50);
    g_fireBalloonFrames[0] = LoadPNGFromResource(g_hInst, IDB_PNG51);
    g_fireBalloonFrames[1] = LoadPNGFromResource(g_hInst, IDB_PNG52);
    g_fireCrouchFrame = LoadPNGFromResource(g_hInst, IDB_PNG53);

    g_bombIdleFrame = LoadPNGFromResource(g_hInst, IDB_PNG54);
    g_bombCrouchFrame = LoadPNGFromResource(g_hInst, IDB_PNG143);
    g_bombWalkFrames[0] = LoadPNGFromResource(g_hInst, IDB_PNG55);
    g_bombWalkFrames[1] = LoadPNGFromResource(g_hInst, IDB_PNG56);
    g_bombWalkFrames[2] = LoadPNGFromResource(g_hInst, IDB_PNG57);
    g_bombWalkFrames[3] = LoadPNGFromResource(g_hInst, IDB_PNG58);
    g_bombBalloonStartFrame = LoadPNGFromResource(g_hInst, IDB_PNG59);
    g_bombBalloonFrames[0] = LoadPNGFromResource(g_hInst, IDB_PNG60);
    g_bombBalloonFrames[1] = LoadPNGFromResource(g_hInst, IDB_PNG61);
    g_bombAttackFrames[0] = LoadPNGFromResource(g_hInst, IDB_PNG62);
    g_bombAttackFrames[1] = LoadPNGFromResource(g_hInst, IDB_PNG63);
    g_bombAttackFrames[2] = LoadPNGFromResource(g_hInst, IDB_PNG64);
    g_bombProjectileFrame = LoadPNGFromResource(g_hInst, IDB_PNG65);
    // 2스테이지에서 폭탄병을 사용하므로 PNG66, PNG67도 로드
    g_bombMonsterFrame = LoadPNGFromResource(g_hInst, IDB_PNG66);
    g_bombMonsterDeadFrame = LoadPNGFromResource(g_hInst, IDB_PNG67);
    g_bombTransformFrame = LoadPNGFromResource(g_hInst, IDB_PNG68);

    g_dashWindFrames[0] = LoadPNGFromResource(g_hInst, IDB_PNG27);
    g_dashWindFrames[1] = LoadPNGFromResource(g_hInst, IDB_PNG28);
    g_dashWindFrames[2] = LoadPNGFromResource(g_hInst, IDB_PNG29);

    // 로드 실패 MessageBox 검사들은 실행 중 불필요한 팝업이라 제거함.

}

void DeleteAllImages()
{
    if (g_openingFrame != NULL)
    {
        delete g_openingFrame;
        g_openingFrame = NULL;
    }

    for (int i = 0; i < STORY_FRAME_COUNT; i++)
    {
        if (g_storyFrames[i] != NULL)
        {
            delete g_storyFrames[i];
            g_storyFrames[i] = NULL;
        }
    }

    if (g_studentBoyFrame != NULL)
    {
        delete g_studentBoyFrame;
        g_studentBoyFrame = NULL;
    }

    if (g_studentGirlFrame != NULL)
    {
        delete g_studentGirlFrame;
        g_studentGirlFrame = NULL;
    }

    for (int i = 0; i < DOOR_FRAME_COUNT; i++)
    {
        if (g_doorFrames[i] != NULL)
        {
            delete g_doorFrames[i];
            g_doorFrames[i] = NULL;
        }
    }

    if (g_idleFrame != NULL)
    {
        delete g_idleFrame;
        g_idleFrame = NULL;
    }

    for (int i = 0; i < walkFrameCount; i++)
    {
        if (g_walkFrames[i] != NULL)
        {
            delete g_walkFrames[i];
            g_walkFrames[i] = NULL;
        }
    }

    for (int i = 0; i < spaceFrameCount; i++)
    {
        if (g_spaceFrames[i] != NULL)
        {
            delete g_spaceFrames[i];
            g_spaceFrames[i] = NULL;
        }
    }

    for (int i = 0; i < absorbFrameCount; i++)
    {
        if (g_absorbFrames[i] != NULL)
        {
            delete g_absorbFrames[i];
            g_absorbFrames[i] = NULL;
        }
    }

    if (g_crouchFrame != NULL)
    {
        delete g_crouchFrame;
        g_crouchFrame = NULL;
    }

    if (g_spaceReleaseEffect != NULL)
    {
        delete g_spaceReleaseEffect;
        g_spaceReleaseEffect = NULL;
    }

    for (int i = 0; i < 2; i++)
    {
        if (g_absorbFrontEffectFrames[i] != NULL)
        {
            delete g_absorbFrontEffectFrames[i];
            g_absorbFrontEffectFrames[i] = NULL;
        }
    }

    for (int i = 0; i < monsterJumpFrameCount; i++)
    {
        if (g_monsterJumpFrames[i] != NULL)
        {
            delete g_monsterJumpFrames[i];
            g_monsterJumpFrames[i] = NULL;
        }
    }

    for (int i = 0; i < monsterFrameCount; i++)
    {
        if (g_monsterFrames[i] != NULL)
        {
            delete g_monsterFrames[i];
            g_monsterFrames[i] = NULL;
        }
    }

    if (g_monsterDeadFrame != NULL)
    {
        delete g_monsterDeadFrame;
        g_monsterDeadFrame = NULL;
    }

    if (g_fireMonsterDeadFrame != NULL)
    {
        delete g_fireMonsterDeadFrame;
        g_fireMonsterDeadFrame = NULL;
    }

    if (g_backgroundScaled != NULL)
    {
        delete g_backgroundScaled;
        g_backgroundScaled = NULL;
    }

    if (g_background2Scaled != NULL)
    {
        delete g_background2Scaled;
        g_background2Scaled = NULL;
    }

    if (g_stage2BackgroundFrontScaled != NULL)
    {
        delete g_stage2BackgroundFrontScaled;
        g_stage2BackgroundFrontScaled = NULL;
    }

    if (g_stage2BackgroundBackScaled != NULL)
    {
        delete g_stage2BackgroundBackScaled;
        g_stage2BackgroundBackScaled = NULL;
    }

    if (g_stage3BackgroundFrontScaled != NULL)
    {
        delete g_stage3BackgroundFrontScaled;
        g_stage3BackgroundFrontScaled = NULL;
    }

    if (g_stage3BackgroundBackScaled != NULL)
    {
        delete g_stage3BackgroundBackScaled;
        g_stage3BackgroundBackScaled = NULL;
    }

    if (g_stage4BackgroundScaled != NULL)
    {
        delete g_stage4BackgroundScaled;
        g_stage4BackgroundScaled = NULL;
    }

    if (g_stage5ClearBackgroundScaled != NULL)
    {
        delete g_stage5ClearBackgroundScaled;
        g_stage5ClearBackgroundScaled = NULL;
    }

    if (g_background != NULL)
    {
        delete g_background;
        g_background = NULL;
    }

    if (g_background2 != NULL)
    {
        delete g_background2;
        g_background2 = NULL;
    }

    if (g_stage2BackgroundFront != NULL)
    {
        delete g_stage2BackgroundFront;
        g_stage2BackgroundFront = NULL;
    }

    if (g_stage2BackgroundBack != NULL)
    {
        delete g_stage2BackgroundBack;
        g_stage2BackgroundBack = NULL;
    }

    if (g_stage3BackgroundFront != NULL)
    {
        delete g_stage3BackgroundFront;
        g_stage3BackgroundFront = NULL;
    }

    if (g_stage3BackgroundBack != NULL)
    {
        delete g_stage3BackgroundBack;
        g_stage3BackgroundBack = NULL;
    }

    if (g_stage3RockFrame != NULL)
    {
        delete g_stage3RockFrame;
        g_stage3RockFrame = NULL;
    }

    if (g_stage4Background != NULL)
    {
        delete g_stage4Background;
        g_stage4Background = NULL;
    }

    if (g_stage5ClearBackground != NULL)
    {
        delete g_stage5ClearBackground;
        g_stage5ClearBackground = NULL;
    }

    if (g_bossMissilePoseFrame != NULL)
    {
        delete g_bossMissilePoseFrame;
        g_bossMissilePoseFrame = NULL;
    }

    if (g_bossDashFrame != NULL)
    {
        delete g_bossDashFrame;
        g_bossDashFrame = NULL;
    }

    if (g_bossIdleMoveFrame != NULL)
    {
        delete g_bossIdleMoveFrame;
        g_bossIdleMoveFrame = NULL;
    }

    if (g_bossMissileFrame != NULL)
    {
        delete g_bossMissileFrame;
        g_bossMissileFrame = NULL;
    }

    if (g_bossMouthBombFrame != NULL)
    {
        delete g_bossMouthBombFrame;
        g_bossMouthBombFrame = NULL;
    }

    if (g_bossTopAttackFrame != NULL)
    {
        delete g_bossTopAttackFrame;
        g_bossTopAttackFrame = NULL;
    }

    if (g_bossPhase2Frame != NULL)
    {
        delete g_bossPhase2Frame;
        g_bossPhase2Frame = NULL;
    }

    if (g_bossRainAttackFrame != NULL)
    {
        delete g_bossRainAttackFrame;
        g_bossRainAttackFrame = NULL;
    }

    if (g_bossRainBombFrame != NULL)
    {
        delete g_bossRainBombFrame;
        g_bossRainBombFrame = NULL;
    }

    if (g_bossDeathFrame1 != NULL)
    {
        delete g_bossDeathFrame1;
        g_bossDeathFrame1 = NULL;
    }

    if (g_bossDeathFrame2 != NULL)
    {
        delete g_bossDeathFrame2;
        g_bossDeathFrame2 = NULL;
    }

    if (g_bossPatternRedBallFrame != NULL)
    {
        delete g_bossPatternRedBallFrame;
        g_bossPatternRedBallFrame = NULL;
    }

    if (g_bossPatternBlueBallFrame != NULL)
    {
        delete g_bossPatternBlueBallFrame;
        g_bossPatternBlueBallFrame = NULL;
    }

    if (g_bossHalfFloorWarnFrame != NULL)
    {
        delete g_bossHalfFloorWarnFrame;
        g_bossHalfFloorWarnFrame = NULL;
    }

    if (g_bossHalfFloorBoomFrame != NULL)
    {
        delete g_bossHalfFloorBoomFrame;
        g_bossHalfFloorBoomFrame = NULL;
    }

    for (int i = 0; i < 4; i++)
    {
        if (g_bossDoorFrames[i] != NULL)
        {
            delete g_bossDoorFrames[i];
            g_bossDoorFrames[i] = NULL;
        }
    }

    if (g_bossKeyFrame != NULL)
    {
        delete g_bossKeyFrame;
        g_bossKeyFrame = NULL;
    }

    if (g_bossChestClosedFrame != NULL)
    {
        delete g_bossChestClosedFrame;
        g_bossChestClosedFrame = NULL;
    }

    if (g_bossChestOpenFrame != NULL)
    {
        delete g_bossChestOpenFrame;
        g_bossChestOpenFrame = NULL;
    }

    if (g_bossBerserkAbsorbFrame1 != NULL)
    {
        delete g_bossBerserkAbsorbFrame1;
        g_bossBerserkAbsorbFrame1 = NULL;
    }

    if (g_bossBerserkAbsorbFrame2 != NULL)
    {
        delete g_bossBerserkAbsorbFrame2;
        g_bossBerserkAbsorbFrame2 = NULL;
    }

    if (g_bossBerserkEnergyBallFrame != NULL)
    {
        delete g_bossBerserkEnergyBallFrame;
        g_bossBerserkEnergyBallFrame = NULL;
    }

    for (int i = 0; i < 4; i++)
    {
        if (g_recoveryItemFrames[i] != NULL)
        {
            delete g_recoveryItemFrames[i];
            g_recoveryItemFrames[i] = NULL;
        }
    }

    for (int i = 0; i < DANCE_FRAME_COUNT; i++)
    {
        if (g_danceFrames[i] != NULL)
        {
            delete g_danceFrames[i];
            g_danceFrames[i] = NULL;
        }
    }

    if (g_powerIdleFrame != NULL)
    {
        delete g_powerIdleFrame;
        g_powerIdleFrame = NULL;
    }

    for (int i = 0; i < powerWalkFrameCount; i++)
    {
        if (g_powerWalkFrames[i] != NULL)
        {
            delete g_powerWalkFrames[i];
            g_powerWalkFrames[i] = NULL;
        }
    }

    if (g_powerAttackFrame != NULL)
    {
        delete g_powerAttackFrame;
        g_powerAttackFrame = NULL;
    }

    if (g_powerProjectileFrame != NULL)
    {
        delete g_powerProjectileFrame;
        g_powerProjectileFrame = NULL;
    }

    if (g_powerDigestFrame != NULL)
    {
        delete g_powerDigestFrame;
        g_powerDigestFrame = NULL;
    }

    if (g_kirbyHitFrame != NULL)
    {
        delete g_kirbyHitFrame;
        g_kirbyHitFrame = NULL;
    }

    if (g_bombKirbyHitFrame != NULL)
    {
        delete g_bombKirbyHitFrame;
        g_bombKirbyHitFrame = NULL;
    }

    if (g_fireKirbyHitFrame != NULL)
    {
        delete g_fireKirbyHitFrame;
        g_fireKirbyHitFrame = NULL;
    }

    if (g_hpBarFrame != NULL)
    {
        delete g_hpBarFrame;
        g_hpBarFrame = NULL;
    }

    if (g_fireTransformFrame != NULL)
    {
        delete g_fireTransformFrame;
        g_fireTransformFrame = NULL;
    }

    if (g_fireIdleFrame != NULL)
    {
        delete g_fireIdleFrame;
        g_fireIdleFrame = NULL;
    }

    for (int i = 0; i < FIRE_WALK_FRAME_COUNT; i++)
    {
        if (g_fireWalkFrames[i] != NULL)
        {
            delete g_fireWalkFrames[i];
            g_fireWalkFrames[i] = NULL;
        }
    }

    if (g_fireAttackKirbyFrame != NULL)
    {
        delete g_fireAttackKirbyFrame;
        g_fireAttackKirbyFrame = NULL;
    }

    if (g_fireBreathFrame != NULL)
    {
        delete g_fireBreathFrame;
        g_fireBreathFrame = NULL;
    }

    if (g_fireBallFrame != NULL)
    {
        delete g_fireBallFrame;
        g_fireBallFrame = NULL;
    }

    if (g_fireMonsterFrame != NULL)
    {
        delete g_fireMonsterFrame;
        g_fireMonsterFrame = NULL;
    }

    if (g_fireMonsterAttackFrame != NULL)
    {
        delete g_fireMonsterAttackFrame;
        g_fireMonsterAttackFrame = NULL;
    }

    if (g_fireBalloonStartFrame != NULL)
    {
        delete g_fireBalloonStartFrame;
        g_fireBalloonStartFrame = NULL;
    }

    for (int i = 0; i < 2; i++)
    {
        if (g_fireBalloonFrames[i] != NULL)
        {
            delete g_fireBalloonFrames[i];
            g_fireBalloonFrames[i] = NULL;
        }
    }

    if (g_fireCrouchFrame != NULL)
    {
        delete g_fireCrouchFrame;
        g_fireCrouchFrame = NULL;
    }

    if (g_bombIdleFrame != NULL)
    {
        delete g_bombIdleFrame;
        g_bombIdleFrame = NULL;
    }

    if (g_bombCrouchFrame != NULL)
    {
        delete g_bombCrouchFrame;
        g_bombCrouchFrame = NULL;
    }

    for (int i = 0; i < BOMB_WALK_FRAME_COUNT; i++)
    {
        if (g_bombWalkFrames[i] != NULL)
        {
            delete g_bombWalkFrames[i];
            g_bombWalkFrames[i] = NULL;
        }
    }

    if (g_bombBalloonStartFrame != NULL)
    {
        delete g_bombBalloonStartFrame;
        g_bombBalloonStartFrame = NULL;
    }

    for (int i = 0; i < 2; i++)
    {
        if (g_bombBalloonFrames[i] != NULL)
        {
            delete g_bombBalloonFrames[i];
            g_bombBalloonFrames[i] = NULL;
        }
    }

    for (int i = 0; i < 3; i++)
    {
        if (g_bombAttackFrames[i] != NULL)
        {
            delete g_bombAttackFrames[i];
            g_bombAttackFrames[i] = NULL;
        }
    }

    if (g_bombProjectileFrame != NULL)
    {
        delete g_bombProjectileFrame;
        g_bombProjectileFrame = NULL;
    }

    if (g_bombMonsterFrame != NULL)
    {
        delete g_bombMonsterFrame;
        g_bombMonsterFrame = NULL;
    }

    if (g_bombMonsterDeadFrame != NULL)
    {
        delete g_bombMonsterDeadFrame;
        g_bombMonsterDeadFrame = NULL;
    }

    if (g_bombTransformFrame != NULL)
    {
        delete g_bombTransformFrame;
        g_bombTransformFrame = NULL;
    }


    for (int i = 0; i < dashFrameCount; i++)
    {
        if (g_dashWindFrames[i] != NULL)
        {
            delete g_dashWindFrames[i];
            g_dashWindFrames[i] = NULL;
        }
    }
}
