// Player movement, ability, status, and Kirby drawing logic
// Included by game.cpp to keep the existing global-state gameplay unchanged.

// Basic Kirby movement, hit, and HP logic
// This file is included by game.cpp. Do not add it to ClCompile separately.

bool IsMoving()
{
    return moveLeft || moveRight || moveUp || moveDown;
}

bool IsKirbyWalkMoving()
{
    return moveLeft || moveRight;
}

void StopMove()
{
    moveLeft = false;
    moveRight = false;
    moveUp = false;
    moveDown = false;
}

void StartKirbyFallGameOver()
{
    if (isGameOver)
        return;

    g_kirbyFallGameOver = true;
    isGameOver = true;
    g_gameOverHandled = false;
    gameOverTick = 0;

    StopMove();
    isAbsorb = false;
    isSpace = false;
    isSpaceRelease = false;
    isCrouch = false;
    balloonTick = 0;
    spaceKeyHeld = false;
    kirbyVY = 0.0f;
}

bool IsKirbyBelowDeathLine()
{
    return kirbyY > WORLD_H + 80;
}

void StartJump()
{
    if (isOnGround && !jumpKeyDown && !isAbsorb && !isCrouch)
    {
        kirbyVY = jumpPower;
        isOnGround = false;
        jumpKeyDown = true;
    }
}

void UpdateDashWindFrame()
{
    if (isDash && IsMoving() && !isAbsorb && !isCrouch)
    {
        dashFrameTick++;

        if (dashFrameTick >= 10)
        {
            dashFrameTick = 0;

            if (dashFrameIndex < dashFrameCount - 1)
            {
                dashFrameIndex++;
            }
        }
    }
    else
    {
        dashFrameIndex = 0;
        dashFrameTick = 0;
    }
}

void UpdateSpaceRelease()
{
    if (!isSpaceRelease)
        return;

    spaceReleaseTick++;

    if (spaceReleaseTick >= SPACE_RELEASE_DURATION)
    {
        isSpaceRelease = false;
        spaceReleaseTick = 0;
    }
}

void StopBalloonWithRelease()
{
    if (isSpace)
    {
        isSpaceRelease = true;
        spaceReleaseTick = 0;
    }

    isSpace = false;
    balloonTick = 0;

    spaceFrameIndex = 0;
    spaceStartFrameDone = false;

    fireBalloonFrameIndex = 0;
    fireBalloonStartFrameDone = false;

    bombBalloonFrameIndex = 0;
    bombBalloonStartFrameDone = false;

    kirbyVY = 0.0f;
    moveUp = false;
    moveDown = false;
}

void UpdateBalloonLimit()
{
    if (!isSpace)
    {
        balloonTick = 0;
        return;
    }

    balloonTick++;

    if (balloonTick >= BALLOON_DURATION_TICK)
    {
        StopBalloonWithRelease();
    }
}

void UpdateAbsorbFrontEffect()
{
    if (!isAbsorb)
    {
        absorbFrontEffectIndex = 0;
        absorbFrontEffectTick = 0;
        return;
    }

    absorbFrontEffectTick++;

    if (absorbFrontEffectTick >= ABSORB_FRONT_EFFECT_FIRST_DURATION)
    {
        absorbFrontEffectIndex = 1;
    }
    else
    {
        absorbFrontEffectIndex = 0;
    }
}

void SetKirbyNormalSizeKeepBottom()
{
    int oldBottom = kirbyY + kirbyH;

    kirbyW = NORMAL_KIRBY_W;
    kirbyH = NORMAL_KIRBY_H;
    kirbyY = oldBottom - kirbyH;

    if (kirbyY < 0)
        kirbyY = 0;

    if (kirbyY + kirbyH > WORLD_H)
        kirbyY = WORLD_H - kirbyH;
}

void SetKirbyPowerSizeKeepBottom()
{
    int oldBottom = kirbyY + kirbyH;

    kirbyW = POWER_KIRBY_W;
    kirbyH = POWER_KIRBY_H;
    kirbyY = oldBottom - kirbyH;

    if (kirbyY < 0)
        kirbyY = 0;

    int currentWorldW = GetCurrentWorldW();

    if (kirbyX + kirbyW > currentWorldW)
        kirbyX = currentWorldW - kirbyW;

    if (kirbyY + kirbyH > WORLD_H)
        kirbyY = WORLD_H - kirbyH;
}

void UpdatePowerWait()
{
    if (!isPowerKirby)
        return;

    if (canPowerShoot)
        return;

    powerWaitTick++;

    if (powerWaitTick >= POWER_WAIT_TICK_MAX)
    {
        canPowerShoot = true;
    }
}

void UpdateKirbyPosition(HWND hWnd)
{
    if (isGameOver)
        return;

    if (isAbsorb)
    {
        return;
    }

    RECT rt;
    GetClientRect(hWnd, &rt);

    if (isSpace)
    {
        int curBalloonSpeed = isDash ? dashSpeed : balloonSpeed;

        if (g_kirbySlowTick > 0)
        {
            curBalloonSpeed /= 2;
            if (curBalloonSpeed < 1)
                curBalloonSpeed = 1;
        }

        int nextX = kirbyX;
        int nextY = kirbyY;

        if (moveLeft)
            nextX -= curBalloonSpeed;

        if (moveRight)
            nextX += curBalloonSpeed;

        if (moveUp)
            nextY -= curBalloonSpeed;

        if (moveDown)
            nextY += curBalloonSpeed;

        RECT hitBlock;

        // 풍선 상태: X축 충돌 검사
        // 나무발판은 통과하지만, 땅/벽/절벽/기둥은 막음
        RECT nextHitX = GetKirbyHitBox(nextX, kirbyY);

        if (!HitSolidBlockForBalloon(nextHitX, &hitBlock))
        {
            kirbyX = nextX;
        }
        else
        {
            if (moveLeft)
            {
                kirbyX = hitBlock.right - KIRBY_HIT_LEFT;
            }

            if (moveRight)
            {
                kirbyX = hitBlock.left - (kirbyW - KIRBY_HIT_RIGHT);
            }
        }

        // 풍선 상태: Y축 충돌 검사
        RECT nextHitY = GetKirbyHitBox(kirbyX, nextY);

        if (!HitSolidBlockForBalloon(nextHitY, &hitBlock))
        {
            kirbyY = nextY;
        }
        else
        {
            if (moveUp)
            {
                kirbyY = hitBlock.bottom - KIRBY_HIT_TOP;
            }

            if (moveDown)
            {
                kirbyY = hitBlock.top - (kirbyH - KIRBY_HIT_BOTTOM);
            }
        }

        kirbyVY = 0.0f;
        isOnGround = false;

        if (kirbyX < 0)
            kirbyX = 0;

        int currentWorldW = GetCurrentWorldW();

        if (kirbyX + kirbyW > currentWorldW)
            kirbyX = currentWorldW - kirbyW;

        if (kirbyY < 0)
            kirbyY = 0;

        // 아래쪽은 막지 않음. 구멍이나 화면 아래로 빠지면 낙사 처리
        if (IsKirbyBelowDeathLine())
            StartKirbyFallGameOver();

        return;
    }

    int nextX = kirbyX;

    if (!isCrouch)
    {
        int curSpeed = isDash ? dashSpeed : speed;

        if (g_kirbySlowTick > 0)
        {
            curSpeed /= 2;
            if (curSpeed < 1)
                curSpeed = 1;
        }

        if (moveLeft)
            nextX -= curSpeed;

        if (moveRight)
            nextX += curSpeed;
    }

    RECT nextHitX = GetKirbyHitBox(nextX, kirbyY);
    RECT hitBlock;

    if (!HitSolidBlock(nextHitX, &hitBlock))
    {
        kirbyX = nextX;
    }
    else
    {
        if (moveLeft)
        {
            kirbyX = hitBlock.right - KIRBY_HIT_LEFT;
        }

        if (moveRight)
        {
            kirbyX = hitBlock.left - (kirbyW - KIRBY_HIT_RIGHT);
        }
    }

    if (kirbyX < 0)
        kirbyX = 0;

    int currentWorldW = GetCurrentWorldW();

    if (kirbyX + kirbyW > currentWorldW)
        kirbyX = currentWorldW - kirbyW;

    kirbyVY += gravity;

    if (kirbyVY > maxFallSpeed)
        kirbyVY = maxFallSpeed;

    int nextY = kirbyY + (int)kirbyVY;
    RECT nextHitY = GetKirbyHitBox(kirbyX, nextY);

    isOnGround = false;

    if (!HitSolidBlock(nextHitY, &hitBlock))
    {
        kirbyY = nextY;
    }
    else
    {
        if (kirbyVY > 0)
        {
            kirbyY = hitBlock.top - (kirbyH - KIRBY_HIT_BOTTOM);
            kirbyVY = 0;
            isOnGround = true;
        }
        else if (kirbyVY < 0)
        {
            kirbyY = hitBlock.bottom - KIRBY_HIT_TOP;
            kirbyVY = 0;
        }
    }

    RECT currentHit = GetKirbyHitBox(kirbyX, kirbyY);
    int groundY;

    if (kirbyVY >= 0 && FindGroundUnderHitBox(currentHit, &groundY))
    {
        kirbyY = groundY - (kirbyH - KIRBY_HIT_BOTTOM);
        kirbyVY = 0;
        isOnGround = true;
    }

    // 예전처럼 WORLD_H에서 멈추게 하면 구멍으로 떨어져도 바닥에 붙어버림.
    // 이제는 아래로 충분히 빠지면 게임오버 처리함.
    if (IsKirbyBelowDeathLine())
    {
        StartKirbyFallGameOver();
    }
}

bool IsInsideKirby(int mouseX, int mouseY)
{
    return mouseX >= kirbyX &&
        mouseX <= kirbyX + kirbyW &&
        mouseY >= kirbyY &&
        mouseY <= kirbyY + kirbyH;
}

void StartKirbyHitEffect()
{
    if (g_invincibleMode)
        return;

    if (kirbyHitCooldownTick > 0)
        return;

    if (isGameOver)
        return;

    // 피격 프레임 표시
    isKirbyHit = true;
    kirbyHitTick = 0;
    kirbyHitCooldownTick = KIRBY_HIT_COOLDOWN;

    // 몬스터에게 공격받을 때마다 실제 체력 목표값만 15% 감소
    // 화면 체력은 UpdateHPBarAnimation()에서 조금씩 따라 내려감
    kirbyHP -= KIRBY_DAMAGE;

    if (kirbyHP < 0)
    {
        kirbyHP = 0;
    }
}

void UpdateKirbyHitEffect()
{
    if (kirbyHitCooldownTick > 0)
        kirbyHitCooldownTick--;

    if (!isKirbyHit)
        return;

    kirbyHitTick++;

    if (kirbyHitTick >= KIRBY_HIT_DURATION)
    {
        kirbyHitTick = 0;
        isKirbyHit = false;
    }
}

void ApplyKirbyStatusDamage(int damage)
{
    if (g_invincibleMode)
        return;

    if (isGameOver)
        return;

    kirbyHP -= damage;

    if (kirbyHP < 0)
        kirbyHP = 0;
}

void StartKirbySlow()
{
    g_kirbySlowTick = 120; // 약 4.8초
}

void StartKirbyBurn()
{
    g_kirbyBurnTick = 105; // 약 4.2초
    g_kirbyBurnDamageTick = 0;
}

void UpdateKirbyStatusEffects()
{
    if (g_kirbySlowTick > 0)
        g_kirbySlowTick--;

    if (g_kirbyBurnTick > 0)
    {
        g_kirbyBurnTick--;
        g_kirbyBurnDamageTick++;

        if (g_kirbyBurnDamageTick >= 18)
        {
            g_kirbyBurnDamageTick = 0;
            ApplyKirbyStatusDamage(3);
        }
    }
}

void UpdateHPBarAnimation()
{
    // 실제 체력 kirbyHP까지 보이는 체력 kirbyDisplayHP를 조금씩 줄임
    if (kirbyDisplayHP > (float)kirbyHP)
    {
        kirbyDisplayHP -= HP_ANIM_SPEED;

        if (kirbyDisplayHP < (float)kirbyHP)
            kirbyDisplayHP = (float)kirbyHP;
    }

    if (kirbyDisplayHP < 0.0f)
        kirbyDisplayHP = 0.0f;

    // 보이는 체력까지 0이 된 뒤에 게임오버 처리 시작
    if (!isGameOver && kirbyHP <= 0 && kirbyDisplayHP <= 0.0f)
    {
        isGameOver = true;
        g_gameOverHandled = false;
        gameOverTick = 0;
    }
}

RECT GetKirbyBodyRect()
{
    RECT rc;

    rc.left = kirbyX;
    rc.top = kirbyY;
    rc.right = kirbyX + kirbyW;
    rc.bottom = kirbyY + kirbyH;

    return rc;
}

// Power Kirby and ability-star logic
// This file is included by game.cpp. Do not add it to ClCompile separately.

void DigestPowerKirby()
{
    if (!isPowerKirby)
        return;

    // 커진 상태에서 L을 누르면 36번 소화 프레임을 보여줌
    // 먹은 몬스터가 불 속성 1번이면 36번 뒤에 39번을 보여주고 40번 불 커비가 됨
    digestResultType = absorbedMonsterType;

    isPowerKirby = false;
    canPowerShoot = false;
    powerWaitTick = 0;
    powerShotUsed = false;

    isPowerAttack = false;
    powerAttackTick = 0;

    isPowerDigest = true;
    powerDigestTick = 0;

    isAbsorb = false;
    isSpace = false;
    isSpaceRelease = false;
    isCrouch = false;

    absorbFrameIndex = 0;
    absorbFrontEffectIndex = 0;
    absorbFrontEffectTick = 0;

    spaceFrameIndex = 0;
    spaceStartFrameDone = false;

    StopMove();
    SetKirbyNormalSizeKeepBottom();
}

void StartPowerProjectile()
{
    if (!isPowerKirby)
        return;

    // 몬스터를 먹고 1초가 지나기 전에는 K를 눌러도 아무 일도 안 일어남
    if (!canPowerShoot)
        return;

    // 한 번 먹었을 때 발사는 딱 한 번만 가능
    if (powerShotUsed)
        return;

    // 이미 날아가는 34번 프레임이 있으면 새로 만들지 않음
    if (isPowerProjectileActive)
        return;

    powerShotUsed = true;

    isPowerAttack = true;
    powerAttackTick = 0;

    powerProjectileDir = kirbyFaceLeft ? -1 : 1;

    powerProjectileW = 40;
    powerProjectileH = 32;

    if (kirbyFaceLeft)
    {
        powerProjectileX = kirbyX - powerProjectileW + 4;
    }
    else
    {
        powerProjectileX = kirbyX + kirbyW - 4;
    }

    powerProjectilePrevX = powerProjectileX;
    powerProjectileY = kirbyY + kirbyH / 2 - powerProjectileH / 2;
    isPowerProjectileActive = true;

    // 한 번 발사하면 바로 일반 커비 상태와 일반 크기로 복귀
    isPowerKirby = false;
    canPowerShoot = false;
    powerWaitTick = 0;

    SetKirbyNormalSizeKeepBottom();
}

void UpdatePowerAttack()
{
    if (isPowerAttack)
    {
        powerAttackTick++;

        if (powerAttackTick >= POWER_ATTACK_DURATION)
        {
            powerAttackTick = 0;
            isPowerAttack = false;
        }
    }
}

void ClearCurrentAbilityState()
{
    kirbyAbilityType = 0;

    isFireKirby = false;
    isFireTransform = false;
    isFireAttackPose = false;
    isFireBreath = false;
    isFireBallActive = false;
    fireBalloonFrameIndex = 0;
    fireBalloonStartFrameDone = false;

    isBombKirby = false;
    isBombTransform = false;
    isBombAttack = false;
    bombAttackFrameIndex = 0;
    bombAttackTick = 0;
    bombAttackBombSpawned = false;
    bombBalloonFrameIndex = 0;
    bombBalloonStartFrameDone = false;

    isPowerKirby = false;
    isPowerAttack = false;
    isPowerProjectileActive = false;

    isSpace = false;
    isSpaceRelease = false;
    isCrouch = false;
    StopMove();

    SetKirbyNormalSizeKeepBottom();
}

void RestoreAbilityFromStar()
{
    int restoreType = abilityStarType;

    isAbilityStarActive = false;
    abilityStarType = 0;
    abilityStarVX = 0.0f;
    abilityStarVY = 0.0f;

    // 능력별을 다시 빨아들이면 바로 속성으로 돌아가지 않고,
    // 몬스터를 먹었을 때처럼 커진 커비 상태로 보관한다.
    // 여기서 K를 누르면 34번 별을 다시 발사하고, L을 누르면 이전 속성으로 변신한다.
    ClearCurrentAbilityState();

    absorbedMonsterType = restoreType;
    digestResultType = 0;

    isPowerKirby = true;
    SetKirbyPowerSizeKeepBottom();

    // 능력별은 이미 커비 안에 들어온 상태이므로 바로 K/L 선택이 가능하게 둔다.
    powerWaitTick = POWER_WAIT_TICK_MAX;
    canPowerShoot = true;
    powerShotUsed = false;

    isPowerAttack = false;
    powerAttackTick = 0;

    isAbsorb = false;
    absorbFrameIndex = 0;
    absorbFrontEffectIndex = 0;
    absorbFrontEffectTick = 0;

    isSpace = false;
    isSpaceRelease = false;
    isCrouch = false;
    spaceFrameIndex = 0;
    spaceStartFrameDone = false;
    fireBalloonFrameIndex = 0;
    fireBalloonStartFrameDone = false;
    bombBalloonFrameIndex = 0;
    bombBalloonStartFrameDone = false;

    moveUp = false;
    moveDown = false;
}

void EjectAbilityStar()
{
    if (isAbilityStarActive)
        return;

    int currentType = 0;

    if (isFireKirby)
        currentType = 1;
    else if (isBombKirby)
        currentType = 2;
    else
        return;

    abilityStarType = currentType;
    abilityStarW = 51;
    abilityStarH = 48;

    if (kirbyFaceLeft)
    {
        abilityStarX = (float)(kirbyX + kirbyW + 4);
        abilityStarVX = 5.0f;
    }
    else
    {
        abilityStarX = (float)(kirbyX - abilityStarW - 4);
        abilityStarVX = -5.0f;
    }

    abilityStarY = (float)(kirbyY + kirbyH / 2 - abilityStarH / 2);
    abilityStarVY = -5.5f;
    isAbilityStarActive = true;
    abilityStarLifeTick = 0;

    ClearCurrentAbilityState();
}

RECT GetAbilityStarRect()
{
    RECT rc;

    rc.left = (int)abilityStarX;
    rc.top = (int)abilityStarY;
    rc.right = (int)abilityStarX + abilityStarW;
    rc.bottom = (int)abilityStarY + abilityStarH;

    return rc;
}

bool IsAbilityStarInAbsorbRange()
{
    if (!isAbilityStarActive)
        return false;

    RECT starRc = GetAbilityStarRect();

    int kirbyCenterX = kirbyX + kirbyW / 2;
    int starCenterX = (starRc.left + starRc.right) / 2;

    bool overlapY = starRc.bottom > kirbyY && starRc.top < kirbyY + kirbyH;

    if (!overlapY)
        return false;

    if (kirbyFaceLeft)
    {
        int distance = kirbyX - starRc.right;

        if (starCenterX <= kirbyCenterX && distance <= ABSORB_RANGE_X + 20)
            return true;
    }
    else
    {
        int distance = starRc.left - (kirbyX + kirbyW);

        if (starCenterX >= kirbyCenterX && distance <= ABSORB_RANGE_X + 20)
            return true;
    }

    return false;
}

bool IsAbilityStarReachedKirby()
{
    RECT starRc = GetAbilityStarRect();

    int kirbyCenterX = kirbyX + kirbyW / 2;
    int kirbyCenterY = kirbyY + kirbyH / 2;
    int starCenterX = (starRc.left + starRc.right) / 2;
    int starCenterY = (starRc.top + starRc.bottom) / 2;

    int dx = starCenterX - kirbyCenterX;
    int dy = starCenterY - kirbyCenterY;

    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;

    return dx <= 14 && dy <= 22;
}

void UpdateAbilityStar()
{
    if (!isAbilityStarActive)
        return;

    abilityStarLifeTick++;

    if (abilityStarLifeTick >= ABILITY_STAR_LIFE_MAX)
    {
        isAbilityStarActive = false;
        abilityStarType = 0;
        abilityStarLifeTick = 0;
        abilityStarVX = 0.0f;
        abilityStarVY = 0.0f;
        return;
    }

    if (isAbsorb && IsAbilityStarInAbsorbRange())
    {
        if (kirbyFaceLeft)
            abilityStarX += ABILITY_STAR_ABSORB_SPEED;
        else
            abilityStarX -= ABILITY_STAR_ABSORB_SPEED;

        int kirbyCenterY = kirbyY + kirbyH / 2;
        int starCenterY = (int)abilityStarY + abilityStarH / 2;

        if (starCenterY < kirbyCenterY)
            abilityStarY += 2.0f;
        else if (starCenterY > kirbyCenterY)
            abilityStarY -= 2.0f;

        abilityStarVX = 0.0f;
        abilityStarVY = 0.0f;

        if (IsAbilityStarReachedKirby())
        {
            RestoreAbilityFromStar();
        }

        return;
    }

    abilityStarX += abilityStarVX;
    abilityStarY += abilityStarVY;
    abilityStarVY += ABILITY_STAR_GRAVITY;

    RECT starRc = GetAbilityStarRect();
    RECT hitBlock;

    if (HitSolidBlock(starRc, &hitBlock) && abilityStarVY >= 0.0f)
    {
        abilityStarY = (float)(hitBlock.top - abilityStarH);
        abilityStarVY = -abilityStarVY * ABILITY_STAR_BOUNCE;

        if (abilityStarVY > -3.0f)
            abilityStarVY = -3.0f;
    }

    if (abilityStarY + abilityStarH >= WORLD_H)
    {
        abilityStarY = (float)(WORLD_H - abilityStarH);
        abilityStarVY = -abilityStarVY * ABILITY_STAR_BOUNCE;

        if (abilityStarVY > -3.0f)
            abilityStarVY = -3.0f;
    }

    if (abilityStarX < 0)
    {
        abilityStarX = 0;
        abilityStarVX = -abilityStarVX;
    }

    int currentWorldW = GetCurrentWorldW();

    if (abilityStarX + abilityStarW > currentWorldW)
    {
        abilityStarX = (float)(currentWorldW - abilityStarW);
        abilityStarVX = -abilityStarVX;
    }
}

void DrawAbilityStar(Graphics& graphics)
{
    if (!isAbilityStarActive)
        return;

    if (g_powerProjectileFrame == NULL)
        return;

    DrawWorldImage(
        graphics,
        g_powerProjectileFrame,
        (int)abilityStarX,
        (int)abilityStarY,
        abilityStarW,
        abilityStarH
    );
}

void UpdatePowerDigest()
{
    if (!isPowerDigest)
        return;

    powerDigestTick++;

    if (powerDigestTick >= POWER_DIGEST_DURATION)
    {
        powerDigestTick = 0;
        isPowerDigest = false;

        if (digestResultType == 1)
        {
            StartFireKirbyTransform();
        }
        else if (digestResultType == 2)
        {
            StartBombKirbyTransform();
        }
        else
        {
            kirbyAbilityType = 0;
            isFireKirby = false;
            isFireTransform = false;
            isBombKirby = false;
            isBombTransform = false;
            absorbedMonsterType = 0;
            digestResultType = 0;
            SetKirbyNormalSizeKeepBottom();
        }
    }
}

RECT GetPowerProjectileRect()
{
    RECT rc;

    rc.left = powerProjectileX;
    rc.top = powerProjectileY;
    rc.right = powerProjectileX + powerProjectileW;
    rc.bottom = powerProjectileY + powerProjectileH;

    return rc;
}

RECT GetPowerProjectileSweepRect()
{
    RECT rc;

    // 이전 위치와 현재 위치를 모두 포함하는 사각형을 만들어서
    // 발사체가 빨리 움직여도 몬스터를 뚫고 지나가는 문제를 줄임
    int oldLeft = powerProjectilePrevX;
    int oldRight = powerProjectilePrevX + powerProjectileW;
    int newLeft = powerProjectileX;
    int newRight = powerProjectileX + powerProjectileW;

    rc.left = oldLeft < newLeft ? oldLeft : newLeft;
    rc.right = oldRight > newRight ? oldRight : newRight;
    rc.top = powerProjectileY;
    rc.bottom = powerProjectileY + powerProjectileH;

    return rc;
}

void CheckPowerProjectileHitMonsters()
{
    if (!isPowerProjectileActive)
        return;

    RECT projectileRc = GetPowerProjectileSweepRect();

    for (int i = 0; i < MONSTER_COUNT; i++)
    {
        if (!g_monsters[i].active)
            continue;

        // 공격 판정은 몬스터 보정 히트박스가 아니라 몬스터 전체 크기로 검사
        // 그래야 PNG34가 보기에는 닿았는데 안 맞는 느낌이 줄어듦
        RECT monsterRc;
        monsterRc.left = g_monsters[i].x;
        monsterRc.top = g_monsters[i].y;
        monsterRc.right = g_monsters[i].x + g_monsters[i].w;
        monsterRc.bottom = g_monsters[i].y + g_monsters[i].h;

        if (IsRectHit(projectileRc, monsterRc))
        {
            // 충돌하면 발사체는 사라지고, 몬스터는 35번 죽는 프레임을 잠깐 보여준 뒤 사라짐
            g_monsters[i].StartDeadEffect();

            isPowerProjectileActive = false;
            return;
        }
    }
}

void DrawPowerProjectile(Graphics& graphics)
{
    if (!isPowerProjectileActive)
        return;

    if (g_powerProjectileFrame == NULL)
        return;

    if (powerProjectileDir < 0)
    {
        DrawImageFlipX(
            graphics,
            g_powerProjectileFrame,
            powerProjectileX,
            powerProjectileY,
            powerProjectileW,
            powerProjectileH
        );
    }
    else
    {
        DrawWorldImage(
            graphics,
            g_powerProjectileFrame,
            powerProjectileX,
            powerProjectileY,
            powerProjectileW,
            powerProjectileH
        );
    }
}

// Fire Kirby and fire projectile logic
// This file is included by game.cpp. Do not add it to ClCompile separately.

void StartFireAttackPose()
{
    if (!isFireKirby)
        return;

    isFireAttackPose = true;
    fireAttackPoseTick = 0;
}

void StartFireBreath()
{
    if (!isFireKirby)
        return;

    StartFireAttackPose();

    isFireBreath = true;
    fireBreathTick = 0;
}

RECT GetFireBreathRect()
{
    RECT rc;

    int breathW = 58;
    int breathH = 36;
    int breathY = kirbyY + kirbyH / 2 - breathH / 2;

    if (kirbyFaceLeft)
    {
        rc.left = kirbyX - breathW + 5;
        rc.right = kirbyX + 5;
    }
    else
    {
        rc.left = kirbyX + kirbyW - 5;
        rc.right = rc.left + breathW;
    }

    rc.top = breathY;
    rc.bottom = breathY + breathH;

    return rc;
}

void SpawnFireBall()
{
    if (!isFireKirby)
        return;

    if (isFireBallActive)
        return;

    StartFireAttackPose();

    fireBallDir = kirbyFaceLeft ? -1 : 1;
    fireBallW = 40;
    fireBallH = 28;

    if (kirbyFaceLeft)
        fireBallX = kirbyX - fireBallW + 4;
    else
        fireBallX = kirbyX + kirbyW - 4;

    fireBallPrevX = fireBallX;
    fireBallY = kirbyY + kirbyH / 2 - fireBallH / 2;
    isFireBallActive = true;
}

RECT GetFireBallSweepRect()
{
    RECT rc;

    int oldLeft = fireBallPrevX;
    int oldRight = fireBallPrevX + fireBallW;
    int newLeft = fireBallX;
    int newRight = fireBallX + fireBallW;

    rc.left = oldLeft < newLeft ? oldLeft : newLeft;
    rc.right = oldRight > newRight ? oldRight : newRight;
    rc.top = fireBallY;
    rc.bottom = fireBallY + fireBallH;

    return rc;
}

void UpdateFireKirbyStates()
{
    if (isFireAttackPose)
    {
        fireAttackPoseTick++;

        if (fireAttackPoseTick >= FIRE_ATTACK_POSE_DURATION)
        {
            fireAttackPoseTick = 0;
            isFireAttackPose = false;
        }
    }

    if (isFireTransform)
    {
        fireTransformTick++;

        if (fireTransformTick >= FIRE_TRANSFORM_DURATION)
        {
            fireTransformTick = 0;
            isFireTransform = false;
            isFireKirby = true;
            kirbyAbilityType = 1;
        }
    }

    if (isBombTransform)
    {
        bombTransformTick++;

        if (bombTransformTick >= BOMB_TRANSFORM_DURATION)
        {
            bombTransformTick = 0;
            isBombTransform = false;
            isBombKirby = true;
            kirbyAbilityType = 2;
        }
    }

    if (isFireBreath)
    {
        fireBreathTick++;

        if (fireBreathTick >= FIRE_BREATH_DURATION)
        {
            fireBreathTick = 0;
            isFireBreath = false;
        }
    }

    if (isFireBallActive)
    {
        fireBallPrevX = fireBallX;
        fireBallX += fireBallSpeed * fireBallDir;

        if (fireBallX + fireBallW < 0 || fireBallX > GetCurrentWorldW())
            isFireBallActive = false;
    }
}

void SpawnEnemyFireBall(int x, int y, int dir)
{
    for (int i = 0; i < ENEMY_FIREBALL_MAX; i++)
    {
        if (!g_enemyFireBalls[i].active)
        {
            g_enemyFireBalls[i].active = true;
            g_enemyFireBalls[i].x = x;
            g_enemyFireBalls[i].y = y;
            g_enemyFireBalls[i].prevX = x;
            g_enemyFireBalls[i].w = 36;
            g_enemyFireBalls[i].h = 24;
            g_enemyFireBalls[i].dir = dir;
            g_enemyFireBalls[i].speed = 7;
            return;
        }
    }
}

RECT GetEnemyFireBallSweepRect(int index)
{
    RECT rc;

    int oldLeft = g_enemyFireBalls[index].prevX;
    int oldRight = g_enemyFireBalls[index].prevX + g_enemyFireBalls[index].w;
    int newLeft = g_enemyFireBalls[index].x;
    int newRight = g_enemyFireBalls[index].x + g_enemyFireBalls[index].w;

    rc.left = oldLeft < newLeft ? oldLeft : newLeft;
    rc.right = oldRight > newRight ? oldRight : newRight;
    rc.top = g_enemyFireBalls[index].y;
    rc.bottom = g_enemyFireBalls[index].y + g_enemyFireBalls[index].h;

    return rc;
}

void UpdateEnemyFireBalls()
{
    for (int i = 0; i < ENEMY_FIREBALL_MAX; i++)
    {
        if (!g_enemyFireBalls[i].active)
            continue;

        g_enemyFireBalls[i].prevX = g_enemyFireBalls[i].x;
        g_enemyFireBalls[i].x += g_enemyFireBalls[i].speed * g_enemyFireBalls[i].dir;

        if (g_enemyFireBalls[i].x + g_enemyFireBalls[i].w < 0 ||
            g_enemyFireBalls[i].x > GetCurrentWorldW())
        {
            g_enemyFireBalls[i].active = false;
        }
    }
}

void CheckEnemyFireBallsHitKirby()
{
    // 빨아들이기 중이어도 적 공격에는 맞게 함
    if (isKirbyHit || kirbyHitCooldownTick > 0)
        return;

    RECT kirbyRc = GetKirbyBodyRect();

    for (int i = 0; i < ENEMY_FIREBALL_MAX; i++)
    {
        if (!g_enemyFireBalls[i].active)
            continue;

        RECT fireRc = GetEnemyFireBallSweepRect(i);

        if (IsRectHit(kirbyRc, fireRc))
        {
            g_enemyFireBalls[i].active = false;
            StartKirbyHitEffect();
            return;
        }
    }
}

void StartFireKirbyTransform()
{
    kirbyAbilityType = 1;
    isBombKirby = false;
    isBombTransform = false;
    bombTransformTick = 0;
    isFireKirby = false;
    isFireTransform = true;
    fireTransformTick = 0;

    isFireAttackPose = false;
    fireAttackPoseTick = 0;
    fireBalloonFrameIndex = 0;
    fireBalloonStartFrameDone = false;

    isPowerKirby = false;
    isPowerAttack = false;
    isPowerProjectileActive = false;
    isAbsorb = false;
    isSpace = false;
    isSpaceRelease = false;
    isCrouch = false;

    absorbedMonsterType = 0;
    digestResultType = 0;

    SetKirbyNormalSizeKeepBottom();
}

void CheckFireAttacksHitMonsters()
{
    RECT fireBreathRc;
    bool hasBreath = false;

    if (isFireBreath)
    {
        fireBreathRc = GetFireBreathRect();
        hasBreath = true;
    }

    RECT fireBallRc;
    bool hasFireBall = false;

    if (isFireBallActive)
    {
        fireBallRc = GetFireBallSweepRect();
        hasFireBall = true;
    }

    if (!hasBreath && !hasFireBall)
        return;

    for (int i = 0; i < MONSTER_COUNT; i++)
    {
        if (!g_monsters[i].active)
            continue;

        RECT monsterRc;
        monsterRc.left = g_monsters[i].x;
        monsterRc.top = g_monsters[i].y;
        monsterRc.right = g_monsters[i].x + g_monsters[i].w;
        monsterRc.bottom = g_monsters[i].y + g_monsters[i].h;

        if (hasBreath && IsRectHit(fireBreathRc, monsterRc))
        {
            g_monsters[i].StartDeadEffect();
            continue;
        }

        if (hasFireBall && IsRectHit(fireBallRc, monsterRc))
        {
            g_monsters[i].StartDeadEffect();
            isFireBallActive = false;
            return;
        }
    }
}

void DrawFireBreath(Graphics& graphics)
{
    if (!isFireBreath)
        return;

    if (g_fireBreathFrame == NULL)
        return;

    RECT rc = GetFireBreathRect();
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;

    if (kirbyFaceLeft)
    {
        DrawImageFlipX(graphics, g_fireBreathFrame, rc.left, rc.top, w, h);
    }
    else
    {
        DrawWorldImage(graphics, g_fireBreathFrame, rc.left, rc.top, w, h);
    }
}

void DrawFireBall(Graphics& graphics)
{
    if (!isFireBallActive)
        return;

    if (g_fireBallFrame == NULL)
        return;

    if (fireBallDir < 0)
    {
        DrawImageFlipX(graphics, g_fireBallFrame, fireBallX, fireBallY, fireBallW, fireBallH);
    }
    else
    {
        DrawWorldImage(graphics, g_fireBallFrame, fireBallX, fireBallY, fireBallW, fireBallH);
    }
}

void DrawEnemyFireBalls(Graphics& graphics)
{
    if (g_fireMonsterAttackFrame == NULL)
        return;

    for (int i = 0; i < ENEMY_FIREBALL_MAX; i++)
    {
        if (!g_enemyFireBalls[i].active)
            continue;

        if (g_enemyFireBalls[i].dir < 0)
        {
            DrawImageFlipX(
                graphics,
                g_fireMonsterAttackFrame,
                g_enemyFireBalls[i].x,
                g_enemyFireBalls[i].y,
                g_enemyFireBalls[i].w,
                g_enemyFireBalls[i].h
            );
        }
        else
        {
            DrawWorldImage(
                graphics,
                g_fireMonsterAttackFrame,
                g_enemyFireBalls[i].x,
                g_enemyFireBalls[i].y,
                g_enemyFireBalls[i].w,
                g_enemyFireBalls[i].h
            );
        }
    }
}

// Bomb Kirby and bomb projectile logic
// This file is included by game.cpp. Do not add it to ClCompile separately.

void StartBombKirbyTransform()
{
    kirbyAbilityType = 2;

    // 폭탄 속성 몬스터를 소화한 뒤에는 바로 54번으로 가지 않고,
    // 먼저 68번 변신 프레임을 잠깐 보여준 뒤 폭탄 커비 상태가 됨
    isBombKirby = false;
    isBombTransform = true;
    bombTransformTick = 0;

    isFireKirby = false;
    isFireTransform = false;
    isFireAttackPose = false;
    isFireBreath = false;
    isFireBallActive = false;

    isPowerKirby = false;
    isPowerAttack = false;
    isPowerProjectileActive = false;
    isAbsorb = false;
    isSpace = false;
    isSpaceRelease = false;
    isCrouch = false;

    bombWalkFrameIndex = 0;
    bombBalloonFrameIndex = 0;
    bombBalloonStartFrameDone = false;
    isBombAttack = false;
    bombAttackFrameIndex = 0;
    bombAttackTick = 0;
    bombAttackBombSpawned = false;

    absorbedMonsterType = 0;
    digestResultType = 0;

    SetKirbyNormalSizeKeepBottom();
}

void SpawnBombExplosion(int x, int y)
{
    for (int i = 0; i < BOMB_EXPLOSION_MAX; i++)
    {
        if (!g_bombExplosions[i].active)
        {
            g_bombExplosions[i].active = true;
            g_bombExplosions[i].w = 56;
            g_bombExplosions[i].h = 40;
            g_bombExplosions[i].x = x - g_bombExplosions[i].w / 2;
            g_bombExplosions[i].y = y - g_bombExplosions[i].h + 6;
            g_bombExplosions[i].tick = 0;

            RECT explosionRc;
            explosionRc.left = g_bombExplosions[i].x;
            explosionRc.top = g_bombExplosions[i].y;
            explosionRc.right = g_bombExplosions[i].x + g_bombExplosions[i].w;
            explosionRc.bottom = g_bombExplosions[i].y + g_bombExplosions[i].h;

            CheckBombExplosionHitKirby(explosionRc);
            CheckBombHitMonsters(explosionRc, false);
            return;
        }
    }
}

void SpawnBombObjectEx(int x, int y, int w, int h, float vx, float vy, bool fromEnemy, int damage, bool bounce)
{
    for (int i = 0; i < BOMB_OBJECT_MAX; i++)
    {
        if (!g_bombs[i].active)
        {
            g_bombs[i].active = true;
            g_bombs[i].fromEnemy = fromEnemy;
            g_bombs[i].x = x;
            g_bombs[i].y = y;
            g_bombs[i].w = w;
            g_bombs[i].h = h;
            g_bombs[i].damage = damage;
            g_bombs[i].bounce = bounce;
            g_bombs[i].vx = vx;
            g_bombs[i].vy = vy;
            return;
        }
    }
}

void SpawnBombObject(int x, int y, float vx, float vy, bool fromEnemy)
{
    SpawnBombObjectEx(x, y, 34, 34, vx, vy, fromEnemy, 28, false);
}

void StartBombAttack()
{
    if (!isBombKirby)
        return;

    if (isBombAttack)
        return;

    if (g_bombKCooldownTick > 0)
        return;

    g_bombKCooldownTick = BOMB_K_COOLDOWN_MAX;
    g_bombSpecialAttackMode = false;
    isBombAttack = true;
    bombAttackFrameIndex = 0;
    bombAttackTick = 0;
    bombAttackBombSpawned = false;

    isSpace = false;
    isSpaceRelease = false;
    isCrouch = false;
    StopMove();
}

void StartBombSpecialAttack()
{
    if (!isBombKirby)
        return;

    if (isBombAttack)
        return;

    if (g_bombICooldownTick > 0)
        return;

    g_bombICooldownTick = BOMB_I_COOLDOWN_MAX;
    g_bombSpecialAttackMode = true;
    isBombAttack = true;
    bombAttackFrameIndex = 0;
    bombAttackTick = 0;
    bombAttackBombSpawned = false;

    isSpace = false;
    isSpaceRelease = false;
    isCrouch = false;
    StopMove();
}

void UpdateBombAttack()
{
    if (!isBombAttack)
        return;

    bombAttackTick++;

    if (bombAttackFrameIndex == 1 && !bombAttackBombSpawned)
    {
        int dir = kirbyFaceLeft ? -1 : 1;

        if (g_bombSpecialAttackMode)
        {
            // I 필살기: K 폭탄 프레임을 3배 크기로 던지고 바닥을 계속 튕기며 이동
            int bigW = 102;
            int bigH = 102;
            int bombX = kirbyFaceLeft ? kirbyX - bigW + 8 : kirbyX + kirbyW - 8;
            int bombY = kirbyY + kirbyH / 2 - bigH / 2;

            SpawnBombObjectEx(bombX, bombY, bigW, bigH, 10.5f * dir, -9.5f, false, 84, true);
        }
        else
        {
            // K 일반 공격: 폭탄 하나만 빠르게 던짐
            int bombX = kirbyFaceLeft ? kirbyX - 28 : kirbyX + kirbyW - 6;
            int bombY = kirbyY + kirbyH / 2 - 22;

            SpawnBombObjectEx(bombX, bombY, 34, 34, 8.5f * dir, -8.5f, false, 28, false);
        }

        bombAttackBombSpawned = true;
    }

    if (bombAttackTick >= BOMB_ATTACK_FRAME_DURATION)
    {
        bombAttackTick = 0;
        bombAttackFrameIndex++;

        if (bombAttackFrameIndex >= 3)
        {
            isBombAttack = false;
            bombAttackFrameIndex = 0;
            bombAttackBombSpawned = false;
            g_bombSpecialAttackMode = false;
        }
    }
}

void CheckBombHitMonsters(RECT bombRc, bool fromEnemy)
{
    if (fromEnemy)
        return;

    for (int i = 0; i < MONSTER_COUNT; i++)
    {
        if (!g_monsters[i].active)
            continue;

        RECT monsterRc;
        monsterRc.left = g_monsters[i].x;
        monsterRc.top = g_monsters[i].y;
        monsterRc.right = g_monsters[i].x + g_monsters[i].w;
        monsterRc.bottom = g_monsters[i].y + g_monsters[i].h;

        if (IsRectHit(bombRc, monsterRc))
        {
            g_monsters[i].StartDeadEffect();
        }
    }
}

void CheckBombExplosionHitKirby(RECT explosionRc)
{
    // 빨아들이기 중이어도 폭발에는 맞게 함
    if (isKirbyHit || kirbyHitCooldownTick > 0)
        return;

    RECT kirbyRc = GetKirbyBodyRect();

    if (IsRectHit(kirbyRc, explosionRc))
    {
        StartKirbyHitEffect();
    }
}

void UpdateBombObjects()
{
    for (int i = 0; i < BOMB_OBJECT_MAX; i++)
    {
        if (!g_bombs[i].active)
            continue;

        g_bombs[i].x += (int)g_bombs[i].vx;
        g_bombs[i].y += (int)g_bombs[i].vy;
        g_bombs[i].vy += 0.45f;

        RECT bombRc;
        bombRc.left = g_bombs[i].x;
        bombRc.top = g_bombs[i].y;
        bombRc.right = g_bombs[i].x + g_bombs[i].w;
        bombRc.bottom = g_bombs[i].y + g_bombs[i].h;

        CheckBombHitMonsters(bombRc, g_bombs[i].fromEnemy);

        RECT hitBlock;
        bool hitGround = false;

        if (HitSolidBlock(bombRc, &hitBlock) && g_bombs[i].vy >= 0)
        {
            hitGround = true;
            g_bombs[i].y = hitBlock.top - g_bombs[i].h;
        }

        if (g_bombs[i].y + g_bombs[i].h >= WORLD_H)
        {
            hitGround = true;
            g_bombs[i].y = WORLD_H - g_bombs[i].h;
        }

        if (hitGround)
        {
            if (g_bombs[i].bounce)
            {
                // I 필살기 폭탄은 바닥에서 계속 튕기면서 앞으로 굴러가듯 이동
                g_bombs[i].vy = -8.5f;
                continue;
            }

            SpawnBombExplosion(g_bombs[i].x + g_bombs[i].w / 2, g_bombs[i].y + g_bombs[i].h);
            g_bombs[i].active = false;
            continue;
        }

        if (g_bombs[i].x + g_bombs[i].w < 0 || g_bombs[i].x > GetCurrentWorldW() || g_bombs[i].y > WORLD_H + 100)
        {
            g_bombs[i].active = false;
        }
    }

    for (int i = 0; i < BOMB_EXPLOSION_MAX; i++)
    {
        if (!g_bombExplosions[i].active)
            continue;

        g_bombExplosions[i].tick++;

        if (g_bombExplosions[i].tick >= BOMB_EXPLOSION_DURATION)
        {
            g_bombExplosions[i].active = false;
            g_bombExplosions[i].tick = 0;
        }
    }
}

void DrawBombObjects(Graphics& graphics)
{
    if (g_bombProjectileFrame == NULL)
        return;

    for (int i = 0; i < BOMB_OBJECT_MAX; i++)
    {
        if (!g_bombs[i].active)
            continue;

        DrawWorldImage(graphics, g_bombProjectileFrame, g_bombs[i].x, g_bombs[i].y, g_bombs[i].w, g_bombs[i].h);
    }
}

void DrawBombExplosions(Graphics& graphics)
{
    // 68번 폭발 프레임은 사용하지 않음.
    // 폭탄이 바닥에 닿으면 내부 판정만 처리하고 화면에는 폭발 이미지를 그리지 않음.
    return;
}

void DrawImageFlipX(Graphics& graphics, Image* image, int x, int y, int w, int h)
{
    if (image == NULL)
        return;

    if (!IsVisibleWorld(x, y, w, h))
        return;

    GraphicsState state = graphics.Save();

    graphics.TranslateTransform((REAL)(x + w), (REAL)y);
    graphics.ScaleTransform(-1.0f, 1.0f);

    graphics.DrawImage(image, 0, 0, w, h);

    graphics.Restore(state);
}

void DrawKirbyImage(Graphics& graphics, Image* image)
{
    if (image == NULL)
        return;

    if (kirbyFaceLeft)
    {
        DrawImageFlipX(graphics, image, kirbyX, kirbyY, kirbyW, kirbyH);
    }
    else
    {
        DrawWorldImage(graphics, image, kirbyX, kirbyY, kirbyW, kirbyH);
    }
}
