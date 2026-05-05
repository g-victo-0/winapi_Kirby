#include "Game.h"
#include "resource.h"
#include <mmsystem.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "winmm.lib")

using namespace Gdiplus;


HINSTANCE g_hInst;
LPCTSTR lpszClass = L"WindowClass";
LPCTSTR lpszWindowName = L"GDI+ Kirby Animation";

ULONG_PTR g_gdiplusToken;

// 72번: 오프닝 화면
Image* g_openingFrame = NULL;

// 73~81번: 오프닝 뒤에 자동으로 넘어가는 스토리 화면
Image* g_storyFrames[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
int g_storyFrameIndex = 0;
int g_storyTick = 0;
const int STORY_FRAME_COUNT = 8;
const int STORY_FRAME_DURATION = 30; // 16ms 타이머 기준 약 1초

// 처음 실행하면 72번 오프닝, SPACE를 누르면 73~81번 스토리 진행 후 1스테이지 시작
bool g_isOpening = true;
bool g_isStory = false;

// 1번: 커비 가만히 있는 자세
Image* g_idleFrame = NULL;

// 2~5번: 커비 움직일 때 프레임
Image* g_walkFrames[4] = { NULL, NULL, NULL, NULL };

// 6~8번: 커비 풍선 프레임
Image* g_spaceFrames[3] = { NULL, NULL, NULL };

// 9~11번: 커비 흡수 프레임
Image* g_absorbFrames[3] = { NULL, NULL, NULL };

// 12번: 커비 앉기 프레임
Image* g_crouchFrame = NULL;

// 13번: SPACE 해제 앞쪽 이펙트
Image* g_spaceReleaseEffect = NULL;

// 14~15번: 빨아들이기 앞쪽 이펙트
Image* g_absorbFrontEffectFrames[2] = { NULL, NULL };

// 16~17번: 몬스터 점프 공격 프레임
Image* g_monsterJumpFrames[2] = { NULL, NULL };

// 18~21번: 몬스터 움직일 때 프레임
Image* g_monsterFrames[4] = { NULL, NULL, NULL, NULL };

// 35번: 일반 몬스터가 죽을 때 표시할 프레임
Image* g_monsterDeadFrame = NULL;

// 71번: 불속성 몬스터가 죽을 때 표시할 프레임
Image* g_fireMonsterDeadFrame = NULL;

// 22번: 맵 1
Image* g_background = NULL;

// 23번: 오른쪽에 이어지는 맵 2
Image* g_background2 = NULL;

// 24번: 몬스터를 먹은 뒤 커진 커비 가만히 있는 프레임
Image* g_powerIdleFrame = NULL;

// 25, 26, 30, 31, 32번: 커진 커비 걷기 프레임
Image* g_powerWalkFrames[5] = { NULL, NULL, NULL, NULL, NULL };

// 33번: 커진 커비 공격 자세 프레임
Image* g_powerAttackFrame = NULL;

// 34번: 커진 커비가 발사하는 투사체 프레임
Image* g_powerProjectileFrame = NULL;

// 36번: 커진 커비가 L을 눌렀을 때 소화시키는 프레임
Image* g_powerDigestFrame = NULL;

// 37번: 일반 커비가 몬스터에게 공격받았을 때 잠깐 보여줄 피격 프레임
Image* g_kirbyHitFrame = NULL;

// 69번: 폭탄 커비가 공격받았을 때 잠깐 보여줄 피격 프레임
Image* g_bombKirbyHitFrame = NULL;

// 70번: 불 커비가 공격받았을 때 잠깐 보여줄 피격 프레임
Image* g_fireKirbyHitFrame = NULL;

// 38번: 빈 체력바 프레임
Image* g_hpBarFrame = NULL;

// 39번: 불 속성으로 변신하는 프레임
Image* g_fireTransformFrame = NULL;

// 40번: 불 속성 커비 기본 프레임
Image* g_fireIdleFrame = NULL;

// 41~44번: 불 속성 커비 걷기 프레임
Image* g_fireWalkFrames[4] = { NULL, NULL, NULL, NULL };

// 45번: 불 속성 커비가 K/I 공격할 때 잠깐 보여줄 공격 자세
Image* g_fireAttackKirbyFrame = NULL;

// 46번: 불 속성 커비 K 공격 앞쪽 불꽃
Image* g_fireBreathFrame = NULL;

// 47번: I 키로 발사하는 화염구
Image* g_fireBallFrame = NULL;

// 48번: 불 속성 몬스터 기본 프레임
Image* g_fireMonsterFrame = NULL;

// 49번: 불 속성 몬스터 원거리 공격 / 탄 프레임
Image* g_fireMonsterAttackFrame = NULL;

// 51번: 불 속성 커비 풍선 시작 프레임
Image* g_fireBalloonStartFrame = NULL;

// 52번: 불 속성 커비 떠다니는 프레임
Image* g_fireBalloonFrames[2] = { NULL, NULL };

// 53번: 불 속성 커비 앉기 프레임
Image* g_fireCrouchFrame = NULL;

// 54번: 폭탄 커비 기본 프레임
Image* g_bombIdleFrame = NULL;

// 55~58번: 폭탄 커비 걷기 프레임
Image* g_bombWalkFrames[4] = { NULL, NULL, NULL, NULL };

// 59번: 폭탄 커비 풍선 시작/해제 프레임
Image* g_bombBalloonStartFrame = NULL;

// 60~61번: 폭탄 커비 풍선 유지 프레임
Image* g_bombBalloonFrames[2] = { NULL, NULL };

// 62~64번: 폭탄 커비 K 공격 프레임
Image* g_bombAttackFrames[3] = { NULL, NULL, NULL };

// 65번: 폭탄 프레임
Image* g_bombProjectileFrame = NULL;

// 66번: 하늘 폭탄 몬스터 기본 프레임
Image* g_bombMonsterFrame = NULL;

// 67번: 하늘 폭탄 몬스터 죽는 프레임
Image* g_bombMonsterDeadFrame = NULL;

// 68번: 폭탄 속성으로 변신하는 프레임
Image* g_bombTransformFrame = NULL;


// 27~29번: SHIFT 달리기 바람 이펙트
Image* g_dashWindFrames[3] = { NULL, NULL, NULL };

int walkFrameIndex = 0;
int walkFrameCount = 4;

int powerWalkFrameIndex = 0;
int powerWalkFrameCount = 5;

// 커비 기본 크기 / 몬스터를 먹은 뒤 커진 크기
// 충돌체 배열 g_solidBlocks[]는 건드리지 않음
const int NORMAL_KIRBY_W = 48;
const int NORMAL_KIRBY_H = 48;
const int POWER_KIRBY_W = 64;
const int POWER_KIRBY_H = 64;

// 몬스터를 먹은 뒤 커진 커비 상태
bool isPowerKirby = false;

// 몬스터를 먹은 뒤 1초 동안 K 발사 금지
int powerWaitTick = 0;
const int POWER_WAIT_TICK_MAX = 60; // 16ms 타이머 기준 약 1초
bool canPowerShoot = false;

// 한 번 먹으면 한 번만 발사 가능
bool powerShotUsed = false;

// 커진 커비 공격 상태
bool isPowerAttack = false;
int powerAttackTick = 0;
const int POWER_ATTACK_DURATION = 12;

// L을 눌러 소화시키는 상태
bool isPowerDigest = false;
int powerDigestTick = 0;
const int POWER_DIGEST_DURATION = 10; // 16ms 타이머 기준 약 0.5초

// 소화가 끝난 뒤 어떤 속성이 될지 저장
int digestResultType = 0;

// 불 속성 커비 상태. 0 = 일반, 1 = 불 속성
int kirbyAbilityType = 0;
bool isFireKirby = false;

// 39번 변신 프레임 표시 상태
bool isFireTransform = false;
int fireTransformTick = 0;
const int FIRE_TRANSFORM_DURATION = 30;

// 불 속성 걷기 프레임
int fireWalkFrameIndex = 0;
const int FIRE_WALK_FRAME_COUNT = 4;

// K/I 공격 시 45번 불 속성 커비 공격 자세
bool isFireAttackPose = false;
int fireAttackPoseTick = 0;
const int FIRE_ATTACK_POSE_DURATION = 12;

// K 불 뿜기 공격
bool isFireBreath = false;
int fireBreathTick = 0;
const int FIRE_BREATH_DURATION = 12;

// 불 속성 커비 풍선 프레임
int fireBalloonFrameIndex = 0;
bool fireBalloonStartFrameDone = false;

// 폭탄 커비 상태. 2 = 폭탄 속성
bool isBombKirby = false;

// 68번 폭탄 속성 변신 프레임 표시 상태
bool isBombTransform = false;
int bombTransformTick = 0;
const int BOMB_TRANSFORM_DURATION = 30;

// 폭탄 커비 걷기 프레임
int bombWalkFrameIndex = 0;
const int BOMB_WALK_FRAME_COUNT = 4;

// 폭탄 커비 풍선 프레임
int bombBalloonFrameIndex = 0;
bool bombBalloonStartFrameDone = false;

// 폭탄 커비 공격 상태. 62 -> 63 -> 64를 한 번만 재생
bool isBombAttack = false;
int bombAttackFrameIndex = 0;
int bombAttackTick = 0;
const int BOMB_ATTACK_FRAME_DURATION = 8;
bool bombAttackBombSpawned = false;

// 65번 폭탄 투사체와 68번 폭발
const int BOMB_OBJECT_MAX = 12;
struct BombObject
{
    bool active;
    bool fromEnemy;
    int x;
    int y;
    int w;
    int h;
    float vx;
    float vy;
};
BombObject g_bombs[BOMB_OBJECT_MAX];

const int BOMB_EXPLOSION_MAX = 12;
struct BombExplosion
{
    bool active;
    int x;
    int y;
    int w;
    int h;
    int tick;
};
BombExplosion g_bombExplosions[BOMB_EXPLOSION_MAX];
const int BOMB_EXPLOSION_DURATION = 18;

// I 화염구 공격
bool isFireBallActive = false;
int fireBallX = 0;
int fireBallPrevX = 0;
int fireBallY = 0;
int fireBallW = 40;
int fireBallH = 28;
int fireBallDir = 1;
int fireBallSpeed = 12;

// 마지막으로 빨아먹은 몬스터 속성 번호. 0 = 일반, 1 = 불 속성
int absorbedMonsterType = 0;

// 몬스터 원거리 공격 탄
const int ENEMY_FIREBALL_MAX = 5;
struct EnemyFireBall
{
    bool active;
    int x;
    int y;
    int prevX;
    int w;
    int h;
    int dir;
    int speed;
};
EnemyFireBall g_enemyFireBalls[ENEMY_FIREBALL_MAX];

// 몬스터에게 맞았을 때 37번 프레임 표시 상태
bool isKirbyHit = false;
int kirbyHitTick = 0;
const int KIRBY_HIT_DURATION = 18; // 16ms 기준 약 0.3초

// 계속 닿아 있을 때 37번 프레임이 매 프레임 다시 시작되는 것을 막는 대기시간
int kirbyHitCooldownTick = 0;
const int KIRBY_HIT_COOLDOWN = 60; // 약 1초

// 커비 체력. 몬스터에게 맞을 때마다 실제 HP는 15% 감소하고,
// 화면에 보이는 체력은 조금씩 줄어들도록 별도로 관리함
int kirbyMaxHP = 100;
int kirbyHP = 100;          // 실제 체력 목표값
float kirbyDisplayHP = 100.0f; // 화면에 부드럽게 표시되는 체력
const int KIRBY_DAMAGE = 15;
const float HP_ANIM_SPEED = 0.5f; // 16ms 타이머 기준. 작을수록 천천히 줄어듦
bool isGameOver = false;
int gameOverTick = 0;
const int GAME_OVER_DELAY = 30; // HP 표시가 0이 된 뒤 약 0.5초 후 종료

// 34번 투사체 상태
bool isPowerProjectileActive = false;
int powerProjectileX = 0;
int powerProjectilePrevX = 0;
int powerProjectileY = 0;
int powerProjectileW = 40;
int powerProjectileH = 32;
int powerProjectileSpeed = 10;
int powerProjectileDir = 1;

// 변신 상태에서 O를 누르면 뒤로 튀어나가는 능력별 상태
// 34번 프레임을 사용하고, 다시 빨아들이면 이전 속성으로 돌아감
bool isAbilityStarActive = false;
int abilityStarType = 0; // 1 = 불속성, 2 = 폭탄속성
float abilityStarX = 0.0f;
float abilityStarY = 0.0f;
float abilityStarVX = 0.0f;
float abilityStarVY = 0.0f;
int abilityStarW = 51;
int abilityStarH = 48;
int abilityStarLifeTick = 0;
const int ABILITY_STAR_LIFE_MAX = 180; // 16ms 타이머 기준 약 3초
const float ABILITY_STAR_GRAVITY = 0.45f;
const float ABILITY_STAR_BOUNCE = 0.72f;
const int ABILITY_STAR_ABSORB_SPEED = 6;

int spaceFrameIndex = 0;
int spaceFrameCount = 3;
bool spaceStartFrameDone = false;

int absorbFrameIndex = 0;
int absorbFrameCount = 3;

int absorbFrontEffectIndex = 0;
int absorbFrontEffectTick = 0;
const int ABSORB_FRONT_EFFECT_FIRST_DURATION = 60;

int monsterFrameCount = 4;
int monsterJumpFrameCount = 2;
const int MONSTER_DEAD_DURATION = 30; // 16ms 타이머 기준 약 0.5초 동안 35번 프레임 표시

// 커비 위치/크기
int kirbyX = 50;
int kirbyY = 470;
int kirbyW = 48;
int kirbyH = 48;

// 이동 상태
bool moveLeft = false;
bool moveRight = false;
bool moveUp = false;
bool moveDown = false;

// 속도
int speed = 4;
int balloonSpeed = 4;

// SHIFT 달리기 상태
bool isDash = false;
int dashSpeed = 6;
int dashFrameIndex = 0;
int dashFrameTick = 0;
int dashFrameCount = 3;

// 점프 / 중력
bool isOnGround = false;
bool jumpKeyDown = false;
float kirbyVY = 0.0f;

float gravity = 0.85f;
float jumpPower = -10.0f;
float maxFallSpeed = 14.0f;

// 상태
bool isSpace = false;
bool isAbsorb = false;
bool isCrouch = false;

// 풍선 상태는 최대 2초까지만 유지
int balloonTick = 0;
const int BALLOON_DURATION_TICK = 125; // 16ms 타이머 기준 약 2초
bool spaceKeyHeld = false; // SPACE를 계속 누르고 있어도 2초 뒤 자동 재시작되지 않게 막음

// 앉기 프레임 위치 조정
int crouchDrawOffsetY = 4;

// SPACE를 뗐을 때 9번 + 13번 잠깐 표시
bool isSpaceRelease = false;
int spaceReleaseTick = 0;
const int SPACE_RELEASE_DURATION = 12;

// 커비가 바라보는 방향
// false = 오른쪽, true = 왼쪽
bool kirbyFaceLeft = false;

// 흡수 범위 / 흡수 속도
const int ABSORB_RANGE_X = 50;
const int ABSORB_PULL_SPEED = 4;

// 마우스 드래그
bool isDragging = false;
int dragOffsetX = 0;
int dragOffsetY = 0;

// 커비 히트박스 보정
const int KIRBY_HIT_LEFT = 25;
const int KIRBY_HIT_RIGHT = 25;
const int KIRBY_HIT_TOP = 20;
const int KIRBY_HIT_BOTTOM = 14;

// 몬스터 히트박스 보정
const int MONSTER_HIT_LEFT = 20;
const int MONSTER_HIT_RIGHT = 20;
const int MONSTER_HIT_TOP = 20;
const int MONSTER_HIT_BOTTOM = 12;

struct SolidBlock
{
    RECT rc;
    LPCWSTR name;
};

SolidBlock g_solidBlocks[] =
{
    // =========================
    // PNG22 첫 번째 맵 충돌체
    // 예전에 쓰던 충돌체 다시 유지
    // =========================
    { { 0,   565, 260, 613 }, L"MAP1_GROUND_1" },
    { { 230, 545, 330, 620 }, L"MAP1_BLOCK_1" },
    { { 305, 565, 447, 616 }, L"MAP1_GROUND_2" },
    { { 430, 545, 485, 616 }, L"MAP1_PILLAR" },
    { { 475, 565, 550, 631 }, L"MAP1_GROUND_3" },
    { { 535, 545, 917, 619 }, L"MAP1_MONSTER_AREA" },

    // =========================
    // PNG23 두 번째 맵 충돌체
    // PNG23은 월드 x = 900부터 시작하므로, 화면에서 보이는 x좌표에 +900을 해줌
    // 사진 보고 대충 맞춘 값이라 F1 눌러서 좌표 확인하면서 조금씩 조절하면 됨
    // =========================

    // 왼쪽 아래 긴 땅
    { { 900, 545, 1500, 619 }, L"MAP2_LEFT_GROUND" },

    // 가운데 아래 바위 기둥
    { { 1455, 453, 1600, 619 }, L"MAP2_SMALL_ROCK" },

    // 오른쪽 큰 절벽/벽
    { { 1530, 135, 1800, 619 }, L"MAP2_BIG_CLIFF" },

    // 오른쪽 큰 절벽 위에서 왼쪽으로 튀어나온 발판 부분
    { { 1460, 135, 1800, 210 }, L"MAP2_TOP_LEDGE" },

    // 나무 발판들
    { { 1052, 223, 1245, 250 }, L"MAP2_WOOD_1" },
    { { 1240, 135, 1390, 160 }, L"MAP2_WOOD_2" },
    { { 1285, 281, 1435, 305 }, L"MAP2_WOOD_3" },
    { { 1285, 369, 1435, 392 }, L"MAP2_WOOD_4" },
    { { 1240, 453, 1500, 482 }, L"MAP2_WOOD_5" }
};

int g_solidBlockCount = sizeof(g_solidBlocks) / sizeof(g_solidBlocks[0]);

// 카메라 / 월드 크기
// 배경 PNG22가 0~899, PNG23이 900~1799에 붙는 구조
const int BG_PART_W = 900;
const int BG_PART_H = 650;
const int WORLD_W = BG_PART_W * 2;
const int WORLD_H = BG_PART_H;

int cameraX = 0;

// F1 디버그 모드
bool g_debugMode = false;
int g_mouseScreenX = 0;
int g_mouseScreenY = 0;
int g_mouseWorldX = 0;
int g_mouseWorldY = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

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
    for (int i = 0; i < g_solidBlockCount; i++)
    {
        if (IsRectHit(rc, g_solidBlocks[i].rc))
        {
            if (hitBlock != NULL)
            {
                *hitBlock = g_solidBlocks[i].rc;
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
    for (int i = 0; i < g_solidBlockCount; i++)
    {
        // 풍선 상태에서는 나무발판만 통과 가능하게 함
        if (IsWoodPlatformBlock(g_solidBlocks[i].name))
            continue;

        if (IsRectHit(rc, g_solidBlocks[i].rc))
        {
            if (hitBlock != NULL)
            {
                *hitBlock = g_solidBlocks[i].rc;
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

    for (int i = 0; i < g_solidBlockCount; i++)
    {
        RECT block = g_solidBlocks[i].rc;

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

    if (kirbyX + kirbyW > WORLD_W)
        kirbyX = WORLD_W - kirbyW;

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

RECT GetPowerProjectileSweepRect();
void CheckPowerProjectileHitMonsters();
void EjectAbilityStar();
void UpdateAbilityStar();
void DrawAbilityStar(Graphics& graphics);
void RestoreAbilityFromStar();
void StartFireKirbyTransform();
void SpawnFireBall();
void StartFireBreath();
void UpdateFireKirbyStates();
void CheckFireAttacksHitMonsters();
void SpawnEnemyFireBall(int x, int y, int dir);
void UpdateEnemyFireBalls();
void CheckEnemyFireBallsHitKirby();
void StartKirbyHitEffect();
RECT GetKirbyBodyRect();
void StartBombKirbyTransform();
void StartBombAttack();
void UpdateBombAttack();
void SpawnBombObject(int x, int y, float vx, float vy, bool fromEnemy);
void UpdateBombObjects();
void DrawBombObjects(Graphics& graphics);
void DrawBombExplosions(Graphics& graphics);
void SpawnBombExplosion(int x, int y);
void CheckBombExplosionHitKirby(RECT explosionRc);
void CheckBombHitMonsters(RECT bombRc, bool fromEnemy);
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

        if (fireBallX + fireBallW < 0 || fireBallX > WORLD_W)
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
            g_enemyFireBalls[i].x > WORLD_W)
        {
            g_enemyFireBalls[i].active = false;
        }
    }
}

void CheckEnemyFireBallsHitKirby()
{
    if (isAbsorb)
        return;

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

void StartKirbyHitEffect()
{
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


void UpdatePowerProjectile()
{
    if (!isPowerProjectileActive)
        return;

    // 이전 위치를 저장해서 빠르게 지나가도 충돌을 놓치지 않게 함
    powerProjectilePrevX = powerProjectileX;
    powerProjectileX += powerProjectileSpeed * powerProjectileDir;

    // 발사체와 몬스터 충돌 검사
    CheckPowerProjectileHitMonsters();

    if (!isPowerProjectileActive)
        return;

    // 화면 밖이 아니라 전체 월드 밖으로 나가면 제거
    if (powerProjectileX + powerProjectileW < 0 || powerProjectileX > WORLD_W)
    {
        isPowerProjectileActive = false;
    }
}

void UpdateKirbyPosition(HWND hWnd)
{
    if (isAbsorb)
    {
        return;
    }

    RECT rt;
    GetClientRect(hWnd, &rt);

    if (isSpace)
    {
        int curBalloonSpeed = isDash ? dashSpeed : balloonSpeed;

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

        if (kirbyX + kirbyW > WORLD_W)
            kirbyX = WORLD_W - kirbyW;

        if (kirbyY < 0)
            kirbyY = 0;

        if (kirbyY + kirbyH > WORLD_H)
            kirbyY = WORLD_H - kirbyH;

        return;
    }

    int nextX = kirbyX;

    if (!isCrouch)
    {
        int curSpeed = isDash ? dashSpeed : speed;

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

    if (kirbyX + kirbyW > WORLD_W)
        kirbyX = WORLD_W - kirbyW;

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

    if (kirbyY + kirbyH > rt.bottom)
    {
        kirbyY = rt.bottom - kirbyH;
        kirbyVY = 0;
        isOnGround = true;
    }
}

bool IsInsideKirby(int mouseX, int mouseY)
{
    return mouseX >= kirbyX &&
        mouseX <= kirbyX + kirbyW &&
        mouseY >= kirbyY &&
        mouseY <= kirbyY + kirbyH;
}

void UpdateCamera(HWND hWnd)
{
    RECT rt;
    GetClientRect(hWnd, &rt);

    int screenW = rt.right - rt.left;

    // 커비 중심이 화면 가운데쯤 오도록 카메라 이동
    cameraX = kirbyX + kirbyW / 2 - screenW / 2;

    if (cameraX < 0)
        cameraX = 0;

    if (cameraX > WORLD_W - screenW)
        cameraX = WORLD_W - screenW;
}

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

    // 몬스터 속성 번호. 0 = 일반, 1 = 불 속성
    int monsterType;

    // 불 속성 몬스터 원거리 공격
    int rangedAttackCooldown;
    int rangedAttackFrameTick;

    // 폭탄 몬스터 투하 공격
    int bombDropCooldown;

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
        w = 32;
        h = 32;

        speed = 2;
        dir = startDir;

        frameIndex = 0;
        frameCount = 4;

        leftLimit = patrolLeft;
        rightLimit = patrolRight - w;

        attackRange = 140;
        isAttack = false;

        vy = 0.0f;
        onGround = false;

        active = true;
        monsterType = type;
        rangedAttackCooldown = 90;
        rangedAttackFrameTick = 0;
        bombDropCooldown = 100;
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

    void UpdateFlyingBombMonster()
    {
        TryBombDropAttack();

        x += speed * dir;

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

        const int EAT_DISTANCE_X = 10;
        const int EAT_DISTANCE_Y = 18;

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
            ApplyAbsorb();
            ApplyGravity();
            return;
        }

        if (isJumpAttack)
        {
            UpdateJumpAttack();
            return;
        }

        if (monsterType != 1 && onGround && jumpAttackCooldown <= 0 && IsKirbyNearForJumpAttack())
        {
            StartJumpAttack();
            return;
        }

        int nextX = x + speed * dir;
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

        frameIndex++;

        if (frameIndex >= frameCount)
        {
            frameIndex = 0;
        }
    }

    void Draw(Graphics& graphics);
};

const int MONSTER_COUNT = 5;
Monster g_monsters[MONSTER_COUNT];

void InitMonsters()
{
    // 기존 몬스터 1마리
    g_monsters[0].Init(600, 470, 546, 914, -1);

    // 추가 몬스터 2마리 - 18~21번 걷기 프레임, 16~17번 점프 공격 프레임 그대로 사용
    g_monsters[1].Init(1080, 470, 930, 1450, -1);
    g_monsters[2].Init(1340, 380, 1240, 1500, 1);

    // 불 속성 몬스터 1번. 48번 기본 프레임, 49번 원거리 공격 프레임 사용
    g_monsters[3].Init(1660, 100, 1530, 1790, -1, 1);

    // 폭탄 몬스터 2번. 66번 프레임으로 하늘에서 좌우로 이동하고 폭탄을 떨어뜨림
    // 불 속성 몬스터 위치(1660 근처)와 겹치지 않도록 왼쪽 구간에 배치
    g_monsters[4].Init(980, 80, 920, 1240, 1, 2);
}

void StartBombKirbyTransform();
void StartBombAttack();
void UpdateBombAttack();
void SpawnBombObject(int x, int y, float vx, float vy, bool fromEnemy);
void UpdateBombObjects();
void DrawBombObjects(Graphics& graphics);
void DrawBombExplosions(Graphics& graphics);
void SpawnBombExplosion(int x, int y);
void CheckBombExplosionHitKirby(RECT explosionRc);
void CheckBombHitMonsters(RECT bombRc, bool fromEnemy);

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

    if (abilityStarX + abilityStarW > WORLD_W)
    {
        abilityStarX = (float)(WORLD_W - abilityStarW);
        abilityStarVX = -abilityStarVX;
    }
}

void DrawAbilityStar(Graphics& graphics)
{
    if (!isAbilityStarActive)
        return;

    if (g_powerProjectileFrame == NULL)
        return;

    graphics.DrawImage(
        g_powerProjectileFrame,
        (int)abilityStarX,
        (int)abilityStarY,
        abilityStarW,
        abilityStarH
    );
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

void SpawnBombObject(int x, int y, float vx, float vy, bool fromEnemy)
{
    for (int i = 0; i < BOMB_OBJECT_MAX; i++)
    {
        if (!g_bombs[i].active)
        {
            g_bombs[i].active = true;
            g_bombs[i].fromEnemy = fromEnemy;
            g_bombs[i].x = x;
            g_bombs[i].y = y;
            g_bombs[i].w = 34;
            g_bombs[i].h = 34;
            g_bombs[i].vx = vx;
            g_bombs[i].vy = vy;
            return;
        }
    }
}

void StartBombAttack()
{
    if (!isBombKirby)
        return;

    if (isBombAttack)
        return;

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
        int bombX = kirbyFaceLeft ? kirbyX - 28 : kirbyX + kirbyW - 6;
        int bombY = kirbyY + kirbyH / 2 - 22;
        SpawnBombObject(bombX, bombY, 6.5f * dir, -8.5f, false);
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
    if (isAbsorb || isKirbyHit || kirbyHitCooldownTick > 0)
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
            SpawnBombExplosion(g_bombs[i].x + g_bombs[i].w / 2, g_bombs[i].y + g_bombs[i].h);
            g_bombs[i].active = false;
            continue;
        }

        if (g_bombs[i].x + g_bombs[i].w < 0 || g_bombs[i].x > WORLD_W || g_bombs[i].y > WORLD_H + 100)
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

        graphics.DrawImage(g_bombProjectileFrame, g_bombs[i].x, g_bombs[i].y, g_bombs[i].w, g_bombs[i].h);
    }
}

void DrawBombExplosions(Graphics& graphics)
{
    // 68번 폭발 프레임은 사용하지 않음.
    // 폭탄이 바닥에 닿으면 내부 판정만 처리하고 화면에는 폭발 이미지를 그리지 않음.
    return;
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

void CheckKirbyHitByMonsters()
{
    // 빨아들이는 중에는 몬스터를 먹는 판정과 겹치지 않게 피격 판정은 잠깐 끔
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


void DrawImageFlipX(Graphics& graphics, Image* image, int x, int y, int w, int h)
{
    if (image == NULL)
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
        graphics.DrawImage(image, kirbyX, kirbyY, kirbyW, kirbyH);
    }
}

void Monster::Draw(Graphics& graphics)
{
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
            graphics.DrawImage(deadFrame, x, y, w, h);
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
            graphics.DrawImage(jumpFrame, x, y, w, h);
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
        graphics.DrawImage(frame, x, y, w, h);
    }
}

void DrawDashWind(Graphics& graphics)
{
    if (!isDash)
        return;

    if (!IsMoving())
        return;

    if (isAbsorb)
        return;

    if (isCrouch)
        return;

    if (g_dashWindFrames[dashFrameIndex] == NULL)
        return;

    int windW = kirbyW / 2;
    int windH = kirbyH / 2;
    int windY = kirbyY + 20;

    if (kirbyFaceLeft)
    {
        int windX = kirbyX + kirbyW - 10;

        DrawImageFlipX(
            graphics,
            g_dashWindFrames[dashFrameIndex],
            windX,
            windY,
            windW,
            windH
        );
    }
    else
    {
        int windX = kirbyX - windW + 10;

        graphics.DrawImage(
            g_dashWindFrames[dashFrameIndex],
            windX,
            windY,
            windW,
            windH
        );
    }
}

void DrawSpaceReleaseEffect(Graphics& graphics)
{
    if (g_spaceReleaseEffect == NULL)
        return;

    int effectW = kirbyW;
    int effectH = kirbyH;
    int effectY = kirbyY;

    if (kirbyFaceLeft)
    {
        int effectX = kirbyX - effectW + 4;

        DrawImageFlipX(
            graphics,
            g_spaceReleaseEffect,
            effectX,
            effectY,
            effectW,
            effectH
        );
    }
    else
    {
        int effectX = kirbyX + kirbyW - 4;

        graphics.DrawImage(
            g_spaceReleaseEffect,
            effectX,
            effectY,
            effectW,
            effectH
        );
    }
}

void DrawAbsorbFrontEffect(Graphics& graphics)
{
    Image* effect = g_absorbFrontEffectFrames[absorbFrontEffectIndex];

    if (effect == NULL)
        return;

    int effectW;
    int effectH;
    int effectY;

    if (absorbFrontEffectIndex == 0)
    {
        effectW = kirbyW * 3 / 4;
        effectH = kirbyH * 3 / 4;
        effectY = kirbyY - 4;
    }
    else
    {
        effectW = ABSORB_RANGE_X;
        effectH = kirbyH;
        effectY = kirbyY - 6;
    }

    if (kirbyFaceLeft)
    {
        int effectX = kirbyX - effectW + 2;

        DrawImageFlipX(
            graphics,
            effect,
            effectX,
            effectY,
            effectW,
            effectH
        );
    }
    else
    {
        int effectX = kirbyX + kirbyW - 2;

        graphics.DrawImage(
            effect,
            effectX,
            effectY,
            effectW,
            effectH
        );
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
        graphics.DrawImage(
            g_powerProjectileFrame,
            powerProjectileX,
            powerProjectileY,
            powerProjectileW,
            powerProjectileH
        );
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
        graphics.DrawImage(g_fireBreathFrame, rc.left, rc.top, w, h);
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
        graphics.DrawImage(g_fireBallFrame, fireBallX, fireBallY, fireBallW, fireBallH);
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
            graphics.DrawImage(
                g_fireMonsterAttackFrame,
                g_enemyFireBalls[i].x,
                g_enemyFireBalls[i].y,
                g_enemyFireBalls[i].w,
                g_enemyFireBalls[i].h
            );
        }
    }
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
    g_storyFrames[7] = LoadPNGFromResource(g_hInst, IDB_PNG80);

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
    g_bombMonsterFrame = LoadPNGFromResource(g_hInst, IDB_PNG66);
    g_bombMonsterDeadFrame = LoadPNGFromResource(g_hInst, IDB_PNG67);
    g_bombTransformFrame = LoadPNGFromResource(g_hInst, IDB_PNG68);

    g_dashWindFrames[0] = LoadPNGFromResource(g_hInst, IDB_PNG27);
    g_dashWindFrames[1] = LoadPNGFromResource(g_hInst, IDB_PNG28);
    g_dashWindFrames[2] = LoadPNGFromResource(g_hInst, IDB_PNG29);

    if (g_openingFrame == NULL)
        MessageBox(hWnd, L"IDB_PNG72 오프닝 화면 PNG 로드 실패", L"로드 실패", MB_OK);

    for (int i = 0; i < STORY_FRAME_COUNT; i++)
    {
        if (g_storyFrames[i] == NULL)
        {
            MessageBox(hWnd, L"스토리 PNG 로드 실패: IDB_PNG73 ~ IDB_PNG80 확인", L"로드 실패", MB_OK);
            break;
        }
    }

    if (g_idleFrame == NULL)
        MessageBox(hWnd, L"IDB_PNG1 가만히 있는 커비 로드 실패", L"로드 실패", MB_OK);

    for (int i = 0; i < walkFrameCount; i++)
    {
        if (g_walkFrames[i] == NULL)
        {
            MessageBox(hWnd, L"커비 걷기 PNG 로드 실패: IDB_PNG2 ~ IDB_PNG5 확인", L"로드 실패", MB_OK);
            break;
        }
    }

    for (int i = 0; i < spaceFrameCount; i++)
    {
        if (g_spaceFrames[i] == NULL)
        {
            MessageBox(hWnd, L"커비 풍선 PNG 로드 실패: IDB_PNG6 ~ IDB_PNG8 확인", L"로드 실패", MB_OK);
            break;
        }
    }

    for (int i = 0; i < absorbFrameCount; i++)
    {
        if (g_absorbFrames[i] == NULL)
        {
            MessageBox(hWnd, L"커비 흡수 PNG 로드 실패: IDB_PNG9 ~ IDB_PNG11 확인", L"로드 실패", MB_OK);
            break;
        }
    }

    if (g_crouchFrame == NULL)
        MessageBox(hWnd, L"IDB_PNG12 앉기 PNG 로드 실패", L"로드 실패", MB_OK);

    if (g_spaceReleaseEffect == NULL)
        MessageBox(hWnd, L"IDB_PNG13 SPACE 해제 이펙트 PNG 로드 실패", L"로드 실패", MB_OK);

    for (int i = 0; i < 2; i++)
    {
        if (g_absorbFrontEffectFrames[i] == NULL)
        {
            MessageBox(hWnd, L"빨아들이기 앞쪽 이펙트 PNG 로드 실패: IDB_PNG14 ~ IDB_PNG15 확인", L"로드 실패", MB_OK);
            break;
        }
    }

    for (int i = 0; i < monsterJumpFrameCount; i++)
    {
        if (g_monsterJumpFrames[i] == NULL)
        {
            MessageBox(hWnd, L"몬스터 점프 공격 PNG 로드 실패: IDB_PNG16 ~ IDB_PNG17 확인", L"로드 실패", MB_OK);
            break;
        }
    }

    for (int i = 0; i < monsterFrameCount; i++)
    {
        if (g_monsterFrames[i] == NULL)
        {
            MessageBox(hWnd, L"몬스터 걷기 PNG 로드 실패: IDB_PNG18 ~ IDB_PNG21 확인", L"로드 실패", MB_OK);
            break;
        }
    }

    if (g_monsterDeadFrame == NULL)
        MessageBox(hWnd, L"IDB_PNG35 몬스터 사망 PNG 로드 실패", L"로드 실패", MB_OK);

    if (g_background == NULL)
        MessageBox(hWnd, L"IDB_PNG22 맵 PNG 로드 실패", L"로드 실패", MB_OK);

    if (g_background2 == NULL)
        MessageBox(hWnd, L"IDB_PNG23 오른쪽 맵 PNG 로드 실패", L"로드 실패", MB_OK);

    if (g_powerIdleFrame == NULL)
        MessageBox(hWnd, L"IDB_PNG24 커진 커비 대기 PNG 로드 실패", L"로드 실패", MB_OK);

    for (int i = 0; i < powerWalkFrameCount; i++)
    {
        if (g_powerWalkFrames[i] == NULL)
        {
            MessageBox(hWnd, L"커진 커비 걷기 PNG 로드 실패: IDB_PNG25, 26, 30, 31, 32 확인", L"로드 실패", MB_OK);
            break;
        }
    }

    if (g_powerAttackFrame == NULL)
        MessageBox(hWnd, L"IDB_PNG33 커진 커비 공격 PNG 로드 실패", L"로드 실패", MB_OK);

    if (g_powerProjectileFrame == NULL)
        MessageBox(hWnd, L"IDB_PNG34 투사체 PNG 로드 실패", L"로드 실패", MB_OK);

    if (g_powerDigestFrame == NULL)
        MessageBox(hWnd, L"IDB_PNG36 소화 프레임 PNG 로드 실패", L"로드 실패", MB_OK);

    if (g_kirbyHitFrame == NULL)
        MessageBox(hWnd, L"IDB_PNG37 커비 피격 프레임 PNG 로드 실패", L"로드 실패", MB_OK);

    if (g_hpBarFrame == NULL)
        MessageBox(hWnd, L"IDB_PNG38 체력바 PNG 로드 실패", L"로드 실패", MB_OK);

    if (g_fireTransformFrame == NULL)
        MessageBox(hWnd, L"IDB_PNG39 불 속성 변신 PNG 로드 실패", L"로드 실패", MB_OK);

    if (g_fireAttackKirbyFrame == NULL)
        MessageBox(hWnd, L"IDB_PNG45 불 속성 커비 공격 자세 PNG 로드 실패", L"로드 실패", MB_OK);

    if (g_fireBalloonStartFrame == NULL)
        MessageBox(hWnd, L"IDB_PNG51 불 속성 풍선 시작 PNG 로드 실패", L"로드 실패", MB_OK);

    for (int i = 0; i < 2; i++)
    {
        if (g_fireBalloonFrames[i] == NULL)
        {
            MessageBox(hWnd, L"불 속성 풍선 PNG 로드 실패: IDB_PNG52 ~ IDB_PNG53 확인", L"로드 실패", MB_OK);
            break;
        }
    }

    if (g_fireIdleFrame == NULL)
        MessageBox(hWnd, L"IDB_PNG40 불 속성 커비 기본 PNG 로드 실패", L"로드 실패", MB_OK);

    for (int i = 0; i < FIRE_WALK_FRAME_COUNT; i++)
    {
        if (g_fireWalkFrames[i] == NULL)
        {
            MessageBox(hWnd, L"불 속성 커비 걷기 PNG 로드 실패: IDB_PNG41 ~ IDB_PNG44 확인", L"로드 실패", MB_OK);
            break;
        }
    }

    if (g_fireBreathFrame == NULL)
        MessageBox(hWnd, L"IDB_PNG46 불 뿜기 PNG 로드 실패", L"로드 실패", MB_OK);

    if (g_fireBallFrame == NULL)
        MessageBox(hWnd, L"IDB_PNG47 화염구 PNG 로드 실패", L"로드 실패", MB_OK);

    if (g_fireMonsterFrame == NULL)
        MessageBox(hWnd, L"IDB_PNG48 불 속성 몬스터 PNG 로드 실패", L"로드 실패", MB_OK);

    if (g_fireMonsterAttackFrame == NULL)
        MessageBox(hWnd, L"IDB_PNG49 불 속성 몬스터 공격 PNG 로드 실패", L"로드 실패", MB_OK);

    for (int i = 0; i < dashFrameCount; i++)
    {
        if (g_dashWindFrames[i] == NULL)
        {
            MessageBox(hWnd, L"달리기 바람 PNG 로드 실패: IDB_PNG27 ~ IDB_PNG29 확인", L"로드 실패", MB_OK);
            break;
        }
    }
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

void DrawDebugInfo(HDC memDC, HWND hWnd)
{
    if (!g_debugMode)
        return;

    RECT rt;
    GetClientRect(hWnd, &rt);

    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, RGB(255, 0, 0));

    HPEN redPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
    HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, GetStockObject(HOLLOW_BRUSH));
    HPEN oldPen = (HPEN)SelectObject(memDC, redPen);

    // 충돌체를 화면 좌표로 바꿔서 표시
    for (int i = 0; i < g_solidBlockCount; i++)
    {
        RECT rc = g_solidBlocks[i].rc;
        rc.left -= cameraX;
        rc.right -= cameraX;

        if (rc.right < 0 || rc.left > rt.right)
            continue;

        Rectangle(memDC, rc.left, rc.top, rc.right, rc.bottom);
    }

    SelectObject(memDC, oldPen);
    SelectObject(memDC, oldBrush);
    DeleteObject(redPen);

    wchar_t text[256];

    wsprintf(
        text,
        L"F1 DEBUG ON | mouse screen=(%d,%d) world=(%d,%d) | cameraX=%d | kirby=(%d,%d)",
        g_mouseScreenX,
        g_mouseScreenY,
        g_mouseWorldX,
        g_mouseWorldY,
        cameraX,
        kirbyX,
        kirbyY
    );

    TextOut(memDC, 10, 10, text, lstrlen(text));

    // 마우스 위치 십자선
    HPEN bluePen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
    oldPen = (HPEN)SelectObject(memDC, bluePen);
    MoveToEx(memDC, g_mouseScreenX - 10, g_mouseScreenY, NULL);
    LineTo(memDC, g_mouseScreenX + 10, g_mouseScreenY);
    MoveToEx(memDC, g_mouseScreenX, g_mouseScreenY - 10, NULL);
    LineTo(memDC, g_mouseScreenX, g_mouseScreenY + 10);
    SelectObject(memDC, oldPen);
    DeleteObject(bluePen);
}

void DrawHPBar(Graphics& graphics)
{
    if (g_hpBarFrame == NULL)
        return;

    // 38번 체력바 PNG 원본 크기 기준 좌표
    // 이미지는 빈 프레임이고, 안쪽 체력 부분만 코드로 채움
    const int scale = 2;

    int frameX = 20;
    int frameY = 20;
    int frameW = 124 * scale;
    int frameH = 47 * scale;

    // 먼저 체력바 프레임을 그림
    graphics.DrawImage(g_hpBarFrame, frameX, frameY, frameW, frameH);

    // 빈 체력 칸 내부 위치
    int hpX = frameX + 46 * scale;
    int hpY = frameY + 26 * scale;
    int hpW = 65 * scale;
    int hpH = 9 * scale;

    int currentW = (int)(hpW * kirbyDisplayHP / kirbyMaxHP);

    if (currentW < 0)
        currentW = 0;

    if (currentW > hpW)
        currentW = hpW;

    // 체력 색상. 필요하면 RGB 값만 바꾸면 됨
    SolidBrush hpBrush(Color(255, 255, 90, 180));
    graphics.FillRectangle(&hpBrush, hpX, hpY, currentW, hpH);
}

void DrawScene(HDC hdc, HWND hWnd)
{
    RECT rt;
    GetClientRect(hWnd, &rt);

    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rt.right, rt.bottom);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

    Graphics graphics(memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    graphics.SetPixelOffsetMode(PixelOffsetModeHalf);

    // 오프닝 화면 상태이면 72번 PNG만 보여주고 게임 화면은 아직 그리지 않음
    if (g_isOpening)
    {
        if (g_openingFrame != NULL)
        {
            graphics.DrawImage(g_openingFrame, 0, 0, rt.right, rt.bottom);
        }
        else
        {
            HBRUSH bgBrush = CreateSolidBrush(RGB(0, 0, 0));
            FillRect(memDC, &rt, bgBrush);
            DeleteObject(bgBrush);

            SetBkMode(memDC, TRANSPARENT);
            SetTextColor(memDC, RGB(255, 255, 255));
            TextOut(memDC, 20, 20, L"IDB_PNG72 오프닝 이미지 로드 실패", 26);
        }

        BitBlt(hdc, 0, 0, rt.right, rt.bottom, memDC, 0, 0, SRCCOPY);

        SelectObject(memDC, oldBitmap);
        DeleteObject(memBitmap);
        DeleteDC(memDC);
        return;
    }

    // 스토리 화면 상태이면 73~81번 PNG를 0.7초마다 보여주고 게임 화면은 아직 그리지 않음
    if (g_isStory)
    {
        Image* storyFrame = NULL;

        if (g_storyFrameIndex >= 0 && g_storyFrameIndex < STORY_FRAME_COUNT)
            storyFrame = g_storyFrames[g_storyFrameIndex];

        if (storyFrame != NULL)
        {
            graphics.DrawImage(storyFrame, 0, 0, rt.right, rt.bottom);
        }
        else
        {
            HBRUSH bgBrush = CreateSolidBrush(RGB(0, 0, 0));
            FillRect(memDC, &rt, bgBrush);
            DeleteObject(bgBrush);

            SetBkMode(memDC, TRANSPARENT);
            SetTextColor(memDC, RGB(255, 255, 255));
            TextOut(memDC, 20, 20, L"IDB_PNG73~81 스토리 이미지 로드 실패", 27);
        }

        BitBlt(hdc, 0, 0, rt.right, rt.bottom, memDC, 0, 0, SRCCOPY);

        SelectObject(memDC, oldBitmap);
        DeleteObject(memBitmap);
        DeleteDC(memDC);
        return;
    }

    // 배경은 월드 좌표 기준으로 이어 붙여서 그리고, 화면에는 cameraX만큼 밀려 보이게 함
    if (g_background != NULL)
    {
        graphics.DrawImage(g_background, 0 - cameraX, 0, BG_PART_W, BG_PART_H);
    }
    else
    {
        HBRUSH bgBrush = CreateSolidBrush(RGB(180, 220, 255));
        FillRect(memDC, &rt, bgBrush);
        DeleteObject(bgBrush);
    }

    if (g_background2 != NULL)
    {
        graphics.DrawImage(g_background2, BG_PART_W - cameraX, 0, BG_PART_W, BG_PART_H);
    }

    // 커비/몬스터/이펙트는 전부 월드 좌표로 움직이고,
    // 그릴 때만 -cameraX만큼 이동해서 화면에 표시
    GraphicsState worldState = graphics.Save();
    graphics.TranslateTransform((REAL)(-cameraX), 0.0f);

    for (int i = 0; i < MONSTER_COUNT; i++)
    {
        g_monsters[i].Draw(graphics);
    }

    DrawDashWind(graphics);

    DrawPowerProjectile(graphics);
    DrawAbilityStar(graphics);
    DrawFireBall(graphics);
    DrawBombObjects(graphics);
    DrawBombExplosions(graphics);
    DrawEnemyFireBalls(graphics);

    if (isKirbyHit)
    {
        Image* hitFrame = g_kirbyHitFrame;

        if ((isBombKirby || isBombTransform) && g_bombKirbyHitFrame != NULL)
            hitFrame = g_bombKirbyHitFrame;
        else if ((isFireKirby || isFireTransform) && g_fireKirbyHitFrame != NULL)
            hitFrame = g_fireKirbyHitFrame;

        if (hitFrame != NULL)
            DrawKirbyImage(graphics, hitFrame);
    }
    else if (isAbsorb && g_absorbFrames[absorbFrameIndex] != NULL)
    {
        DrawKirbyImage(graphics, g_absorbFrames[absorbFrameIndex]);
        DrawAbsorbFrontEffect(graphics);
    }
    else if (isPowerDigest && g_powerDigestFrame != NULL)
    {
        DrawKirbyImage(graphics, g_powerDigestFrame);
    }
    else if (isFireTransform && g_fireTransformFrame != NULL)
    {
        DrawKirbyImage(graphics, g_fireTransformFrame);
    }
    else if (isBombTransform && g_bombTransformFrame != NULL)
    {
        DrawKirbyImage(graphics, g_bombTransformFrame);
    }
    else if (isFireAttackPose && g_fireAttackKirbyFrame != NULL)
    {
        DrawKirbyImage(graphics, g_fireAttackKirbyFrame);
    }
    else if (isBombAttack && g_bombAttackFrames[bombAttackFrameIndex] != NULL)
    {
        DrawKirbyImage(graphics, g_bombAttackFrames[bombAttackFrameIndex]);
    }
    else if (isPowerAttack && g_powerAttackFrame != NULL)
    {
        DrawKirbyImage(graphics, g_powerAttackFrame);
    }
    else if (isSpaceRelease)
    {
        if (isBombKirby && g_bombBalloonStartFrame != NULL)
        {
            DrawKirbyImage(graphics, g_bombBalloonStartFrame);
        }
        else if (isFireKirby && g_fireBalloonStartFrame != NULL)
        {
            // 불 커비는 SPACE를 뗄 때도 50번을 잠깐 보여줌
            DrawKirbyImage(graphics, g_fireBalloonStartFrame);
        }
        else
        {
            DrawKirbyImage(graphics, g_absorbFrames[0]);
        }

        // 커비 앞에 13번 이펙트 표시
        DrawSpaceReleaseEffect(graphics);
    }
    else if (isSpace)
    {
        if (isFireKirby)
        {
            if (!fireBalloonStartFrameDone && g_fireBalloonStartFrame != NULL)
            {
                DrawKirbyImage(graphics, g_fireBalloonStartFrame);
            }
            else if (g_fireBalloonFrames[fireBalloonFrameIndex] != NULL)
            {
                DrawKirbyImage(graphics, g_fireBalloonFrames[fireBalloonFrameIndex]);
            }
        }
        else if (isBombKirby)
        {
            if (!bombBalloonStartFrameDone && g_bombBalloonStartFrame != NULL)
            {
                DrawKirbyImage(graphics, g_bombBalloonStartFrame);
            }
            else if (g_bombBalloonFrames[bombBalloonFrameIndex] != NULL)
            {
                DrawKirbyImage(graphics, g_bombBalloonFrames[bombBalloonFrameIndex]);
            }
        }
        else if (g_spaceFrames[spaceFrameIndex] != NULL)
        {
            DrawKirbyImage(graphics, g_spaceFrames[spaceFrameIndex]);
        }
    }
    else if (isCrouch)
    {
        Image* crouchImage = g_crouchFrame;

        if (isFireKirby && g_fireCrouchFrame != NULL)
            crouchImage = g_fireCrouchFrame;
        else if (isBombKirby && g_bombIdleFrame != NULL)
            crouchImage = g_bombIdleFrame;

        if (crouchImage != NULL)
        {
            if (kirbyFaceLeft)
            {
                DrawImageFlipX(
                    graphics,
                    crouchImage,
                    kirbyX,
                    kirbyY + crouchDrawOffsetY,
                    kirbyW,
                    kirbyH
                );
            }
            else
            {
                graphics.DrawImage(
                    crouchImage,
                    kirbyX,
                    kirbyY + crouchDrawOffsetY,
                    kirbyW,
                    kirbyH
                );
            }
        }
    }
    else if (isPowerKirby && IsKirbyWalkMoving() && g_powerWalkFrames[powerWalkFrameIndex] != NULL)
    {
        DrawKirbyImage(graphics, g_powerWalkFrames[powerWalkFrameIndex]);
    }
    else if (isPowerKirby && g_powerIdleFrame != NULL)
    {
        DrawKirbyImage(graphics, g_powerIdleFrame);
    }
    else if (isFireKirby && IsKirbyWalkMoving() && g_fireWalkFrames[fireWalkFrameIndex] != NULL)
    {
        DrawKirbyImage(graphics, g_fireWalkFrames[fireWalkFrameIndex]);
    }
    else if (isFireKirby && g_fireIdleFrame != NULL)
    {
        DrawKirbyImage(graphics, g_fireIdleFrame);
    }
    else if (isBombKirby && IsKirbyWalkMoving() && g_bombWalkFrames[bombWalkFrameIndex] != NULL)
    {
        DrawKirbyImage(graphics, g_bombWalkFrames[bombWalkFrameIndex]);
    }
    else if (isBombKirby && g_bombIdleFrame != NULL)
    {
        DrawKirbyImage(graphics, g_bombIdleFrame);
    }
    else if (IsKirbyWalkMoving() && g_walkFrames[walkFrameIndex] != NULL)
    {
        DrawKirbyImage(graphics, g_walkFrames[walkFrameIndex]);
    }
    else
    {
        DrawKirbyImage(graphics, g_idleFrame);
    }

    DrawFireBreath(graphics);

    graphics.Restore(worldState);

    // 화면 고정 UI: 카메라 영향을 받지 않는 체력바
    DrawHPBar(graphics);

    graphics.Flush();

    DrawDebugInfo(memDC, hWnd);

    BitBlt(hdc, 0, 0, rt.right, rt.bottom, memDC, 0, 0, SRCCOPY);

    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    switch (iMessage)
    {
    case WM_CREATE:
        LoadAllImages(hWnd);
        InitMonsters();

        // WAV 리소스 배경음악 재생
        // resource.h에 있는 실제 소리 ID 이름이 다르면 IDR_WAVE1만 바꾸면 됨
        PlaySound(
            MAKEINTRESOURCE(IDR_WAVE1),
            g_hInst,
            SND_RESOURCE | SND_ASYNC | SND_LOOP
        );

        UpdateCamera(hWnd);

        SetTimer(hWnd, 1, 16, NULL);
        SetTimer(hWnd, 2, 120, NULL);
        SetTimer(hWnd, 3, 180, NULL);
        SetTimer(hWnd, 5, 160, NULL);
        SetTimer(hWnd, 7, 120, NULL);
        break;

    case WM_TIMER:
        if (g_isOpening)
        {
            InvalidateRect(hWnd, NULL, FALSE);
            return 0;
        }

        if (g_isStory)
        {
            if (wParam == 1)
            {
                g_storyTick++;

                if (g_storyTick >= STORY_FRAME_DURATION)
                {
                    g_storyTick = 0;
                    g_storyFrameIndex++;

                    if (g_storyFrameIndex >= STORY_FRAME_COUNT)
                    {
                        g_isStory = false;
                        g_storyFrameIndex = STORY_FRAME_COUNT - 1;
                        StopMove();
                        isSpace = false;
                        isSpaceRelease = false;
                        balloonTick = 0;
                        spaceKeyHeld = false;
                        spaceFrameIndex = 0;
                        spaceStartFrameDone = false;
                        fireBalloonFrameIndex = 0;
                        fireBalloonStartFrameDone = false;
                        bombBalloonFrameIndex = 0;
                        bombBalloonStartFrameDone = false;
                        UpdateCamera(hWnd);
                    }
                }
            }

            InvalidateRect(hWnd, NULL, FALSE);
            return 0;
        }

        if (wParam == 1)
        {
            if (!isDragging)
            {
                UpdateKirbyPosition(hWnd);
            }

            UpdateBalloonLimit();
            UpdateDashWindFrame();
            UpdateSpaceRelease();
            UpdateAbsorbFrontEffect();
            UpdatePowerWait();
            UpdatePowerAttack();
            UpdatePowerDigest();
            UpdateFireKirbyStates();
            UpdateKirbyHitEffect();
            UpdateHPBarAnimation();
            UpdatePowerProjectile();
            UpdateAbilityStar();
            UpdateBombAttack();
            UpdateBombObjects();
            UpdateEnemyFireBalls();
            CheckPowerProjectileHitMonsters();
            CheckFireAttacksHitMonsters();

            for (int i = 0; i < MONSTER_COUNT; i++)
            {
                g_monsters[i].Update();
            }

            CheckKirbyHitByMonsters();
            CheckEnemyFireBallsHitKirby();

            if (isGameOver)
            {
                gameOverTick++;

                if (gameOverTick >= GAME_OVER_DELAY)
                {
                    MessageBox(hWnd, L"체력이 0%가 되었습니다. 게임 오버!", L"GAME OVER", MB_OK);
                    DestroyWindow(hWnd);
                    return 0;
                }
            }

            UpdateCamera(hWnd);

            InvalidateRect(hWnd, NULL, FALSE);
        }
        else if (wParam == 2)
        {
            if (!isSpace && !isAbsorb && !isCrouch && IsKirbyWalkMoving())
            {
                if (isPowerKirby)
                {
                    powerWalkFrameIndex++;

                    if (powerWalkFrameIndex >= powerWalkFrameCount)
                    {
                        powerWalkFrameIndex = 0;
                    }
                }
                else if (isFireKirby)
                {
                    fireWalkFrameIndex++;

                    if (fireWalkFrameIndex >= FIRE_WALK_FRAME_COUNT)
                    {
                        fireWalkFrameIndex = 0;
                    }
                }
                else if (isBombKirby)
                {
                    bombWalkFrameIndex++;

                    if (bombWalkFrameIndex >= BOMB_WALK_FRAME_COUNT)
                    {
                        bombWalkFrameIndex = 0;
                    }
                }
                else
                {
                    walkFrameIndex++;

                    if (walkFrameIndex >= walkFrameCount)
                    {
                        walkFrameIndex = 0;
                    }
                }
            }
            else
            {
                walkFrameIndex = 0;
                powerWalkFrameIndex = 0;
                fireWalkFrameIndex = 0;
                bombWalkFrameIndex = 0;
            }

            InvalidateRect(hWnd, NULL, FALSE);
        }
        else if (wParam == 3)
        {
            if (isSpace && !isAbsorb)
            {
                if (isFireKirby)
                {
                    if (!fireBalloonStartFrameDone)
                    {
                        fireBalloonFrameIndex = 0;
                        fireBalloonStartFrameDone = true;
                    }
                    else
                    {
                        fireBalloonFrameIndex++;

                        if (fireBalloonFrameIndex >= 2)
                            fireBalloonFrameIndex = 0;
                    }
                }
                else if (isBombKirby)
                {
                    if (!bombBalloonStartFrameDone)
                    {
                        bombBalloonFrameIndex = 0;
                        bombBalloonStartFrameDone = true;
                    }
                    else
                    {
                        bombBalloonFrameIndex++;

                        if (bombBalloonFrameIndex >= 2)
                            bombBalloonFrameIndex = 0;
                    }
                }
                else
                {
                    if (!spaceStartFrameDone)
                    {
                        spaceFrameIndex = 0;
                        spaceStartFrameDone = true;
                    }
                    else
                    {
                        if (spaceFrameIndex == 0)
                        {
                            spaceFrameIndex = 1;
                        }
                        else if (spaceFrameIndex == 1)
                        {
                            spaceFrameIndex = 2;
                        }
                        else
                        {
                            spaceFrameIndex = 1;
                        }
                    }
                }
            }
            else
            {
                spaceFrameIndex = 0;
                spaceStartFrameDone = false;
                fireBalloonFrameIndex = 0;
                fireBalloonStartFrameDone = false;
                bombBalloonFrameIndex = 0;
                bombBalloonStartFrameDone = false;
            }

            InvalidateRect(hWnd, NULL, FALSE);
        }
        else if (wParam == 5)
        {
            if (isAbsorb)
            {
                if (absorbFrameIndex < absorbFrameCount - 1)
                {
                    absorbFrameIndex++;
                }
                else
                {
                    absorbFrameIndex = absorbFrameCount - 1;
                }

                InvalidateRect(hWnd, NULL, FALSE);
            }
        }
        else if (wParam == 7)
        {
            for (int i = 0; i < MONSTER_COUNT; i++)
            {
                g_monsters[i].NextFrame();
            }
            InvalidateRect(hWnd, NULL, FALSE);
        }
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        DrawScene(hdc, hWnd);
        EndPaint(hWnd, &ps);
        break;

    case WM_LBUTTONDOWN:
    {
        if (g_isOpening || g_isStory)
            return 0;

        if (isAbsorb)
            return 0;

        int mouseX = LOWORD(lParam);
        int mouseY = HIWORD(lParam);

        g_mouseScreenX = mouseX;
        g_mouseScreenY = mouseY;
        g_mouseWorldX = mouseX + cameraX;
        g_mouseWorldY = mouseY;

        // 마우스는 화면 좌표이므로 cameraX를 더해서 월드 좌표로 변환
        int worldMouseX = mouseX + cameraX;

        if (IsInsideKirby(worldMouseX, mouseY))
        {
            isDragging = true;
            dragOffsetX = worldMouseX - kirbyX;
            dragOffsetY = mouseY - kirbyY;
            SetCapture(hWnd);
        }
        break;
    }

    case WM_MOUSEMOVE:
    {
        if (g_isOpening || g_isStory)
            return 0;

        int mouseX = LOWORD(lParam);
        int mouseY = HIWORD(lParam);

        g_mouseScreenX = mouseX;
        g_mouseScreenY = mouseY;
        g_mouseWorldX = mouseX + cameraX;
        g_mouseWorldY = mouseY;

        if (g_debugMode)
        {
            InvalidateRect(hWnd, NULL, FALSE);
        }

        if (isDragging && !isAbsorb)
        {
            int worldMouseX = mouseX + cameraX;

            kirbyX = worldMouseX - dragOffsetX;
            kirbyY = mouseY - dragOffsetY;

            if (kirbyX < 0)
                kirbyX = 0;

            if (kirbyX + kirbyW > WORLD_W)
                kirbyX = WORLD_W - kirbyW;

            UpdateCamera(hWnd);

            InvalidateRect(hWnd, NULL, FALSE);
        }
        break;
    }

    case WM_LBUTTONUP:
        if (isDragging)
        {
            isDragging = false;
            ReleaseCapture();
        }
        break;

    case WM_KEYDOWN:
        if (g_isOpening)
        {
            if (wParam == VK_SPACE)
            {
                g_isOpening = false;
                g_isStory = true;
                g_storyFrameIndex = 0;
                g_storyTick = 0;
                StopMove();
                isSpace = false;
                isSpaceRelease = false;
                balloonTick = 0;
                spaceKeyHeld = false;
                spaceFrameIndex = 0;
                spaceStartFrameDone = false;
                fireBalloonFrameIndex = 0;
                fireBalloonStartFrameDone = false;
                bombBalloonFrameIndex = 0;
                bombBalloonStartFrameDone = false;
                InvalidateRect(hWnd, NULL, FALSE);
            }
            else if (wParam == VK_ESCAPE)
            {
                DestroyWindow(hWnd);
            }

            return 0;
        }

        if (g_isStory)
        {
            if (wParam == VK_ESCAPE)
            {
                DestroyWindow(hWnd);
            }

            return 0;
        }

        if (wParam == VK_F1)
        {
            g_debugMode = !g_debugMode;
            InvalidateRect(hWnd, NULL, FALSE);
            return 0;
        }

        if (wParam == VK_SHIFT)
        {
            isDash = true;
            InvalidateRect(hWnd, NULL, FALSE);
            return 0;
        }

        if (isAbsorb && wParam != 'K')
        {
            return 0;
        }

        switch (wParam)
        {
        case 'W':
            if (isSpace)
            {
                moveUp = true;
            }
            else
            {
                StartJump();
            }
            break;

        case 'A':
            moveLeft = true;
            kirbyFaceLeft = true;
            break;

        case 'D':
            moveRight = true;
            kirbyFaceLeft = false;
            break;

        case 'S':
            if (isSpace)
            {
                moveDown = true;
            }
            else
            {
                isCrouch = true;
                moveLeft = false;
                moveRight = false;
            }
            break;

        case VK_SPACE:
            // SPACE를 계속 누르고 있는 중에 2초 제한으로 풍선이 풀렸다면,
            // 키를 한 번 떼기 전까지 다시 풍선이 켜지지 않게 함
            if (spaceKeyHeld && !isSpace)
            {
                break;
            }

            spaceKeyHeld = true;

            // 커진 커비 상태에서는 풍선 상태로 들어갈 수 없음
            if (isPowerKirby)
            {
                isSpace = false;
                isSpaceRelease = false;
                balloonTick = 0;
                spaceKeyHeld = false;
                spaceFrameIndex = 0;
                spaceStartFrameDone = false;
                fireBalloonFrameIndex = 0;
                fireBalloonStartFrameDone = false;
                bombBalloonFrameIndex = 0;
                bombBalloonStartFrameDone = false;
                moveUp = false;
                moveDown = false;
                break;
            }

            if (!isAbsorb)
            {
                if (!isSpace)
                {
                    balloonTick = 0;
                    spaceFrameIndex = 0;
                    spaceStartFrameDone = false;
                    fireBalloonFrameIndex = 0;
                    fireBalloonStartFrameDone = false;
                    bombBalloonFrameIndex = 0;
                    bombBalloonStartFrameDone = false;
                }

                isSpace = true;
                isSpaceRelease = false;
                isCrouch = false;

                kirbyVY = 0.0f;
                isOnGround = false;

                if (GetAsyncKeyState('W') & 0x8000)
                    moveUp = true;

                if (GetAsyncKeyState('S') & 0x8000)
                    moveDown = true;

                if (GetAsyncKeyState('A') & 0x8000)
                    kirbyFaceLeft = true;

                if (GetAsyncKeyState('D') & 0x8000)
                    kirbyFaceLeft = false;
            }
            break;

        case 'O':
            // 변신한 상태에서 O를 누르면 능력별을 뒤로 뱉고 기본 커비로 돌아감
            // 뱉은 34번 별을 다시 빨아들이면 이전 속성으로 복귀함
            EjectAbilityStar();
            break;

        case 'K':
            if (isBombKirby)
            {
                StartBombAttack();
            }
            else if (isFireKirby)
            {
                // 불 속성 커비 상태에서는 K가 빨아들이기가 아니라 46번 불 뿜기 공격
                isSpace = false;
                isSpaceRelease = false;
                isCrouch = false;
                StartFireBreath();
            }
            else if (isPowerKirby)
            {
                // 커진 커비 상태에서는 K가 빨아들이기가 아니라 33번 자세 + 34번 발사로 동작
                isSpace = false;
                isSpaceRelease = false;
                isCrouch = false;

                spaceFrameIndex = 0;
                spaceStartFrameDone = false;

                StartPowerProjectile();
            }
            else if (!isAbsorb)
            {
                isAbsorb = true;
                absorbFrameIndex = 0;

                absorbFrontEffectIndex = 0;
                absorbFrontEffectTick = 0;

                isSpace = false;
                isSpaceRelease = false;
                isCrouch = false;

                spaceFrameIndex = 0;
                spaceStartFrameDone = false;

                StopMove();
            }
            break;

        case 'L':
            if (isPowerKirby)
            {
                // 커진 커비 상태에서 L을 누르면 36번 프레임으로 소화시킴
                // 먹은 몬스터가 불 속성 1번이면 39번 뒤에 40번 불 커비가 됨
                DigestPowerKirby();
            }
            break;

        case 'I':
            if (isFireKirby)
            {
                // 불 속성 커비 I 공격: 47번 화염구 발사
                SpawnFireBall();
            }
            break;

        case VK_ESCAPE:
            DestroyWindow(hWnd);
            break;
        }

        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_KEYUP:
        if (g_isOpening || g_isStory)
            return 0;

        if (wParam == VK_SHIFT)
        {
            isDash = false;
            dashFrameIndex = 0;
            dashFrameTick = 0;
            InvalidateRect(hWnd, NULL, FALSE);
            return 0;
        }

        if (isAbsorb && wParam != 'K')
        {
            return 0;
        }

        switch (wParam)
        {
        case 'W':
            moveUp = false;
            jumpKeyDown = false;
            break;

        case 'A':
            moveLeft = false;

            if (GetAsyncKeyState('D') & 0x8000)
                kirbyFaceLeft = false;

            break;

        case 'D':
            moveRight = false;

            if (GetAsyncKeyState('A') & 0x8000)
                kirbyFaceLeft = true;

            break;

        case 'S':
            moveDown = false;
            isCrouch = false;
            break;

        case VK_SPACE:
            spaceKeyHeld = false;

            if (isPowerKirby)
            {
                isSpace = false;
                isSpaceRelease = false;
                balloonTick = 0;
                spaceFrameIndex = 0;
                spaceStartFrameDone = false;
                fireBalloonFrameIndex = 0;
                fireBalloonStartFrameDone = false;
                bombBalloonFrameIndex = 0;
                bombBalloonStartFrameDone = false;
                moveUp = false;
                moveDown = false;
                break;
            }

            StopBalloonWithRelease();
            break;

        case 'K':
            if (!isPowerKirby && !isFireKirby && !isBombKirby)
            {
                isAbsorb = false;
                absorbFrameIndex = 0;

                absorbFrontEffectIndex = 0;
                absorbFrontEffectTick = 0;
            }
            break;
        }

        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_DESTROY:
        // 프로그램 종료 시 소리 정지
        PlaySound(NULL, NULL, 0);

        KillTimer(hWnd, 1);
        KillTimer(hWnd, 2);
        KillTimer(hWnd, 3);
        KillTimer(hWnd, 5);
        KillTimer(hWnd, 7);
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hWnd, iMessage, wParam, lParam);
}