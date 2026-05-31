#pragma once

#include <windows.h>
#include <gdiplus.h>

class Monster
{
public:
    int x;
    int y;
    int w;
    int h;

    int speed;
    int dir;

    int frameIndex;
    int frameCount;

    int leftLimit;
    int rightLimit;

    int attackRange;
    bool isAttack;

    float vy;
    bool onGround;

    bool active;

    // 몬스터 속성 번호. 0 = 일반, 1 = 불, 2 = 폭탄, 3 = 망치, 4 = 스파크
    int monsterType;

    // 불 속성 몬스터 원거리 공격
    int rangedAttackCooldown;
    int rangedAttackFrameTick;

    // 폭탄 몬스터 투하 공격
    int bombDropCooldown;

    // 망치 몬스터 근접 공격
    int hammerAttackFrameIndex;
    int hammerAttackFrameTick;
    int hammerAttackCooldown;
    bool hammerAttackHitDone;

    // 스파크 몬스터 근접 전기 공격
    int sparkAttackFrameIndex;
    int sparkAttackFrameTick;
    int sparkAttackCooldown;
    bool sparkAttackHitDone;

    // 발사체에 맞아 죽는 연출 상태
    bool isDeadEffect;
    int deadEffectTick;

    bool isJumpAttack;
    int jumpAttackFrameIndex;
    int jumpAttackCooldown;
    float jumpAttackVX;

    Monster()
    {
        x = 600;
        y = 470;
        w = 32;
        h = 32;

        speed = 2;
        dir = -1;

        frameIndex = 0;
        frameCount = 4;

        leftLimit = 546;
        rightLimit = 914 - w;

        attackRange = 140;
        isAttack = false;

        vy = 0.0f;
        onGround = false;

        active = true;
        monsterType = 0;
        rangedAttackCooldown = 90;
        rangedAttackFrameTick = 0;
        bombDropCooldown = 100;
        hammerAttackFrameIndex = 0;
        hammerAttackFrameTick = 0;
        hammerAttackCooldown = 60;
        hammerAttackHitDone = false;
        sparkAttackFrameIndex = 0;
        sparkAttackFrameTick = 0;
        sparkAttackCooldown = 75;
        sparkAttackHitDone = false;
        isDeadEffect = false;
        deadEffectTick = 0;

        isJumpAttack = false;
        jumpAttackFrameIndex = 0;
        jumpAttackCooldown = 0;
        jumpAttackVX = 0.0f;
    }

    void Init(int startX, int startY, int patrolLeft, int patrolRight, int startDir, int type = 0)
    {
        x = startX;
        y = startY;
        if (type == 3)
        {
            w = HAMMER_MONSTER_W;
            h = HAMMER_MONSTER_H;
        }
        else if (type == 4)
        {
            w = SPARK_MONSTER_W;
            h = SPARK_MONSTER_H;
        }
        else
        {
            w = 32;
            h = 32;
        }

        speed = 2;
        dir = startDir;

        frameIndex = 0;
        frameCount = 4;

        leftLimit = patrolLeft;
        rightLimit = patrolRight - w;

        attackRange = (type == 3) ? 90 : ((type == 4) ? 115 : 140);
        isAttack = false;

        vy = 0.0f;
        onGround = false;

        active = true;
        monsterType = type;
        frameCount = (type == 3) ? HAMMER_MONSTER_WALK_FRAME_COUNT : ((type == 4) ? SPARK_MONSTER_WALK_FRAME_COUNT : 4);
        rangedAttackCooldown = 90;
        rangedAttackFrameTick = 0;
        bombDropCooldown = 100;
        hammerAttackFrameIndex = 0;
        hammerAttackFrameTick = 0;
        hammerAttackCooldown = 60;
        hammerAttackHitDone = false;
        sparkAttackFrameIndex = 0;
        sparkAttackFrameTick = 0;
        sparkAttackCooldown = 75;
        sparkAttackHitDone = false;
        isDeadEffect = false;
        deadEffectTick = 0;

        isJumpAttack = false;
        jumpAttackFrameIndex = 0;
        jumpAttackCooldown = 0;
        jumpAttackVX = 0.0f;
    }

    void StartDeadEffect()
    {
        active = false;
        isAttack = false;
        isJumpAttack = false;
        hammerAttackFrameIndex = 0;
        hammerAttackFrameTick = 0;
        hammerAttackHitDone = false;
        sparkAttackFrameIndex = 0;
        sparkAttackFrameTick = 0;
        sparkAttackHitDone = false;
        vy = 0.0f;

        isDeadEffect = true;
        deadEffectTick = 0;
    }

    void UpdateDeadEffect()
    {
        if (!isDeadEffect)
            return;

        deadEffectTick++;

        if (deadEffectTick >= MONSTER_DEAD_DURATION)
        {
            isDeadEffect = false;
            deadEffectTick = 0;
        }
    }

    bool IsKirbyInRangedAttackRange()
    {
        if (monsterType != 1)
            return false;

        int kirbyCenterX = kirbyX + kirbyW / 2;
        int kirbyCenterY = kirbyY + kirbyH / 2;
        int monsterCenterX = x + w / 2;
        int monsterCenterY = y + h / 2;

        int dx = kirbyCenterX - monsterCenterX;
        int dy = kirbyCenterY - monsterCenterY;

        if (dx < 0) dx = -dx;
        if (dy < 0) dy = -dy;

        return dx <= 260 && dy <= 90;
    }

    void TryRangedAttack()
    {
        if (monsterType != 1)
            return;

        if (rangedAttackCooldown > 0)
            rangedAttackCooldown--;

        if (rangedAttackFrameTick > 0)
            rangedAttackFrameTick--;

        if (rangedAttackCooldown > 0)
            return;

        if (!IsKirbyInRangedAttackRange())
            return;

        int kirbyCenterX = kirbyX + kirbyW / 2;
        int monsterCenterX = x + w / 2;

        if (kirbyCenterX < monsterCenterX)
            dir = -1;
        else
            dir = 1;

        int bulletX = (dir == -1) ? x - 30 : x + w;
        int bulletY = y + h / 2 - 12;

        SpawnEnemyFireBall(bulletX, bulletY, dir);

        rangedAttackFrameTick = 18;
        rangedAttackCooldown = 90;
    }


    bool IsKirbyBelowForBombDrop()
    {
        if (monsterType != 2)
            return false;

        int kirbyCenterX = kirbyX + kirbyW / 2;
        int monsterCenterX = x + w / 2;
        int dx = kirbyCenterX - monsterCenterX;
        if (dx < 0) dx = -dx;

        return dx <= 70 && kirbyY > y;
    }

    void TryBombDropAttack()
    {
        if (monsterType != 2)
            return;

        if (bombDropCooldown > 0)
            bombDropCooldown--;

        if (bombDropCooldown > 0)
            return;

        if (!IsKirbyBelowForBombDrop())
            return;

        SpawnBombObject(x + w / 2 - 17, y + h, 0.0f, 1.0f, true);
        bombDropCooldown = 100;
    }

    bool IsKirbyInHammerAttackRange()
    {
        if (monsterType != 3)
            return false;

        int kirbyCenterX = kirbyX + kirbyW / 2;
        int kirbyCenterY = kirbyY + kirbyH / 2;
        int monsterCenterX = x + w / 2;
        int monsterCenterY = y + h / 2;

        int dx = kirbyCenterX - monsterCenterX;
        int dy = kirbyCenterY - monsterCenterY;

        if (dx < 0) dx = -dx;
        if (dy < 0) dy = -dy;

        return dx <= 76 && dy <= 55;
    }

    RECT GetHammerMonsterAttackRect()
    {
        RECT rc;
        int attackW = 46;
        int attackH = 34;
        int attackY = y + h / 2 - attackH / 2;

        if (dir < 0)
        {
            rc.left = x - attackW + 12;
            rc.right = x + 12;
        }
        else
        {
            rc.left = x + w - 12;
            rc.right = rc.left + attackW;
        }

        rc.top = attackY;
        rc.bottom = attackY + attackH;

        return rc;
    }

    void StartHammerMonsterAttack()
    {
        int kirbyCenterX = kirbyX + kirbyW / 2;
        int monsterCenterX = x + w / 2;

        dir = (kirbyCenterX < monsterCenterX) ? -1 : 1;
        isAttack = true;
        hammerAttackFrameIndex = 0;
        hammerAttackFrameTick = 0;
        hammerAttackHitDone = false;
        hammerAttackCooldown = 80;
        isJumpAttack = false;
    }

    void UpdateHammerMonsterAttack()
    {
        if (monsterType != 3)
            return;

        if (hammerAttackCooldown > 0)
            hammerAttackCooldown--;

        if (!isAttack)
        {
            if (hammerAttackCooldown <= 0 && IsKirbyInHammerAttackRange())
                StartHammerMonsterAttack();
            return;
        }

        hammerAttackFrameTick++;

        if (!hammerAttackHitDone && hammerAttackFrameIndex >= 1 && hammerAttackFrameIndex <= 2)
        {
            RECT attackRc = GetHammerMonsterAttackRect();
            RECT kirbyRc = GetKirbyBodyRect();

            if (IsRectHit(attackRc, kirbyRc) && !isKirbyHit && kirbyHitCooldownTick <= 0)
            {
                StartKirbyHitEffect();
                hammerAttackHitDone = true;
            }
        }

        if (hammerAttackFrameTick >= 5)
        {
            hammerAttackFrameTick = 0;
            hammerAttackFrameIndex++;

            if (hammerAttackFrameIndex >= HAMMER_MONSTER_ATTACK_FRAME_COUNT)
            {
                isAttack = false;
                hammerAttackFrameIndex = 0;
                hammerAttackHitDone = false;
            }
        }
    }

    bool IsKirbyInSparkAttackRange()
    {
        if (monsterType != 4)
            return false;

        int kirbyCenterX = kirbyX + kirbyW / 2;
        int kirbyCenterY = kirbyY + kirbyH / 2;
        int monsterCenterX = x + w / 2;
        int monsterCenterY = y + h / 2;

        int dx = kirbyCenterX - monsterCenterX;
        int dy = kirbyCenterY - monsterCenterY;

        if (dx < 0) dx = -dx;
        if (dy < 0) dy = -dy;

        return dx <= 115 && dy <= 65;
    }

    RECT GetSparkMonsterAttackRect()
    {
        RECT rc;
        int attackW = 56;
        int attackH = 42;
        int attackY = y + h / 2 - attackH / 2;

        if (dir < 0)
        {
            rc.left = x - attackW + 12;
            rc.right = x + 12;
        }
        else
        {
            rc.left = x + w - 12;
            rc.right = rc.left + attackW;
        }

        rc.top = attackY;
        rc.bottom = attackY + attackH;

        return rc;
    }

    void StartSparkMonsterAttack()
    {
        int kirbyCenterX = kirbyX + kirbyW / 2;
        int monsterCenterX = x + w / 2;

        dir = (kirbyCenterX < monsterCenterX) ? -1 : 1;
        isAttack = true;
        sparkAttackFrameIndex = 0;
        sparkAttackFrameTick = 0;
        sparkAttackHitDone = false;
        sparkAttackCooldown = 80;
        isJumpAttack = false;
    }

    void UpdateSparkMonsterAttack()
    {
        if (monsterType != 4)
            return;

        if (sparkAttackCooldown > 0)
            sparkAttackCooldown--;

        if (!isAttack)
        {
            if (sparkAttackCooldown <= 0 && IsKirbyInSparkAttackRange())
                StartSparkMonsterAttack();
            return;
        }

        sparkAttackFrameTick++;

        if (!sparkAttackHitDone && sparkAttackFrameIndex >= 1 && sparkAttackFrameIndex <= 3)
        {
            RECT attackRc = GetSparkMonsterAttackRect();
            RECT kirbyRc = GetKirbyBodyRect();

            if (IsRectHit(attackRc, kirbyRc) && !isKirbyHit && kirbyHitCooldownTick <= 0)
            {
                StartKirbyHitEffect();
                sparkAttackHitDone = true;
            }
        }

        if (sparkAttackFrameTick >= 5)
        {
            sparkAttackFrameTick = 0;
            sparkAttackFrameIndex++;

            if (sparkAttackFrameIndex >= SPARK_MONSTER_ATTACK_FRAME_COUNT)
            {
                isAttack = false;
                sparkAttackFrameIndex = 0;
                sparkAttackHitDone = false;
            }
        }
    }
    bool HasSafeGroundBelowX(int testX)
    {
        int blockCount = 0;
        SolidBlock* blocks = GetCurrentSolidBlocks(&blockCount);

        for (int i = 0; i < blockCount; i++)
        {
            RECT block = blocks[i].rc;

            if (testX >= block.left + 6 && testX <= block.right - 6 && block.top >= y)
                return true;
        }

        return false;
    }

    bool HasGroundAhead(int nextX)
    {
        int footX;

        if (dir < 0)
            footX = nextX + MONSTER_HIT_LEFT - 6;
        else
            footX = nextX + w - MONSTER_HIT_RIGHT + 6;

        int footY = y + h - MONSTER_HIT_BOTTOM;

        RECT probe;
        probe.left = footX - 3;
        probe.right = footX + 3;
        probe.top = footY + 2;
        probe.bottom = footY + 34;

        return HitSolidBlock(probe, NULL);
    }

    void UpdateFlyingBombMonster()
    {
        TryBombDropAttack();

        int nextX = x + speed * dir;

        // 3스테이지 폭탄 몬스터도 구멍 위로 계속 넘어가지 않게,
        // 아래에 발판/땅이 없는 쪽으로 가려 하면 방향을 바꿈.
        if (g_currentStage == 3 && !HasSafeGroundBelowX(nextX + w / 2))
        {
            dir *= -1;
            nextX = x + speed * dir;
        }

        x = nextX;

        if (x < leftLimit)
        {
            x = leftLimit;
            dir = 1;
        }

        if (x > rightLimit)
        {
            x = rightLimit;
            dir = -1;
        }
    }

    void ApplyGravity()
    {
        if (!active)
            return;

        vy += gravity;

        if (vy > maxFallSpeed)
            vy = maxFallSpeed;

        int nextY = y + (int)vy;
        RECT nextHitY = GetMonsterHitBox(x, nextY, w, h);

        RECT hitBlock;
        onGround = false;

        if (!HitSolidBlock(nextHitY, &hitBlock))
        {
            y = nextY;
        }
        else
        {
            if (vy > 0)
            {
                y = hitBlock.top - (h - MONSTER_HIT_BOTTOM);
                vy = 0;
                onGround = true;

                if (isJumpAttack)
                {
                    isJumpAttack = false;
                    jumpAttackFrameIndex = 0;
                    jumpAttackCooldown = 45;
                }
            }
            else if (vy < 0)
            {
                y = hitBlock.bottom - MONSTER_HIT_TOP;
                vy = 0;
            }
        }

        RECT currentHit = GetMonsterHitBox(x, y, w, h);
        int groundY;

        if (vy >= 0 && FindGroundUnderHitBox(currentHit, &groundY))
        {
            y = groundY - (h - MONSTER_HIT_BOTTOM);
            vy = 0;
            onGround = true;

            if (isJumpAttack)
            {
                isJumpAttack = false;
                jumpAttackFrameIndex = 0;
                jumpAttackCooldown = 45;
            }
        }

        // 몬스터가 구멍 아래로 떨어지면 완전히 제거
        if (y > WORLD_H + 80)
        {
            active = false;
            isDeadEffect = false;
            isAttack = false;
            isJumpAttack = false;
            vy = 0.0f;
            return;
        }
    }

    bool IsKirbyNearForJumpAttack()
    {
        int kirbyCenterX = kirbyX + kirbyW / 2;
        int kirbyCenterY = kirbyY + kirbyH / 2;

        int monsterCenterX = x + w / 2;
        int monsterCenterY = y + h / 2;

        int dx = kirbyCenterX - monsterCenterX;
        int dy = kirbyCenterY - monsterCenterY;

        if (dx < 0)
            dx = -dx;

        if (dy < 0)
            dy = -dy;

        if (dx <= attackRange && dy <= 80)
            return true;

        return false;
    }

    void StartJumpAttack()
    {
        int kirbyCenterX = kirbyX + kirbyW / 2;
        int monsterCenterX = x + w / 2;

        if (kirbyCenterX < monsterCenterX)
        {
            dir = -1;
            jumpAttackVX = -4.0f;
        }
        else
        {
            dir = 1;
            jumpAttackVX = 4.0f;
        }

        isJumpAttack = true;
        isAttack = true;
        jumpAttackFrameIndex = 0;

        vy = -8.0f;
        onGround = false;
    }

    void UpdateJumpAttack()
    {
        if (!isJumpAttack)
            return;

        int nextX = x + (int)jumpAttackVX;
        RECT nextHitX = GetMonsterHitBox(nextX, y, w, h);
        RECT hitBlock;

        if (!HitSolidBlock(nextHitX, &hitBlock))
        {
            x = nextX;
        }
        else
        {
            if (jumpAttackVX < 0)
            {
                x = hitBlock.right - MONSTER_HIT_LEFT;
            }
            else
            {
                x = hitBlock.left - (w - MONSTER_HIT_RIGHT);
            }

            jumpAttackVX = 0.0f;
        }

        if (x < leftLimit)
        {
            x = leftLimit;
            jumpAttackVX = 0.0f;
        }

        if (x > rightLimit)
        {
            x = rightLimit;
            jumpAttackVX = 0.0f;
        }

        if (vy < 0)
        {
            jumpAttackFrameIndex = 0;
        }
        else
        {
            jumpAttackFrameIndex = 1;
        }

        ApplyGravity();
    }

    bool IsInAbsorbRange()
    {
        if (!active)
            return false;

        int kirbyCenterX = kirbyX + kirbyW / 2;
        int monsterCenterX = x + w / 2;

        int kirbyTop = kirbyY;
        int kirbyBottom = kirbyY + kirbyH;

        int monsterTop = y;
        int monsterBottom = y + h;

        bool overlapY = monsterBottom > kirbyTop && monsterTop < kirbyBottom;

        if (!overlapY)
            return false;

        if (kirbyFaceLeft)
        {
            int distance = kirbyX - (x + w);

            if (monsterCenterX <= kirbyCenterX && distance <= ABSORB_RANGE_X)
                return true;
        }
        else
        {
            int distance = x - (kirbyX + kirbyW);

            if (monsterCenterX >= kirbyCenterX && distance <= ABSORB_RANGE_X)
                return true;
        }

        return false;
    }

    bool IsReachedKirby()
    {
        if (!active)
            return false;

        int kirbyCenterX = kirbyX + kirbyW / 2;
        int kirbyCenterY = kirbyY + kirbyH / 2;

        int monsterCenterX = x + w / 2;
        int monsterCenterY = y + h / 2;

        int dx = monsterCenterX - kirbyCenterX;
        int dy = monsterCenterY - kirbyCenterY;

        if (dx < 0)
            dx = -dx;

        if (dy < 0)
            dy = -dy;

        const int EAT_DISTANCE_X = 14;
        const int EAT_DISTANCE_Y = 34; // Absorb fix: allow eating even when Y is slightly off.

        if (dx <= EAT_DISTANCE_X && dy <= EAT_DISTANCE_Y)
        {
            return true;
        }

        return false;
    }

    void ApplyAbsorb()
    {
        if (!active)
            return;

        if (!isAbsorb)
            return;

        if (!IsInAbsorbRange())
            return;

        if (kirbyFaceLeft)
        {
            x += ABSORB_PULL_SPEED;
        }
        else
        {
            x -= ABSORB_PULL_SPEED;
        }

        int kirbyCenterY = kirbyY + kirbyH / 2;
        int monsterCenterY = y + h / 2;
        int pullY = kirbyCenterY - monsterCenterY;

        if (pullY > 3) // Absorb fix: pull the monster vertically toward Kirby's mouth area.
            y += 3;
        else if (pullY < -3)
            y -= 3;

        if (IsReachedKirby())
        {
            active = false;
            isAttack = false;
            vy = 0.0f;

            // 몬스터를 먹으면 몬스터 속성 번호를 저장하고 커비가 커진 상태로 변경
            absorbedMonsterType = monsterType;
            isFireKirby = false;
            isFireTransform = false;
            isBombKirby = false;
            isBombTransform = false;
            isHammerKirby = false;
            isHammerAttack = false;
            hammerAttackFrameIndex = 0;
            hammerAttackTick = 0;
            hammerAttackHitDone = false;
            isHammerInvincibleSkill = false;
            hammerInvincibleSkillTick = 0;
            hammerInvincibleFrameIndex = 0;
            hammerInvincibleFrameTick = 0;
            isSparkKirby = false;
            isSparkAttack = false;
            sparkAttackFrameIndex = 0;
            sparkAttackTick = 0;
            sparkAttackHitDone = false;
            isSparkSpecialAttack = false;
            sparkSpecialAttackFrameIndex = 0;
            sparkSpecialAttackTick = 0;
            sparkSpecialAttackHitDone = false;
            kirbyAbilityType = 0;
            isPowerKirby = true;
            SetKirbyPowerSizeKeepBottom();

            // 커진 상태에서는 풍선 상태를 못 하게 즉시 해제
            isSpace = false;
            isSpaceRelease = false;
            spaceFrameIndex = 0;
            spaceStartFrameDone = false;
            moveUp = false;
            moveDown = false;

            // 먹은 뒤 1초 동안은 K를 눌러도 발사 안 됨
            powerWaitTick = 0;
            canPowerShoot = false;

            // 한 번 먹으면 한 번만 발사 가능
            powerShotUsed = false;

            isPowerAttack = false;
            powerAttackTick = 0;

            isAbsorb = false;
            absorbFrameIndex = 0;
            absorbFrontEffectIndex = 0;
            absorbFrontEffectTick = 0;
        }
    }

    void Update()
    {
        if (!active)
        {
            UpdateDeadEffect();
            return;
        }

        if (jumpAttackCooldown > 0)
            jumpAttackCooldown--;

        if (monsterType == 1)
        {
            TryRangedAttack();
        }

        if (monsterType == 3)
        {
            UpdateHammerMonsterAttack();
        }

        if (monsterType == 4)
        {
            UpdateSparkMonsterAttack();
        }

        if (monsterType == 2)
        {
            // 빨아들이기 중이어도 모든 몬스터가 멈추면 안 됨.
            // 커비 흡수 범위 안에 들어온 폭탄 몬스터만 빨려가고,
            // 범위 밖의 폭탄 몬스터는 평소처럼 하늘에서 움직이게 함.
            if (isAbsorb && IsInAbsorbRange())
            {
                ApplyAbsorb();
            }
            else
            {
                UpdateFlyingBombMonster();
            }
            return;
        }

        if (isAbsorb && IsInAbsorbRange())
        {
            // 빨아들이기 범위 안에 있는 몬스터만 흡수 처리.
            // 범위 밖 몬스터는 아래 일반 이동 코드로 계속 움직임.
            isJumpAttack = false;
            isAttack = false;
            ApplyAbsorb();
            ApplyGravity();
            return;
        }

        if ((monsterType == 3 || monsterType == 4) && isAttack)
        {
            ApplyGravity();
            return;
        }

        if (isJumpAttack)
        {
            UpdateJumpAttack();
            return;
        }

        if (monsterType != 1 && monsterType != 3 && monsterType != 4 && onGround && jumpAttackCooldown <= 0 && IsKirbyNearForJumpAttack())
        {
            StartJumpAttack();
            return;
        }

        int nextX = x + speed * dir;

        // 땅 몬스터는 발판 끝이나 구멍 쪽으로 계속 걸어가지 않고 되돌아감
        if (onGround && !HasGroundAhead(nextX))
        {
            dir *= -1;
            nextX = x + speed * dir;
        }

        RECT nextHitX = GetMonsterHitBox(nextX, y, w, h);
        RECT hitBlock;

        if (!HitSolidBlock(nextHitX, &hitBlock))
        {
            x = nextX;
        }
        else
        {
            if (dir < 0)
            {
                x = hitBlock.right - MONSTER_HIT_LEFT;
                dir = 1;
            }
            else
            {
                x = hitBlock.left - (w - MONSTER_HIT_RIGHT);
                dir = -1;
            }
        }

        if (x < leftLimit)
        {
            x = leftLimit;
            dir = 1;
        }

        if (x > rightLimit)
        {
            x = rightLimit;
            dir = -1;
        }

        ApplyGravity();
    }

    void NextFrame()
    {
        if (!active)
            return;

        if (isJumpAttack)
        {
            if (vy < 0)
                jumpAttackFrameIndex = 0;
            else
                jumpAttackFrameIndex = 1;

            return;
        }

        if ((monsterType == 3 || monsterType == 4) && isAttack)
            return;

        frameIndex++;

        if (monsterType == 3)
        {
            if (frameIndex >= HAMMER_MONSTER_WALK_FRAME_COUNT)
                frameIndex = 0;
            return;
        }

        if (monsterType == 4)
        {
            if (frameIndex >= SPARK_MONSTER_WALK_FRAME_COUNT)
                frameIndex = 0;
            return;
        }

        if (frameIndex >= frameCount)
        {
            frameIndex = 0;
        }
    }

    void Draw(Graphics& graphics);
};

extern const int MONSTER_COUNT;
extern Monster g_monsters[];

void CheckKirbyHitByMonsters();
