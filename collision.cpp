// Collision and hitbox helpers
// Included by game.cpp to keep the existing global-state gameplay unchanged.

RECT MakeRectFromXYWH(int x, int y, int w, int h)
{
    RECT rc;

    rc.left = x;
    rc.top = y;
    rc.right = x + w;
    rc.bottom = y + h;

    return rc;
}

bool IsRectHit(RECT a, RECT b)
{
    if (a.right <= b.left) return false;
    if (a.left >= b.right) return false;
    if (a.bottom <= b.top) return false;
    if (a.top >= b.bottom) return false;

    return true;
}

bool HitSolidBlock(RECT rc, RECT* hitBlock)
{
    int blockCount = 0;
    SolidBlock* blocks = GetCurrentSolidBlocks(&blockCount);

    for (int i = 0; i < blockCount; i++)
    {
        if (IsRectHit(rc, blocks[i].rc))
        {
            if (hitBlock != NULL)
            {
                *hitBlock = blocks[i].rc;
            }

            return true;
        }
    }

    return false;
}

bool IsWoodPlatformBlock(LPCWSTR name)
{
    if (name == NULL)
        return false;

    // 이름에 WOOD가 들어간 충돌체는 나무발판으로 판단
    if (wcsstr(name, L"WOOD") != NULL)
        return true;

    return false;
}

bool HitSolidBlockForBalloon(RECT rc, RECT* hitBlock)
{
    int blockCount = 0;
    SolidBlock* blocks = GetCurrentSolidBlocks(&blockCount);

    for (int i = 0; i < blockCount; i++)
    {
        // 풍선 상태에서는 나무발판만 통과 가능하게 함
        if (IsWoodPlatformBlock(blocks[i].name))
            continue;

        if (IsRectHit(rc, blocks[i].rc))
        {
            if (hitBlock != NULL)
            {
                *hitBlock = blocks[i].rc;
            }

            return true;
        }
    }

    return false;
}

bool FindGroundUnderHitBox(RECT hitBox, int* groundY)
{
    int bestY = 999999;
    bool found = false;

    int blockCount = 0;
    SolidBlock* blocks = GetCurrentSolidBlocks(&blockCount);

    for (int i = 0; i < blockCount; i++)
    {
        RECT block = blocks[i].rc;

        bool overlapX = hitBox.right > block.left && hitBox.left < block.right;

        if (overlapX)
        {
            if (hitBox.bottom >= block.top && hitBox.bottom <= block.top + 20)
            {
                if (block.top < bestY)
                {
                    bestY = block.top;
                    found = true;
                }
            }
        }
    }

    if (found)
    {
        *groundY = bestY;
        return true;
    }

    return false;
}

RECT GetKirbyHitBox(int x, int y)
{
    RECT rc;

    rc.left = x + KIRBY_HIT_LEFT;
    rc.top = y + KIRBY_HIT_TOP;
    rc.right = x + kirbyW - KIRBY_HIT_RIGHT;
    rc.bottom = y + kirbyH - KIRBY_HIT_BOTTOM;

    return rc;
}

RECT GetMonsterHitBox(int x, int y, int w, int h)
{
    RECT rc;

    rc.left = x + MONSTER_HIT_LEFT;
    rc.top = y + MONSTER_HIT_TOP;
    rc.right = x + w - MONSTER_HIT_RIGHT;
    rc.bottom = y + h - MONSTER_HIT_BOTTOM;

    return rc;
}
