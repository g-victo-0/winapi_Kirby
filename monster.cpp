// Monster storage, drawing, and contact damage
// Included by game.cpp to keep the existing global-state gameplay unchanged.

const int MONSTER_COUNT = 5; // 2스테이지에서는 폭탄병까지 사용
Monster g_monsters[MONSTER_COUNT];
void Monster::Draw(Graphics& graphics)
{
    if (!IsVisibleWorld(x, y, w, h))
        return;

    if (isDeadEffect)
    {
        Image* deadFrame = g_monsterDeadFrame;

        // 불속성 몬스터는 71번 죽는 프레임 사용
        if (monsterType == 1 && g_fireMonsterDeadFrame != NULL)
        {
            deadFrame = g_fireMonsterDeadFrame;
        }
        // 폭탄 몬스터는 67번 죽는 프레임 사용
        else if (monsterType == 2 && g_bombMonsterDeadFrame != NULL)
        {
            deadFrame = g_bombMonsterDeadFrame;
        }

        if (deadFrame == NULL)
            return;

        if (dir == -1)
        {
            DrawImageFlipX(graphics, deadFrame, x, y, w, h);
        }
        else
        {
            DrawWorldImage(graphics, deadFrame, x, y, w, h);
        }

        return;
    }

    if (!active)
        return;

    if (isJumpAttack)
    {
        Image* jumpFrame = g_monsterJumpFrames[jumpAttackFrameIndex];

        if (jumpFrame == NULL)
            return;

        if (dir == -1)
        {
            DrawImageFlipX(graphics, jumpFrame, x, y, w, h);
        }
        else
        {
            DrawWorldImage(graphics, jumpFrame, x, y, w, h);
        }

        return;
    }

    Image* frame = NULL;
     
    if (monsterType == 1)
    {
        frame = g_fireMonsterFrame; // 불속성 몬스터 몸은 항상 PNG48
    }
    else if (monsterType == 2)
    {
        frame = g_bombMonsterFrame; // 하늘 폭탄 몬스터 몸은 PNG66
    }
    else
    {
        frame = g_monsterFrames[frameIndex];
    }

    if (frame == NULL)
        return;

    if (dir == -1)
    {
        DrawImageFlipX(graphics, frame, x, y, w, h);
    }
    else
    {
        DrawWorldImage(graphics, frame, x, y, w, h);
    }
}

void CheckKirbyHitByMonsters()
{
    // 빨아들이는 중에는 몬스터를 끌어와서 먹는 판정이 있으니 몸통 데미지는 끔
    if (isAbsorb)
        return;

    if (isKirbyHit)
        return;

    if (kirbyHitCooldownTick > 0)
        return;

    RECT kirbyRc = GetKirbyBodyRect();

    for (int i = 0; i < MONSTER_COUNT; i++)
    {
        if (!g_monsters[i].active)
            continue;

        RECT monsterRc;
        monsterRc.left = g_monsters[i].x;
        monsterRc.top = g_monsters[i].y;
        monsterRc.right = g_monsters[i].x + g_monsters[i].w;
        monsterRc.bottom = g_monsters[i].y + g_monsters[i].h;

        // 몬스터 몸에 닿거나, 몬스터가 점프 공격 중인 몸에 닿으면 37번 프레임 표시
        if (IsRectHit(kirbyRc, monsterRc))
        {
            StartKirbyHitEffect();
            return;
        }
    }
}
