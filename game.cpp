#include "Game.h"
#include "resource.h"
#include <mmsystem.h>
#include <stdlib.h>
#include <time.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "winmm.lib")

using namespace Gdiplus;


#ifndef IDB_PNG93
#define IDB_PNG93 93
#endif
#ifndef IDB_PNG94
#define IDB_PNG94 94
#endif
#ifndef IDB_PNG95
#define IDB_PNG95 95
#endif
#ifndef IDB_PNG96
#define IDB_PNG96 96
#endif
#ifndef IDB_PNG97
#define IDB_PNG97 97
#endif
#ifndef IDB_PNG98
#define IDB_PNG98 98
#endif
#ifndef IDB_PNG99
#define IDB_PNG99 99
#endif
#ifndef IDB_PNG100
#define IDB_PNG100 100
#endif
#ifndef IDB_PNG101
#define IDB_PNG101 101
#endif
#ifndef IDB_PNG102
#define IDB_PNG102 102
#endif
#ifndef IDB_PNG103
#define IDB_PNG103 103
#endif
#ifndef IDB_PNG104
#define IDB_PNG104 104
#endif
#ifndef IDB_PNG105
#define IDB_PNG105 105
#endif
#ifndef IDB_PNG106
#define IDB_PNG106 106
#endif
#ifndef IDB_PNG107
#define IDB_PNG107 107
#endif
#ifndef IDB_PNG108
#define IDB_PNG108 108
#endif
#ifndef IDB_PNG109
#define IDB_PNG109 109
#endif
#ifndef IDB_PNG110
#define IDB_PNG110 110
#endif
#ifndef IDB_PNG111
#define IDB_PNG111 111
#endif
#ifndef IDB_PNG112
#define IDB_PNG112 112
#endif
#ifndef IDB_PNG113
#define IDB_PNG113 113
#endif
#ifndef IDB_PNG114
#define IDB_PNG114 114
#endif


HINSTANCE g_hInst;
LPCTSTR lpszClass = L"WindowClass";
LPCTSTR lpszWindowName = L"GDI+ Kirby Animation";

ULONG_PTR g_gdiplusToken;

// 72번: 오프닝 화면
Image* g_openingFrame = NULL;

// 73~79번: 오프닝 뒤에 자동으로 넘어가는 스토리 화면
Image* g_storyFrames[7] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };
int g_storyFrameIndex = 0;
int g_storyTick = 0;
const int STORY_FRAME_COUNT = 7;
const int STORY_FRAME_DURATION = 22; // 40ms 타이머 기준 약 0.9초

// 처음 실행하면 72번 오프닝, SPACE를 누르면 73~79번 스토리 진행 후 1스테이지 시작
bool g_isOpening = true;
bool g_isStory = false;


// 81번: 악몽 속에서 떨고 있는 남자 아이
Image* g_studentBoyFrame = NULL;

// 83번: 악몽 속에서 떨고 있는 여자 아이
Image* g_studentGirlFrame = NULL;

// 84~87번: 다음 맵으로 넘어가는 문 열림 애니메이션
Image* g_doorFrames[4] = { NULL, NULL, NULL, NULL };
const int DOOR_FRAME_COUNT = 4;

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

// 1스테이지 구출/문 상태
RescueChild g_stage1Boy;
StageDoor g_stage1Door;
int g_stage1ChildTotal = 1;
int g_stage1ChildRescued = 0;

// 2스테이지 구출/문 상태
const int STAGE2_CHILD_COUNT = 4;
RescueChild g_stage2Children[STAGE2_CHILD_COUNT];
int g_stage2ChildFrameType[STAGE2_CHILD_COUNT] = { 81, 83, 81, 83 };
StageDoor g_stage2Door;
int g_stage2ChildTotal = STAGE2_CHILD_COUNT;
int g_stage2ChildRescued = 0;

// 3스테이지 문 상태. 3스테이지는 구출 없이 문만 적당한 위치에 배치
StageDoor g_stage3Door;

int g_currentStage = 1;
bool g_isChangingMap = false;
int g_rescueAnimTick = 0;
const int DOOR_OPEN_FRAME_TICK = 8; // 값이 클수록 문이 천천히 열림

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
Bitmap* g_backgroundScaled = NULL; // 렉 줄이기용: 늘린 배경을 미리 만들어 둠

// 23번: 오른쪽에 이어지는 맵 2
Image* g_background2 = NULL;
Bitmap* g_background2Scaled = NULL; // 렉 줄이기용: 늘린 배경을 미리 만들어 둠

// 88번: 2스테이지 앞쪽 배경(달 있는 곳)
Image* g_stage2BackgroundFront = NULL;
Bitmap* g_stage2BackgroundFrontScaled = NULL;

// 89번: 2스테이지 뒤쪽 배경(달 없는 곳)
Image* g_stage2BackgroundBack = NULL;
Bitmap* g_stage2BackgroundBackScaled = NULL;

// 90~91번: 3스테이지 배경
Image* g_stage3BackgroundFront = NULL;
Bitmap* g_stage3BackgroundFrontScaled = NULL;
Image* g_stage3BackgroundBack = NULL;
Bitmap* g_stage3BackgroundBackScaled = NULL;

// 92번: 마지막 4스테이지 / 보스전 배경
Image* g_stage4Background = NULL;
Bitmap* g_stage4BackgroundScaled = NULL;

// 93~101번: 4스테이지 보스 관련 프레임
Image* g_bossMissilePoseFrame = NULL; // 93번: 미사일 공격 자세
Image* g_bossDashFrame = NULL;        // 94번: 대각선 돌진
Image* g_bossIdleMoveFrame = NULL;    // 95번: 보스 기본/이동
Image* g_bossMissileFrame = NULL;     // 96번: 미사일
Image* g_bossMouthBombFrame = NULL;   // 97번: 입에서 떨어지는 폭탄
Image* g_bossTopAttackFrame = NULL;   // 98번: 위쪽 폭탄 공격 자세
Image* g_bossPhase2Frame = NULL;      // 99번: 2페이즈 기본 모습
Image* g_bossRainAttackFrame = NULL;  // 100번: 하늘 공격
Image* g_bossRainBombFrame = NULL;    // 101번: 하늘에서 떨어지는 폭탄
Image* g_bossDeathFrame1 = NULL;      // 102번: 보스 사망 연출 1
Image* g_bossDeathFrame2 = NULL;      // 103번: 보스 사망 연출 2
Image* g_bossPatternRedBallFrame = NULL;  // 104번: 보스 패턴 빨간 공
Image* g_bossPatternBlueBallFrame = NULL; // 105번: 보스 패턴 파란 공
Image* g_bossHalfFloorWarnFrame = NULL;   // 106번: 바닥 절반 경고
Image* g_bossHalfFloorBoomFrame = NULL;   // 107번: 바닥 절반 폭발
Image* g_bossDoorFrames[4] = { NULL, NULL, NULL, NULL }; // 108~111번 문 열림
Image* g_bossKeyFrame = NULL;         // 112번 열쇠
Image* g_bossChestClosedFrame = NULL; // 113번 닫힌 상자
Image* g_bossChestOpenFrame = NULL;   // 114번 열린 상자

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

// K/I 공격 시 45번 불 속성 커비 공격 자세ff
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
bool g_bombSpecialAttackMode = false; // I 필살기 모드
int g_bombKCooldownTick = 0;          // K 일반 폭탄 쿨타임
int g_bombICooldownTick = 0;          // I 필살기 쿨타임
const int BOMB_K_COOLDOWN_MAX = 5;    // GAME_TIMER_MS 40ms 기준 약 0.2초
const int BOMB_I_COOLDOWN_MAX = 125;  // GAME_TIMER_MS 40ms 기준 약 5초

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
    int damage;
    bool bounce;
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
bool g_gameOverHandled = false; // 게임오버 메시지박스가 여러 번 뜨는 것 방지
bool g_invincibleMode = false;  // F2 무적모드
int g_kirbySlowTick = 0;          // 파란 공 피격 시 이동속도 감소 시간
int g_kirbyBurnTick = 0;          // 빨간 공 피격 시 지속피해 시간
int g_kirbyBurnDamageTick = 0;    // 지속피해 간격
int gameOverTick = 0;
const int GAME_OVER_DELAY = 30; // HP 표시가 0이 된 뒤 약 0.5초 후 종료

// 낙사 게임오버 상태. HP가 0이 된 게임오버와 메시지를 다르게 보여주기 위해 따로 저장
bool g_kirbyFallGameOver = false;

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
int kirbyX = 55;
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
    { { 892, 539, 1000, 650 }, L"S3_90_RIGHT_GROUND" },


    // 화면 끝에 걸리는 정도만 막는 얇은 벽
    // 예전처럼 x=0~115, x=945~1000 전체를 막으면 충돌범위가 너무 어색하게 커짐
    { { 0, 0, 12, 650 }, L"S3_90_LEFT_LIMIT" },

    // PNG91 두 번째 구간: x = 1000 ~ 1999
 { { 1000, 536, 2000, 650 }, L"S3_90_TOP_LEDGE" },
 { { 1182, 406, 2000, 650 }, L"S3_91_LEFT_BOTTOM_GROUND" },
 { { 1137, 476, 1190, 491 }, L"S3_91_MAIN_LEDGE WOOD" },
 { { 1100, 406, 1187, 424 }, L"S3_91_MAIN_GROUNDWOOD" },
 { { 1175, 406, 2000 , 650 }, L"S3_91_CENTER_PILLAR" },

 { { 1852, 290, 1900, 405 }, L"S3_91_RIGHT_LEDGE" },
 { { 1635, 425, 2000, 650 }, L"S3_91_RIGHT_GROUND" },
   { { 1660, 290, 1723, 405 }, L"S3_91_RIGHT_OBSTACLE" },
       { { 1660, 290, 1736, 348 }, L"S3_91_RIGHT_OBSTACLE.2" },
           { { 1761, 290, 1900, 348 }, L"S3_91_RIGHT_WALL" },
 { { 1805, 123, 1890, 146 }, L"S3_91_TOP_LEDGE" },
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
// 4스테이지 충돌체: 92번 보스전 배경
// 보스전용이라 바닥 하나만 길게 깔아둠
// =========================
SolidBlock g_stage4SolidBlocks[] =
{
    // 4스테이지는 92번 배경 하나만 사용하는 보스전 맵이라 x = 0 ~ 1000까지만 사용
    { { 0, 545, 1000, 650 }, L"S4_BOSS_GROUND" },
    { { 0, 0, 12, 650 }, L"S4_LEFT_LIMIT" },
    { { 988, 0, 1000, 650 }, L"S4_RIGHT_LIMIT" }
};

int g_stage4SolidBlockCount = sizeof(g_stage4SolidBlocks) / sizeof(g_stage4SolidBlocks[0]);

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

    *count = g_solidBlockCount;
    return g_solidBlocks;
}


// 카메라 / 월드 크기
// 배경 PNG22가 0~999, PNG23이 1000~1999에 붙는 구조
const int BG_PART_W = 1000;
const int BG_PART_H = 650;
const int WORLD_W = BG_PART_W * 2;
const int WORLD_H = BG_PART_H;

int GetCurrentWorldW()
{
    if (g_currentStage == 4)
        return BG_PART_W;

    return WORLD_W;
}

int cameraX = 0;


// 렉 줄이기용: 백버퍼를 매번 만들지 않고 재사용함
const UINT GAME_TIMER_MS = 40; // 렉 줄이기용. 약 25FPS라 GDI+ PNG가 훨씬 덜 버벅임
HDC g_backDC = NULL;
HBITMAP g_backBitmap = NULL;
HBITMAP g_backOldBitmap = NULL;
int g_backW = 0;
int g_backH = 0;

void ReleaseBackBuffer()
{
    if (g_backDC != NULL)
    {
        if (g_backOldBitmap != NULL)
        {
            SelectObject(g_backDC, g_backOldBitmap);
            g_backOldBitmap = NULL;
        }
        DeleteDC(g_backDC);
        g_backDC = NULL;
    }

    if (g_backBitmap != NULL)
    {
        DeleteObject(g_backBitmap);
        g_backBitmap = NULL;
    }

    g_backW = 0;
    g_backH = 0;
}

bool PrepareBackBuffer(HDC hdc, int w, int h)
{
    if (w <= 0 || h <= 0)
        return false;

    if (g_backDC != NULL && g_backBitmap != NULL && g_backW == w && g_backH == h)
        return true;

    ReleaseBackBuffer();

    g_backDC = CreateCompatibleDC(hdc);
    if (g_backDC == NULL)
        return false;

    g_backBitmap = CreateCompatibleBitmap(hdc, w, h);
    if (g_backBitmap == NULL)
    {
        ReleaseBackBuffer();
        return false;
    }

    g_backOldBitmap = (HBITMAP)SelectObject(g_backDC, g_backBitmap);
    g_backW = w;
    g_backH = h;
    return true;
}



Bitmap* CreateScaledBitmap(Image* source, int w, int h)
{
    if (source == NULL)
        return NULL;

    Bitmap* bmp = new Bitmap(w, h, PixelFormat32bppARGB);
    if (bmp == NULL || bmp->GetLastStatus() != Ok)
    {
        delete bmp;
        return NULL;
    }

    Graphics g(bmp);
    g.SetCompositingQuality(CompositingQualityHighSpeed);
    g.SetSmoothingMode(SmoothingModeNone);
    g.SetInterpolationMode(InterpolationModeNearestNeighbor);
    g.SetPixelOffsetMode(PixelOffsetModeHalf);
    g.DrawImage(source, 0, 0, w, h);

    return bmp;
}

void ResizeWindowToClient(HWND hWnd, int clientW, int clientH)
{
    RECT rc = { 0, 0, clientW, clientH };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    SetWindowPos(hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);
}

bool IsVisibleWorld(int x, int y, int w, int h)
{
    if (x + w < cameraX) return false;
    if (x > cameraX + g_backW) return false;
    if (y + h < 0) return false;
    if (y > g_backH) return false;
    return true;
}

void DrawWorldImage(Graphics& graphics, Image* image, int x, int y, int w, int h)
{
    if (image == NULL)
        return;

    if (!IsVisibleWorld(x, y, w, h))
        return;

    graphics.DrawImage(image, x, y, w, h);
}

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


void UpdateCamera(HWND hWnd);
void DrawWorldImage(Graphics& graphics, Image* image, int x, int y, int w, int h);

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

void InitMonsters();

void InitBossObjects();
void UpdateBossObjects();
void DrawBossObjects(Graphics& graphics);
void CheckKirbyAttacksHitBoss();
void ResetBossProjectiles();

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

void GoNextMap(HWND hWnd)
{
    if (g_isChangingMap)
        return;

    g_isChangingMap = true;

    StopMove();
    isAbsorb = false;
    isSpace = false;
    isSpaceRelease = false;
    isCrouch = false;
    balloonTick = 0;
    spaceKeyHeld = false;

    if (g_currentStage == 1)
    {
        // 능력 상태는 건드리지 않고 2스테이지로 이동
        g_currentStage = 2;
        kirbyX = 70;
        kirbyY = 330;
        kirbyVY = 0.0f;
        cameraX = 0;
        InitRescueObjects();
        InitMonsters();
        UpdateCamera(hWnd);
        g_isChangingMap = false;
        return;
    }

    if (g_currentStage == 2)
    {
        // 2스테이지 문에 들어가면 3스테이지로 이동
        g_currentStage = 3;
        kirbyX = 145;
        kirbyY = 500;
        kirbyVY = 0.0f;
        cameraX = 0;
        InitRescueObjects();
        InitMonsters();
        UpdateCamera(hWnd);
        g_isChangingMap = false;
        return;
    }

    if (g_currentStage == 3)
    {
        // 3스테이지 문에 들어가면 마지막 4스테이지/보스전으로 이동
        g_currentStage = 4;
        kirbyX = 80;
        kirbyY = 480;
        kirbyVY = 0.0f;
        cameraX = 0;
        InitRescueObjects();
        InitMonsters();
        UpdateCamera(hWnd);
        g_isChangingMap = false;
        return;
    }

    if (g_currentStage == 4)
    {
        g_isChangingMap = false;
    }
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

    int shakeX = (g_rescueAnimTick / 4) % 2 == 0 ? -1 : 1;
    DrawWorldImage(graphics, childFrame, child.x + shakeX, child.y, child.w, child.h);
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
    if (powerProjectileX + powerProjectileW < 0 || powerProjectileX > GetCurrentWorldW())
    {
        isPowerProjectileActive = false;
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

void UpdateCamera(HWND hWnd)
{
    RECT rt;
    GetClientRect(hWnd, &rt);

    int screenW = rt.right - rt.left;

    // 커비 중심이 화면 가운데쯤 오도록 카메라 이동
    cameraX = kirbyX + kirbyW / 2 - screenW / 2;

    if (cameraX < 0)
        cameraX = 0;

    int currentWorldW = GetCurrentWorldW();
    int maxCameraX = currentWorldW - screenW;

    if (maxCameraX < 0)
        maxCameraX = 0;

    if (cameraX > maxCameraX)
        cameraX = maxCameraX;
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

        frameIndex++;

        if (frameIndex >= frameCount)
        {
            frameIndex = 0;
        }
    }

    void Draw(Graphics& graphics);
};

const int MONSTER_COUNT = 5; // 2스테이지에서는 폭탄병까지 사용
Monster g_monsters[MONSTER_COUNT];

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


// =========================
// 4스테이지 보스전
// 95: 기본/이동, 93: 미사일 공격 자세, 96: 미사일
// 94: 대각선 돌진, 99: 2페이즈 기본, 98: 2페이즈 상단 폭탄 자세, 97: 입 폭탄
// 100/101: 보스맵 입장 후 계속 위에서 떨어지는 공격
// =========================
const int BOSS_MAX_HP = 650; // 보스 체력 증가
const int BOSS_W = 72;   // 커비 기본 크기 48의 1.5배
const int BOSS_H = 72;   // 커비 기본 크기 48의 1.5배
const int BOSS_PHASE2_W = 200; // 2페이즈 99번 모습을 더 크게 표시
const int BOSS_PHASE2_H = 95;
const int BOSS_GROUND_Y = 545 - BOSS_H;
const int BOSS_PHASE2_GROUND_Y = 545 - BOSS_PHASE2_H;
const int BOSS_TOP_Y = 65;
const float BOSS_DASH_GRAVITY = 0.42f;

enum BossState
{
    BOSS_STATE_IDLE = 0,
    BOSS_STATE_MISSILE = 1,
    BOSS_STATE_DASH = 2,
    BOSS_STATE_TOP_BOMB = 3,
    BOSS_STATE_RISE_TOP = 4,
    BOSS_STATE_DESCEND = 5,
    BOSS_STATE_FAST_DASH = 6
};

struct BossObject
{
    bool active;
    bool phase2;
    int hp;
    int x;
    int y;
    int w;
    int h;
    int dir;
    int state;
    int actionTick;
    int missileCooldown;
    int dashCooldown;
    int topBombCooldown;
    int fastDashCooldown;
    int hitCooldown;
    int redFlashTick;       // 피격 시 아주 짧게 빨간색 표시
    int dangerTextTick;     // 패턴 시작 경고 문구
    float vx;
    float vy;
};

BossObject g_boss;

const int BOSS_PROJECTILE_MAX = 80;
struct BossProjectile
{
    bool active;
    int type; // 0=96 미사일, 1=97 입 폭탄, 2=100 상시 낙하 공격, 3=101 상시 낙하 폭탄, 4/7/8/105=파란 공, 5/6/9/104=빨간 공, 10/11=패턴 낙하, 12=바닥 절반 경고, 13=바닥 절반 폭발
    int tick;
    int x;
    int y;
    int w;
    int h;
    float vx;
    float vy;
};

BossProjectile g_bossProjectiles[BOSS_PROJECTILE_MAX];
int g_bossRainAttackCooldown = 0;
int g_bossRainBombCooldown = 0;
int g_bossSideBallCooldown = 0;
int g_bossSpreadShotCooldown = 0; // 보스가 여러 방향으로 공 발사
int g_bossGroundWaveCooldown = 0; // 바닥을 타고 오는 공
int g_bossRainBurstCooldown = 0;  // 2페이즈 연속 낙하 공격
int g_bossAimedShotCooldown = 0;  // 커비 위치를 보고 조준탄
int g_bossWallRainCooldown = 0;   // 안전구역 하나 남기고 떨어지는 장벽 낙하
int g_bossZigzagCooldown = 0;     // 위아래로 흔들리는 탄
int g_bossBounceCooldown = 0;     // 바닥에 튕기는 탄
int g_bossHalfFloorCooldown = 0;  // 바닥 절반 폭발 패턴

// 보스전 연출용 상태
bool g_bossIntro = false;
int g_bossIntroTick = 0;
bool g_bossPhase2Transition = false;
int g_bossPhase2TransitionTick = 0;
bool g_bossDeadEffect = false;
int g_bossDeadEffectTick = 0;
bool g_bossClear = false;
int g_screenShakeTick = 0;

// 보스 처치 후 보상/문 연출
bool g_rewardStarted = false;
bool g_rewardChestActive = false;
bool g_rewardChestLanded = false;
bool g_rewardChestOpened = false;
bool g_rewardKeyVisible = false;
bool g_rewardKeyTaken = false;
bool g_rewardDoorActive = false;
bool g_rewardDoorOpening = false;
bool g_rewardDoorOpened = false;

int g_rewardChestX = 0;
int g_rewardChestY = 0;
int g_rewardChestW = 72;
int g_rewardChestH = 72;
int g_rewardChestTargetY = 0;

int g_rewardKeyX = 0;
int g_rewardKeyY = 0;
int g_rewardKeyW = 38;
int g_rewardKeyH = 38;
int g_rewardKeyTargetY = 0;

int g_rewardDoorX = 0;
int g_rewardDoorY = 0;
int g_rewardDoorW = 86;
int g_rewardDoorH = 116;
int g_rewardDoorFrameIndex = 0;
int g_rewardDoorFrameTick = 0;

const int BOSS_WARNING_MAX = 24;
struct BossWarning
{
    bool active;
    int type; // 2=100 낙하 공격, 3=101 낙하 폭탄, 4=2페이즈 가로 공
    int x;
    int y;
    int w;
    int h;
    int tick;
    int dir;
};

BossWarning g_bossWarnings[BOSS_WARNING_MAX];

int RandomRange(int minValue, int maxValue)
{
    if (maxValue <= minValue)
        return minValue;

    return minValue + rand() % (maxValue - minValue + 1);
}

RECT GetBossRect()
{
    RECT rc;
    rc.left = g_boss.x;
    rc.top = g_boss.y;
    rc.right = g_boss.x + g_boss.w;
    rc.bottom = g_boss.y + g_boss.h;
    return rc;
}

void ResetBossProjectiles()
{
    for (int i = 0; i < BOSS_PROJECTILE_MAX; i++)
    {
        g_bossProjectiles[i].active = false;
        g_bossProjectiles[i].type = 0;
        g_bossProjectiles[i].tick = 0;
        g_bossProjectiles[i].x = 0;
        g_bossProjectiles[i].y = 0;
        g_bossProjectiles[i].w = 0;
        g_bossProjectiles[i].h = 0;
        g_bossProjectiles[i].vx = 0.0f;
        g_bossProjectiles[i].vy = 0.0f;
    }
}

void ResetBossWarnings()
{
    for (int i = 0; i < BOSS_WARNING_MAX; i++)
    {
        g_bossWarnings[i].active = false;
        g_bossWarnings[i].type = 0;
        g_bossWarnings[i].x = 0;
        g_bossWarnings[i].y = 0;
        g_bossWarnings[i].w = 0;
        g_bossWarnings[i].h = 0;
        g_bossWarnings[i].tick = 0;
        g_bossWarnings[i].dir = 0;
    }
}

void FaceBossToKirby()
{
    int kirbyCenterX = kirbyX + kirbyW / 2;
    int bossCenterX = g_boss.x + g_boss.w / 2;

    if (kirbyCenterX < bossCenterX)
        g_boss.dir = -1;
    else
        g_boss.dir = 1;
}

int GetBossGroundY()
{
    if (g_boss.phase2)
        return BOSS_PHASE2_GROUND_Y;

    return BOSS_GROUND_Y;
}

void StartBossPhase2()
{
    if (g_boss.phase2)
        return;

    int oldBottom = g_boss.y + g_boss.h;

    g_boss.phase2 = true;
    g_boss.w = BOSS_PHASE2_W;
    g_boss.h = BOSS_PHASE2_H;
    g_boss.y = oldBottom - g_boss.h;

    if (g_boss.y > BOSS_PHASE2_GROUND_Y)
        g_boss.y = BOSS_PHASE2_GROUND_Y;

    if (g_boss.x + g_boss.w > BG_PART_W - 70)
        g_boss.x = BG_PART_W - 70 - g_boss.w;

    // 2페이즈 전환 연출: 공격을 잠깐 멈추고 화면 흔들림/변신 느낌을 줌
    ResetBossProjectiles();
    ResetBossWarnings();
    g_bossPhase2Transition = true;
    g_bossPhase2TransitionTick = 45;
    g_screenShakeTick = 25;

    // 보스 패턴 간격 조절: 숫자가 클수록 다음 패턴까지 더 오래 기다림
    g_boss.fastDashCooldown = RandomRange(95, 150);
    g_boss.dangerTextTick = 50;
    g_boss.topBombCooldown = 130;
    g_bossSideBallCooldown = 70;
    g_bossSpreadShotCooldown = 120;
    g_bossGroundWaveCooldown = 145;
    g_bossRainBurstCooldown = 180;
    g_bossAimedShotCooldown = 85;
    g_bossWallRainCooldown = 165;
    g_bossZigzagCooldown = 120;
    g_bossBounceCooldown = 150;
    g_bossHalfFloorCooldown = 180;

    g_boss.state = BOSS_STATE_IDLE;
    g_boss.y = BOSS_PHASE2_GROUND_Y;
    g_boss.vx = (float)(2 * g_boss.dir);
    g_boss.vy = 0.0f;
}

void InitBossObjects()
{
    ResetBossProjectiles();
    ResetBossWarnings();

    g_bossIntro = true;
    g_bossIntroTick = 0;
    g_bossPhase2Transition = false;
    g_bossPhase2TransitionTick = 0;
    g_bossDeadEffect = false;
    g_bossDeadEffectTick = 0;
    g_bossClear = false;
    g_screenShakeTick = 0;

    g_rewardStarted = false;
    g_rewardChestActive = false;
    g_rewardChestLanded = false;
    g_rewardChestOpened = false;
    g_rewardKeyVisible = false;
    g_rewardKeyTaken = false;
    g_rewardDoorActive = false;
    g_rewardDoorOpening = false;
    g_rewardDoorOpened = false;
    g_rewardDoorFrameIndex = 0;
    g_rewardDoorFrameTick = 0;

    g_boss.active = true;
    g_boss.phase2 = false;
    g_boss.hp = BOSS_MAX_HP;
    g_boss.w = BOSS_W;
    g_boss.h = BOSS_H;
    g_boss.x = 820;
    g_boss.y = -BOSS_H;
    g_boss.dir = -1;
    g_boss.state = BOSS_STATE_IDLE;
    g_boss.actionTick = 0;
    // 보스 기본 패턴 시작 간격 조절: 숫자가 클수록 공격을 늦게 시작함
    g_boss.missileCooldown = 85;
    g_boss.dashCooldown = 170;
    g_boss.topBombCooldown = 220;
    g_boss.fastDashCooldown = 170;
    g_boss.hitCooldown = 0;
    g_boss.redFlashTick = 0;
    g_boss.dangerTextTick = 0;
    g_boss.vx = -2.0f;
    g_boss.vy = 0.0f;

    // 보스 투사체 패턴 시작 간격 조절: 숫자가 클수록 처음 패턴이 늦게 나옴
    g_bossRainAttackCooldown = 35;
    g_bossRainBombCooldown = 60;
    g_bossSideBallCooldown = 100;
    g_bossSpreadShotCooldown = 140;
    g_bossGroundWaveCooldown = 180;
    g_bossRainBurstCooldown = 210;
    g_bossAimedShotCooldown = 115;
    g_bossWallRainCooldown = 240;
    g_bossZigzagCooldown = 175;
    g_bossBounceCooldown = 210;
    g_bossHalfFloorCooldown = 260;
}

void SpawnBossProjectile(int type, int x, int y, int w, int h, float vx, float vy)
{
    for (int i = 0; i < BOSS_PROJECTILE_MAX; i++)
    {
        if (!g_bossProjectiles[i].active)
        {
            g_bossProjectiles[i].active = true;
            g_bossProjectiles[i].type = type;
            g_bossProjectiles[i].tick = 0;
            g_bossProjectiles[i].x = x;
            g_bossProjectiles[i].y = y;
            g_bossProjectiles[i].w = w;
            g_bossProjectiles[i].h = h;
            g_bossProjectiles[i].vx = vx;
            g_bossProjectiles[i].vy = vy;
            return;
        }
    }
}

void SpawnBossWarning(int type, int x, int y, int w, int h, int tick, int dir)
{
    for (int i = 0; i < BOSS_WARNING_MAX; i++)
    {
        if (!g_bossWarnings[i].active)
        {
            g_bossWarnings[i].active = true;
            g_bossWarnings[i].type = type;
            g_bossWarnings[i].x = x;
            g_bossWarnings[i].y = y;
            g_bossWarnings[i].w = w;
            g_bossWarnings[i].h = h;
            g_bossWarnings[i].tick = tick;
            g_bossWarnings[i].dir = dir;
            return;
        }
    }
}

void UpdateBossWarnings()
{
    for (int i = 0; i < BOSS_WARNING_MAX; i++)
    {
        if (!g_bossWarnings[i].active)
            continue;

        g_bossWarnings[i].tick--;

        if (g_bossWarnings[i].tick > 0)
            continue;

        int type = g_bossWarnings[i].type;
        int x = g_bossWarnings[i].x;
        int y = g_bossWarnings[i].y;
        int w = g_bossWarnings[i].w;
        int h = g_bossWarnings[i].h;
        int dir = g_bossWarnings[i].dir;

        g_bossWarnings[i].active = false;

        if (type == 2)
            SpawnBossProjectile(2, x, -40, 26, 26, 0.0f, g_boss.phase2 ? 9.0f : 6.0f);
        else if (type == 3)
            SpawnBossProjectile(3, x, -45, 29, 29, 0.0f, g_boss.phase2 ? 7.8f : 5.2f);
        else if (type == 10)
            SpawnBossProjectile(10, x, -45, 34, 34, 0.0f, g_boss.phase2 ? 8.4f : 5.6f);
        else if (type == 11)
            SpawnBossProjectile(11, x, -45, 34, 34, 0.0f, g_boss.phase2 ? 8.4f : 5.6f);
    }
}

void SpawnBossMissile()
{
    if (!g_boss.active)
        return;

    int missileW = kirbyW * 2;
    int missileH = kirbyH * 2;

    FaceBossToKirby();

    int missileX;
    if (g_boss.dir < 0)
        missileX = g_boss.x - missileW + 8;
    else
        missileX = g_boss.x + g_boss.w - 8;

    int missileY = g_boss.y + g_boss.h / 2 - missileH / 2;

    SpawnBossProjectile(0, missileX, missileY, missileW, missileH, 9.0f * g_boss.dir, 0.0f);
}

void SpawnBossRainAttack()
{
    // 바로 떨어지지 않고 바닥에 경고 표시 후 100번 낙하 공격 생성
    int x = RandomRange(30, BG_PART_W - 60);
    SpawnBossWarning(2, x, 545 - 18, 26, 18, 16, 0);
}

void SpawnBossRainBomb()
{
    // 바로 떨어지지 않고 바닥에 경고 표시 후 101번 낙하 폭탄 생성
    int x = RandomRange(30, BG_PART_W - 60);
    SpawnBossWarning(3, x, 545 - 20, 29, 20, 18, 0);
}

void SpawnBossSideBall()
{
    // 가로 위험 표시는 없애고, 공만 바로 빠르게 지나가게 함
    int ballSize = 34;
    int y = RandomRange(205, 430);
    int dir = RandomRange(0, 1) == 0 ? 1 : -1;
    int x = (dir > 0) ? -ballSize : BG_PART_W;

    SpawnBossProjectile(4, x, y, ballSize, ballSize, 9.0f * dir, 0.0f);
}

void SpawnBossSpreadShot()
{
    // 보스 중심에서 여러 방향으로 퍼지는 탄. 1페이즈는 보통 속도, 2페이즈만 1.5배 빠르게.
    if (!g_boss.active)
        return;

    int size = 28;
    int sx = g_boss.x + g_boss.w / 2 - size / 2;
    int sy = g_boss.y + g_boss.h / 2 - size / 2;

    int dirToKirby = (kirbyX + kirbyW / 2 < g_boss.x + g_boss.w / 2) ? -1 : 1;
    float mainSpeed = g_boss.phase2 ? 9.0f : 6.0f;
    float subSpeed = g_boss.phase2 ? 6.5f : 4.2f;

    SpawnBossProjectile(5, sx, sy, size, size, mainSpeed * dirToKirby, -3.0f);
    SpawnBossProjectile(5, sx, sy, size, size, mainSpeed * dirToKirby, 0.0f);
    SpawnBossProjectile(5, sx, sy, size, size, mainSpeed * dirToKirby, 3.0f);

    if (g_boss.phase2)
    {
        SpawnBossProjectile(5, sx, sy, size, size, subSpeed * dirToKirby, -5.0f);
        SpawnBossProjectile(5, sx, sy, size, size, subSpeed * dirToKirby, 5.0f);
    }
}

void SpawnBossGroundWave()
{
    // 바닥을 타고 좌우로 퍼지는 공격. 점프로 피하게 만드는 패턴.
    if (!g_boss.active)
        return;

    int size = 40;
    int y = GetBossGroundY() + g_boss.h - size + 2;
    int leftX = g_boss.x - size;
    int rightX = g_boss.x + g_boss.w;

    SpawnBossProjectile(6, leftX, y, size, size, -8.5f, 0.0f);
    SpawnBossProjectile(6, rightX, y, size, size, 8.5f, 0.0f);
}

void SpawnBossRainBurst()
{
    // 2페이즈 연속 낙하 패턴. 세로 경고 표시는 유지함.
    for (int i = 0; i < 5; i++)
    {
        int x = RandomRange(35, BG_PART_W - 70);
        int delay = 10 + i * 5;
        int type = (i % 2 == 0) ? 10 : 11;
        int w = 34;
        int h = 20;
        SpawnBossWarning(type, x, 545 - h, w, h, delay, 0);
    }
}

void SpawnBossAimedShot()
{
    // 커비 위치를 보고 날아오는 조준탄. 2페이즈에서만 빠르게.
    if (!g_boss.active)
        return;

    int size = 30;
    int sx = g_boss.x + g_boss.w / 2 - size / 2;
    int sy = g_boss.y + g_boss.h / 2 - size / 2;

    int targetX = kirbyX + kirbyW / 2;
    int targetY = kirbyY + kirbyH / 2;

    int dx = targetX - sx;
    int dy = targetY - sy;
    int adx = dx < 0 ? -dx : dx;
    int ady = dy < 0 ? -dy : dy;

    float speed = g_boss.phase2 ? 9.0f : 6.0f;
    float vx = 0.0f;
    float vy = 0.0f;

    if (adx >= ady)
    {
        if (adx == 0) adx = 1;
        vx = (dx < 0 ? -speed : speed);
        vy = (float)dy * speed / (float)adx;
    }
    else
    {
        if (ady == 0) ady = 1;
        vy = (dy < 0 ? -speed : speed);
        vx = (float)dx * speed / (float)ady;
    }

    SpawnBossProjectile(7, sx, sy, size, size, vx, vy);
}

void SpawnBossWallRain()
{
    // 여러 줄이 떨어지지만 커비 근처 한 칸은 안전구역으로 남기는 패턴
    int gapCenter = kirbyX + kirbyW / 2;
    int gapW = 145;

    for (int x = 45; x < BG_PART_W - 45; x += 90)
    {
        if (x > gapCenter - gapW / 2 && x < gapCenter + gapW / 2)
            continue;

        int type = (x / 90) % 2 == 0 ? 10 : 11;
        int w = 34;
        int h = 20;
        SpawnBossWarning(type, x, 545 - h, w, h, 18 + (x % 3) * 3, 0);
    }
}

void SpawnBossZigzagShot()
{
    // 위아래로 흔들리는 탄. 그냥 직선탄보다 보스전 느낌이 남.
    if (!g_boss.active)
        return;

    int size = 30;
    int dir = (kirbyX + kirbyW / 2 < g_boss.x + g_boss.w / 2) ? -1 : 1;
    int sx = (dir < 0) ? g_boss.x - size + 5 : g_boss.x + g_boss.w - 5;
    int sy = g_boss.y + g_boss.h / 2 - size / 2;

    float speed = g_boss.phase2 ? 9.0f : 6.0f;
    SpawnBossProjectile(8, sx, sy, size, size, speed * dir, -3.0f);
}

void SpawnBossBounceBall()
{
    // 바닥에 한 번씩 튕기면서 오는 탄. 2페이즈 전용.
    if (!g_boss.active)
        return;

    int size = 36;
    int dir = RandomRange(0, 1) == 0 ? -1 : 1;
    int x = (dir > 0) ? 10 : BG_PART_W - size - 10;
    int y = 260;

    SpawnBossProjectile(9, x, y, size, size, 7.8f * dir, -6.0f);
}

void SpawnBossHalfFloorAttack()
{
    // 바닥 106/107 폭발 패턴 제거
    return;
}

void SpawnBossMouthBomb()
{
    if (!g_boss.active)
        return;

    int bombW = 54;
    int bombH = 54;
    int bombX = g_boss.x + g_boss.w / 2 - bombW / 2;
    int bombY = g_boss.y + g_boss.h - 14;

    // 2페이즈 보스가 위에서 폭탄을 떨어뜨리는 패턴
    SpawnBossProjectile(1, bombX, bombY, bombW, bombH, 0.0f, 8.7f);
}

void DamageBoss(int damage)
{
    if (g_currentStage != 4)
        return;

    if (!g_boss.active)
        return;

    if (damage <= 0)
        return;

    g_boss.hp -= damage;
    g_boss.hitCooldown = 10;
    g_boss.redFlashTick = 6;
    g_boss.dangerTextTick = 10;
    g_screenShakeTick = 7;

    if (g_boss.hp <= BOSS_MAX_HP / 2)
        StartBossPhase2();

    if (g_boss.hp <= 0)
    {
        g_boss.hp = 0;
        g_boss.active = false;
        g_bossClear = true;
        g_bossDeadEffect = true;
        g_bossDeadEffectTick = 80;
        g_screenShakeTick = 35;
        ResetBossProjectiles();
        ResetBossWarnings();
    }
}

void CheckKirbyAttacksHitBoss()
{
    if (g_currentStage != 4)
        return;

    if (!g_boss.active)
        return;

    if (g_boss.hitCooldown > 0)
        return;

    RECT bossRc = GetBossRect();

    if (isPowerProjectileActive)
    {
        RECT powerRc = GetPowerProjectileSweepRect();
        if (IsRectHit(powerRc, bossRc))
        {
            isPowerProjectileActive = false;
            DamageBoss(35);
            return;
        }
    }

    if (isFireBallActive)
    {
        RECT fireBallRc = GetFireBallSweepRect();
        if (IsRectHit(fireBallRc, bossRc))
        {
            isFireBallActive = false;
            DamageBoss(8);
            return;
        }
    }

    if (isFireBreath)
    {
        RECT breathRc = GetFireBreathRect();
        if (IsRectHit(breathRc, bossRc))
        {
            DamageBoss(14);
            return;
        }
    }

    for (int i = 0; i < BOMB_OBJECT_MAX; i++)
    {
        if (!g_bombs[i].active || g_bombs[i].fromEnemy)
            continue;

        RECT bombRc;
        bombRc.left = g_bombs[i].x;
        bombRc.top = g_bombs[i].y;
        bombRc.right = g_bombs[i].x + g_bombs[i].w;
        bombRc.bottom = g_bombs[i].y + g_bombs[i].h;

        if (IsRectHit(bombRc, bossRc))
        {
            SpawnBombExplosion(g_bombs[i].x + g_bombs[i].w / 2, g_bombs[i].y + g_bombs[i].h / 2);
            g_bombs[i].active = false;
            DamageBoss(g_bombs[i].damage);
            return;
        }
    }
}

void CheckBossBodyHitKirby()
{
    if (!g_boss.active)
        return;

    // 빨아들이기 중 보스/적이 끌려와서 닿는 판정은 데미지로 처리하지 않음
    if (isAbsorb)
        return;

    if (isKirbyHit || kirbyHitCooldownTick > 0)
        return;

    RECT kirbyRc = GetKirbyBodyRect();
    RECT bossRc = GetBossRect();

    if (IsRectHit(kirbyRc, bossRc))
    {
        StartKirbyHitEffect();
    }
}

void UpdateBossProjectiles()
{
    RECT kirbyRc = GetKirbyBodyRect();

    for (int i = 0; i < BOSS_PROJECTILE_MAX; i++)
    {
        if (!g_bossProjectiles[i].active)
            continue;

        if (g_bossProjectiles[i].type == 12 || g_bossProjectiles[i].type == 13)
        {
            g_bossProjectiles[i].active = false;
            continue;
        }

        g_bossProjectiles[i].tick++;

        if (g_bossProjectiles[i].type == 12)
        {
            // 106번 바닥 절반 경고: 약 1초 뒤 107번 폭발로 변경
            if (g_bossProjectiles[i].tick >= 25)
            {
                g_bossProjectiles[i].type = 13;
                g_bossProjectiles[i].tick = 0;
                g_screenShakeTick = 8;
            }
        }

        if (g_bossProjectiles[i].type == 13)
        {
            // 107번 폭발은 잠깐만 유지
            if (g_bossProjectiles[i].tick >= 14)
            {
                g_bossProjectiles[i].active = false;
                continue;
            }
        }

        if (g_bossProjectiles[i].type == 8)
        {
            // 지그재그탄은 위아래로 흔들림
            if ((g_bossProjectiles[i].tick / 8) % 2 == 0)
                g_bossProjectiles[i].vy = g_boss.phase2 ? 4.5f : 3.0f;
            else
                g_bossProjectiles[i].vy = g_boss.phase2 ? -4.5f : -3.0f;
        }

        if (g_bossProjectiles[i].type == 9)
        {
            // 바운스탄은 중력으로 떨어졌다가 바닥에서 튕김
            g_bossProjectiles[i].vy += 0.45f;
        }

        g_bossProjectiles[i].x += (int)g_bossProjectiles[i].vx;
        g_bossProjectiles[i].y += (int)g_bossProjectiles[i].vy;

        if (g_bossProjectiles[i].type == 9)
        {
            int groundY = 545 - g_bossProjectiles[i].h;
            if (g_bossProjectiles[i].y > groundY)
            {
                g_bossProjectiles[i].y = groundY;
                g_bossProjectiles[i].vy = -7.2f;
            }
        }

        RECT rc;
        rc.left = g_bossProjectiles[i].x;
        rc.top = g_bossProjectiles[i].y;
        rc.right = g_bossProjectiles[i].x + g_bossProjectiles[i].w;
        rc.bottom = g_bossProjectiles[i].y + g_bossProjectiles[i].h;

        // 106번 경고는 데미지 없음. 107번 폭발은 판정 있음.
        if (g_bossProjectiles[i].type != 12 &&
            !isKirbyHit && kirbyHitCooldownTick <= 0 && IsRectHit(kirbyRc, rc))
        {
            int pType = g_bossProjectiles[i].type;

            if (pType == 4 || pType == 7 || pType == 8 || pType == 11)
                StartKirbySlow(); // 파란 공: 이동속도 감소

            if (pType == 5 || pType == 6 || pType == 9 || pType == 10 || pType == 13)
                StartKirbyBurn(); // 빨간 공/폭발: 잠깐 지속피해

            if (pType != 13)
                g_bossProjectiles[i].active = false;

            StartKirbyHitEffect();
            continue;
        }

        if (g_bossProjectiles[i].x + g_bossProjectiles[i].w < -100 ||
            g_bossProjectiles[i].x > BG_PART_W + 100 ||
            g_bossProjectiles[i].y > WORLD_H + 120)
        {
            g_bossProjectiles[i].active = false;
        }
    }
}

void StartBossRewardObjects();
void UpdateBossRewardObjects();
void TryBossRewardInteraction();
void DrawBossRewardObjects(Graphics& graphics);

void UpdateBossObjects()
{
    if (g_currentStage != 4)
        return;

    if (g_screenShakeTick > 0)
        g_screenShakeTick--;

    if (g_bossDeadEffect)
    {
        g_bossDeadEffectTick--;
        if (g_bossDeadEffectTick <= 0)
        {
            g_bossDeadEffect = false;
            g_bossDeadEffectTick = 0;
            StartBossRewardObjects();
        }

        ResetBossProjectiles();
        ResetBossWarnings();
        return;
    }

    // 보스가 죽으면 위에서 떨어지는 공격/가로 공도 전부 사라지고 더 이상 생성되지 않음
    if (!g_boss.active)
    {
        UpdateBossRewardObjects();
        ResetBossProjectiles();
        ResetBossWarnings();
        return;
    }

    if (g_bossIntro)
    {
        int groundY = GetBossGroundY();
        g_bossIntroTick++;

        if (g_boss.y < groundY)
        {
            g_boss.y += 6;
            if (g_boss.y > groundY)
                g_boss.y = groundY;
        }

        if (g_boss.y >= groundY && g_bossIntroTick > 45)
        {
            g_bossIntro = false;
            g_screenShakeTick = 12;
        }

        return;
    }

    if (g_bossPhase2Transition)
    {
        g_bossPhase2TransitionTick--;
        g_screenShakeTick = 4;

        if (g_bossPhase2TransitionTick <= 0)
        {
            g_bossPhase2Transition = false;
            g_bossPhase2TransitionTick = 0;
        }

        return;
    }

    UpdateBossWarnings();

    // 4스테이지에 들어온 순간부터 100번/101번 낙하 공격은 계속 떨어짐
    g_bossRainAttackCooldown--;
    if (g_bossRainAttackCooldown <= 0)
    {
        SpawnBossRainAttack();
        g_bossRainAttackCooldown = RandomRange(35, 60); // 숫자가 클수록 하늘 공격 간격 증가
    }

    g_bossRainBombCooldown--;
    if (g_bossRainBombCooldown <= 0)
    {
        SpawnBossRainBomb();
        g_bossRainBombCooldown = RandomRange(55, 85); // 숫자가 클수록 폭탄 낙하 간격 증가
    }

    // 2페이즈부터는 세로 낙하 공격 말고 가로로 지나가는 공도 추가
    if (g_boss.phase2)
    {
        g_bossSideBallCooldown--;
        if (g_bossSideBallCooldown <= 0)
        {
            SpawnBossSideBall();
            g_bossSideBallCooldown = RandomRange(55, 85); // 숫자가 클수록 가로 공 간격 증가
        }

        g_bossRainBurstCooldown--;
        if (g_bossRainBurstCooldown <= 0)
        {
            SpawnBossRainBurst();
            g_boss.dangerTextTick = 28;
            g_bossRainBurstCooldown = RandomRange(180, 260);
        }
    }

    g_bossSpreadShotCooldown--;
    if (g_bossSpreadShotCooldown <= 0)
    {
        SpawnBossSpreadShot();
        g_boss.dangerTextTick = 22;
        g_bossSpreadShotCooldown = g_boss.phase2 ? RandomRange(130, 190) : RandomRange(180, 240);
    }

    g_bossAimedShotCooldown--;
    if (g_bossAimedShotCooldown <= 0)
    {
        SpawnBossAimedShot();
        g_boss.dangerTextTick = 20;
        g_bossAimedShotCooldown = g_boss.phase2 ? RandomRange(95, 140) : RandomRange(150, 210);
    }

    g_bossZigzagCooldown--;
    if (g_bossZigzagCooldown <= 0)
    {
        SpawnBossZigzagShot();
        g_boss.dangerTextTick = 20;
        g_bossZigzagCooldown = g_boss.phase2 ? RandomRange(110, 160) : RandomRange(180, 240);
    }

    if (g_boss.phase2)
    {
        g_bossGroundWaveCooldown--;
        if (g_bossGroundWaveCooldown <= 0)
        {
            SpawnBossGroundWave();
            g_boss.dangerTextTick = 24;
            g_bossGroundWaveCooldown = RandomRange(170, 240);
        }

        g_bossWallRainCooldown--;
        if (g_bossWallRainCooldown <= 0)
        {
            SpawnBossWallRain();
            g_boss.dangerTextTick = 26;
            g_bossWallRainCooldown = RandomRange(200, 280);
        }

        g_bossBounceCooldown--;
        if (g_bossBounceCooldown <= 0)
        {
            SpawnBossBounceBall();
            g_boss.dangerTextTick = 22;
            g_bossBounceCooldown = RandomRange(160, 230);
        }

        // 106/107 바닥 절반 폭발 패턴은 어색해서 제거함.
        // g_bossHalfFloorCooldown은 더 이상 사용하지 않음.
    }

    UpdateBossProjectiles();

    if (g_boss.hitCooldown > 0)
        g_boss.hitCooldown--;

    if (g_boss.redFlashTick > 0)
        g_boss.redFlashTick--;

    if (g_boss.dangerTextTick > 0)
        g_boss.dangerTextTick--;

    if (g_boss.hp <= BOSS_MAX_HP / 2)
        StartBossPhase2();

    if (g_boss.state == BOSS_STATE_MISSILE)
    {
        g_boss.actionTick--;

        if (g_boss.actionTick == 14)
            SpawnBossMissile();

        if (g_boss.actionTick <= 0)
        {
            g_boss.state = BOSS_STATE_IDLE;
            g_boss.missileCooldown = g_boss.phase2 ? 100 : 140;
        }

        CheckBossBodyHitKirby();
        return;
    }

    if (g_boss.state == BOSS_STATE_DASH)
    {
        int groundY = GetBossGroundY();

        // 대각선 돌진은 위로 뛰어오른 뒤 포물선처럼 내려오게 함.
        // 예전처럼 시간 끝나자마자 바닥으로 순간이동하지 않고, 바닥에 닿을 때 끝남.
        g_boss.x += (int)g_boss.vx;
        g_boss.y += (int)g_boss.vy;
        g_boss.vy += BOSS_DASH_GRAVITY;

        if (g_boss.x < 40)
        {
            g_boss.x = 40;
            g_boss.vx = -g_boss.vx;
            g_boss.dir = 1;
        }

        if (g_boss.x + g_boss.w > BG_PART_W - 40)
        {
            g_boss.x = BG_PART_W - 40 - g_boss.w;
            g_boss.vx = -g_boss.vx;
            g_boss.dir = -1;
        }

        CheckBossBodyHitKirby();

        if (g_boss.y >= groundY && g_boss.vy > 0.0f)
        {
            g_boss.y = groundY;
            g_boss.state = BOSS_STATE_IDLE;
            g_boss.vx = (float)(2 * g_boss.dir);
            g_boss.vy = 0.0f;
            g_boss.dashCooldown = g_boss.phase2 ? 150 : 210;
            return;
        }

        return;
    }

    if (g_boss.state == BOSS_STATE_RISE_TOP)
    {
        // 갑자기 순간이동하지 않고 천천히 위로 올라감
        if (g_boss.y > BOSS_TOP_Y)
        {
            g_boss.y -= 4;
            if (g_boss.y < BOSS_TOP_Y)
                g_boss.y = BOSS_TOP_Y;
        }
        else
        {
            g_boss.y = BOSS_TOP_Y;
            g_boss.state = BOSS_STATE_TOP_BOMB;
            g_boss.actionTick = 95;
            g_boss.vx = (g_boss.x < BG_PART_W / 2) ? 3.0f : -3.0f;
        }

        CheckBossBodyHitKirby();
        return;
    }

    if (g_boss.state == BOSS_STATE_DESCEND)
    {
        int groundY = GetBossGroundY();

        // 마지막 97번 폭탄을 떨어뜨린 뒤 바로 내려오면 폭탄이랑 같이 내려오는 것처럼 보여서
        // 약 0.5초 정도 위에서 멈췄다가 내려오게 함. GAME_TIMER_MS가 40ms라 13틱이면 약 0.52초.
        if (g_boss.actionTick > 0)
        {
            g_boss.actionTick--;
            CheckBossBodyHitKirby();
            return;
        }

        g_boss.y += 5;
        if (g_boss.y >= groundY)
        {
            g_boss.y = groundY;
            g_boss.state = BOSS_STATE_IDLE;
            g_boss.topBombCooldown = RandomRange(190, 260);
        }

        CheckBossBodyHitKirby();
        return;
    }

    if (g_boss.state == BOSS_STATE_FAST_DASH)
    {
        // 2페이즈 전용: 바닥에서 매우 빠르게 좌우로 튕기듯 이동
        int groundY = GetBossGroundY();
        g_boss.y = groundY;
        g_boss.x += (int)g_boss.vx;

        if (g_boss.x < 35)
        {
            g_boss.x = 35;
            g_boss.vx = 13.0f;
            g_boss.dir = 1;
        }

        if (g_boss.x + g_boss.w > BG_PART_W - 35)
        {
            g_boss.x = BG_PART_W - 35 - g_boss.w;
            g_boss.vx = -13.0f;
            g_boss.dir = -1;
        }

        CheckBossBodyHitKirby();

        g_boss.actionTick--;
        if (g_boss.actionTick <= 0)
        {
            g_boss.state = BOSS_STATE_IDLE;
            g_boss.vx = (float)(2 * g_boss.dir);
            g_boss.fastDashCooldown = RandomRange(120, 190);
        }

        return;
    }

    if (g_boss.state == BOSS_STATE_TOP_BOMB)
    {
        // 특정 높이에 도달하면 좌우로 움직이면서 97번 폭탄을 떨어뜨림
        g_boss.actionTick--;
        g_boss.x += (int)g_boss.vx;

        if (g_boss.x < 70)
        {
            g_boss.x = 70;
            g_boss.vx = 3.0f;
            g_boss.dir = 1;
        }

        if (g_boss.x + g_boss.w > BG_PART_W - 70)
        {
            g_boss.x = BG_PART_W - 70 - g_boss.w;
            g_boss.vx = -3.0f;
            g_boss.dir = -1;
        }

        if (g_boss.actionTick % 12 == 0)
            SpawnBossMouthBomb();

        if (g_boss.actionTick <= 0)
        {
            g_boss.state = BOSS_STATE_DESCEND;
            g_boss.actionTick = 13; // 마지막 폭탄 투하 후 약 0.5초 대기
        }

        CheckBossBodyHitKirby();
        return;
    }

    // 기본 이동: 95번, 2페이즈 이후는 99번 모습
    g_boss.x += 2 * g_boss.dir;

    if (g_boss.x < 70)
    {
        g_boss.x = 70;
        g_boss.dir = 1;
    }

    if (g_boss.x + g_boss.w > BG_PART_W - 70)
    {
        g_boss.x = BG_PART_W - 70 - g_boss.w;
        g_boss.dir = -1;
    }

    if (g_boss.missileCooldown > 0)
        g_boss.missileCooldown--;

    if (g_boss.dashCooldown > 0)
        g_boss.dashCooldown--;

    if (g_boss.phase2 && g_boss.topBombCooldown > 0)
        g_boss.topBombCooldown--;

    if (g_boss.phase2 && g_boss.fastDashCooldown > 0)
        g_boss.fastDashCooldown--;

    // 2페이즈부터는 미사일 발사와 대각선 돌진 공격을 비활성화
    if (!g_boss.phase2 && g_boss.missileCooldown <= 0)
    {
        FaceBossToKirby(); // 미사일 자세부터 커비를 바라보게 함
        g_boss.state = BOSS_STATE_MISSILE;
        g_boss.actionTick = 34;
        g_boss.dangerTextTick = 34;
        g_screenShakeTick = 8;
        return;
    }

    if (!g_boss.phase2 && g_boss.dashCooldown <= 0)
    {
        FaceBossToKirby();
        g_boss.dangerTextTick = 24;
        g_screenShakeTick = 6;
        g_boss.vx = 5.6f * g_boss.dir;
        g_boss.vy = -8.2f;
        g_boss.state = BOSS_STATE_DASH;
        g_boss.actionTick = 0;
        return;
    }

    if (g_boss.phase2 && g_boss.fastDashCooldown <= 0)
    {
        // 랜덤 방향으로 매우 빠르게 좌우 이동. 몸에 닿으면 커비 데미지.
        g_boss.dangerTextTick = 24;
        g_screenShakeTick = 10;
        int dashDir = RandomRange(0, 1) == 0 ? -1 : 1;
        g_boss.dir = dashDir;
        g_boss.vx = 13.0f * dashDir;
        g_boss.vy = 0.0f;
        g_boss.y = GetBossGroundY();
        g_boss.state = BOSS_STATE_FAST_DASH;
        g_boss.actionTick = RandomRange(42, 68);
        return;
    }

    if (g_boss.phase2 && g_boss.topBombCooldown <= 0)
    {
        FaceBossToKirby();
        g_boss.dangerTextTick = 28;
        g_boss.state = BOSS_STATE_RISE_TOP;
        g_boss.vx = 0.0f;
        g_boss.vy = 0.0f;
        return;
    }

    CheckBossBodyHitKirby();
}

void DrawKirbyStatusUI(Graphics& graphics)
{
    // 감속/화상 상태는 효과만 적용하고 화면에는 표시하지 않음.
    return;
}

RECT MakeRect(int x, int y, int w, int h)
{
    RECT rc;
    rc.left = x;
    rc.top = y;
    rc.right = x + w;
    rc.bottom = y + h;
    return rc;
}

void StartBossRewardObjects()
{
    if (g_rewardStarted)
        return;

    g_rewardStarted = true;

    g_rewardChestActive = true;
    g_rewardChestLanded = false;
    g_rewardChestOpened = false;

    g_rewardChestW = 88;
    g_rewardChestH = 88;
    g_rewardChestX = BG_PART_W / 2 - g_rewardChestW / 2;
    g_rewardChestY = -g_rewardChestH - 10;
    // 113/114 프레임 아래쪽에 투명 여백이 있어 떠 보이므로 실제 표시 위치를 아래로 보정
    g_rewardChestTargetY = 545 - g_rewardChestH + 36;

    g_rewardKeyVisible = false;
    g_rewardKeyTaken = false;
    g_rewardKeyW = 44;
    g_rewardKeyH = 44;
    g_rewardKeyX = g_rewardChestX + g_rewardChestW / 2 - g_rewardKeyW / 2;
    g_rewardKeyY = g_rewardChestY + 10;
    g_rewardKeyTargetY = g_rewardChestTargetY - g_rewardKeyH + 4;

    g_rewardDoorActive = false;
    g_rewardDoorOpening = false;
    g_rewardDoorOpened = false;
    g_rewardDoorW = 150;
    g_rewardDoorH = 200;
    g_rewardDoorX = BG_PART_W - g_rewardDoorW - 28;
    g_rewardDoorY = 410; 
    g_rewardDoorFrameIndex = 0;
    g_rewardDoorFrameTick = 0;
}

bool IsKirbyNearRect(int x, int y, int w, int h)
{
    RECT kirbyRc = GetKirbyBodyRect();

    RECT rc;
    rc.left = x - 26;
    rc.top = y - 26;
    rc.right = x + w + 26;
    rc.bottom = y + h + 26;

    return IsRectHit(kirbyRc, rc);
}

void UpdateBossRewardObjects()
{
    if (!g_rewardStarted)
        return;

    if (g_rewardChestActive && !g_rewardChestLanded)
    {
        g_rewardChestY += 4;
        if (g_rewardChestY >= g_rewardChestTargetY)
        {
            g_rewardChestY = g_rewardChestTargetY;
            g_rewardChestLanded = true;
        }
    }

    if (g_rewardKeyVisible && !g_rewardKeyTaken)
    {
        g_rewardKeyX = g_rewardChestX + g_rewardChestW / 2 - g_rewardKeyW / 2;

        if (g_rewardKeyY > g_rewardKeyTargetY)
        {
            g_rewardKeyY -= 2;
            if (g_rewardKeyY < g_rewardKeyTargetY)
                g_rewardKeyY = g_rewardKeyTargetY;
        }
    }

    if (g_rewardDoorOpening)
    {
        g_rewardDoorFrameTick++;

        if (g_rewardDoorFrameTick >= 8)
        {
            g_rewardDoorFrameTick = 0;

            if (g_rewardDoorFrameIndex < 3)
                g_rewardDoorFrameIndex++;
            else
            {
                g_rewardDoorOpening = false;
                g_rewardDoorOpened = true;
                g_rewardDoorFrameIndex = 3;
            }
        }
    }
}

void TryBossRewardInteraction()
{
    if (g_currentStage != 4)
        return;

    if (!g_rewardStarted)
        return;

    if (g_rewardChestActive && g_rewardChestLanded && !g_rewardChestOpened)
    {
        if (IsKirbyNearRect(g_rewardChestX, g_rewardChestY, g_rewardChestW, g_rewardChestH))
        {
            g_rewardChestOpened = true;
            g_rewardKeyVisible = true;
            g_rewardKeyTaken = false;

            g_rewardKeyX = g_rewardChestX + g_rewardChestW / 2 - g_rewardKeyW / 2;
            g_rewardKeyY = g_rewardChestY + 20;
            g_rewardKeyTargetY = g_rewardChestY - g_rewardKeyH + 4;

            g_rewardDoorActive = true;
            g_rewardDoorFrameIndex = 0;
            return;
        }
    }

    if (g_rewardKeyVisible && !g_rewardKeyTaken)
    {
        if (IsKirbyNearRect(g_rewardKeyX, g_rewardKeyY, g_rewardKeyW, g_rewardKeyH))
        {
            g_rewardKeyTaken = true;
            g_rewardKeyVisible = false;
            return;
        }
    }

    if (g_rewardDoorActive && g_rewardKeyTaken && !g_rewardDoorOpened && !g_rewardDoorOpening)
    {
        if (IsKirbyNearRect(g_rewardDoorX, g_rewardDoorY, g_rewardDoorW, g_rewardDoorH))
        {
            g_rewardDoorOpening = true;
            g_rewardDoorFrameIndex = 0;
            g_rewardDoorFrameTick = 0;
            return;
        }
    }
}

void DrawBossRewardObjects(Graphics& graphics)
{
    if (g_currentStage != 4)
        return;

    if (!g_rewardStarted)
        return;

    if (g_rewardDoorActive)
    {
        Image* doorFrame = g_bossDoorFrames[g_rewardDoorFrameIndex];
        if (doorFrame != NULL)
            DrawWorldImage(graphics, doorFrame, g_rewardDoorX, g_rewardDoorY, g_rewardDoorW, g_rewardDoorH);
    }

    if (g_rewardChestActive)
    {
        Image* chestFrame = g_rewardChestOpened ? g_bossChestOpenFrame : g_bossChestClosedFrame;
        if (chestFrame != NULL)
            DrawWorldImage(graphics, chestFrame, g_rewardChestX, g_rewardChestY, g_rewardChestW, g_rewardChestH);
    }

    if (g_rewardKeyVisible && !g_rewardKeyTaken)
    {
        if (g_bossKeyFrame != NULL)
            DrawWorldImage(graphics, g_bossKeyFrame, g_rewardKeyX, g_rewardKeyY, g_rewardKeyW, g_rewardKeyH);
    }
}

void DrawBossHpBar(Graphics& graphics)
{
    if (g_currentStage != 4)
        return;

    FontFamily fontFamily(L"Arial");
    Font smallFont(&fontFamily, 18, FontStyleBold, UnitPixel);
    Font bigFont(&fontFamily, 32, FontStyleBold, UnitPixel);
    SolidBrush textBrush(Color(230, 255, 230, 255));
    SolidBrush clearBrush(Color(240, 255, 230, 120));
    if (!g_boss.active)
        return;

    int barX = 250;
    int barY = 24;
    int barW = 500;
    int barH = 16;

    SolidBrush backBrush(Color(180, 20, 10, 30));
    SolidBrush hpBrush(Color(220, 160, 40, 220));
    Pen borderPen(Color(230, 230, 190, 255), 2);

    graphics.FillRectangle(&backBrush, barX, barY, barW, barH);

    int hpW = barW * g_boss.hp / BOSS_MAX_HP;
    if (hpW < 0) hpW = 0;
    if (hpW > barW) hpW = barW;

    graphics.FillRectangle(&hpBrush, barX, barY, hpW, barH);
    graphics.DrawRectangle(&borderPen, barX, barY, barW, barH);
}

void DrawBossWarnings(Graphics& graphics)
{
    if (g_currentStage != 4)
        return;

    for (int i = 0; i < BOSS_WARNING_MAX; i++)
    {
        if (!g_bossWarnings[i].active)
            continue;

        int alpha = 90 + (g_bossWarnings[i].tick % 6) * 20;
        if (alpha > 210) alpha = 210;

        SolidBrush warningBrush(Color(alpha, 255, 40, 90));
        Pen warningPen(Color(230, 255, 230, 120), 2);

        // 가로 위험 표시는 제거하고, 세로 낙하 경고만 보여줌
        if (g_bossWarnings[i].type == 2 || g_bossWarnings[i].type == 3 || g_bossWarnings[i].type == 10 || g_bossWarnings[i].type == 11)
        {
            graphics.FillEllipse(&warningBrush, g_bossWarnings[i].x - 8, g_bossWarnings[i].y, g_bossWarnings[i].w + 16, g_bossWarnings[i].h);
            graphics.DrawEllipse(&warningPen, g_bossWarnings[i].x - 8, g_bossWarnings[i].y, g_bossWarnings[i].w + 16, g_bossWarnings[i].h);
        }
    }
}

void DrawBossDeathEffect(Graphics& graphics)
{
    if (!g_bossDeadEffect)
        return;

    Image* deathFrame = (g_bossDeadEffectTick % 16 < 8) ? g_bossDeathFrame1 : g_bossDeathFrame2;

    int alpha = g_bossDeadEffectTick * 255 / 80;
    if (alpha < 0) alpha = 0;
    if (alpha > 255) alpha = 255;

    int drawW = g_boss.w;
    int drawH = g_boss.h;
    int drawX = g_boss.x;
    int drawY = g_boss.y;

    if (deathFrame != NULL)
    {
        ColorMatrix colorMatrix =
        {
            1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, alpha / 255.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 1.0f
        };

        ImageAttributes attr;
        attr.SetColorMatrix(&colorMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);

        Rect dest(drawX, drawY, drawW, drawH);
        graphics.DrawImage(
            deathFrame,
            dest,
            0,
            0,
            deathFrame->GetWidth(),
            deathFrame->GetHeight(),
            UnitPixel,
            &attr
        );
    }
    else
    {
        SolidBrush fadeBrush(Color(alpha, 180, 40, 240));
        graphics.FillEllipse(&fadeBrush, drawX, drawY, drawW, drawH);
    }
}

void DrawBossShadow(Graphics& graphics)
{
    if (g_currentStage != 4 || !g_boss.active)
        return;

    int groundY = GetBossGroundY() + g_boss.h - 4;
    int shadowW = g_boss.w;
    int shadowH = 14;

    SolidBrush shadowBrush(Color(90, 0, 0, 0));
    graphics.FillEllipse(&shadowBrush, g_boss.x + g_boss.w / 2 - shadowW / 2, groundY, shadowW, shadowH);
}

void DrawBossLaserDanger(Graphics& graphics)
{
    if (g_currentStage != 4 || !g_boss.active)
        return;

    if (g_boss.state != BOSS_STATE_MISSILE)
        return;

    // 미사일/레이저 발사 방향에 빨간 경고선만 표시함. 실제 공격범위에 맞춰 2배 높이.
    int laserY = g_boss.y + g_boss.h / 2 - 36;
    int laserH = 72;

    int x1, x2;
    if (g_boss.dir < 0)
    {
        x1 = 0;
        x2 = g_boss.x;
    }
    else
    {
        x1 = g_boss.x + g_boss.w;
        x2 = BG_PART_W;
    }

    if (x2 < x1)
    {
        int temp = x1;
        x1 = x2;
        x2 = temp;
    }

    int alpha = 60 + (g_boss.actionTick % 6) * 22;
    if (alpha > 190) alpha = 190;

    SolidBrush laserBrush(Color(alpha, 255, 0, 40));
    Pen laserPen(Color(230, 255, 220, 220), 2);
    graphics.FillRectangle(&laserBrush, x1, laserY, x2 - x1, laserH);
    graphics.DrawRectangle(&laserPen, x1, laserY, x2 - x1, laserH);

}

void DrawBossPatternText(Graphics& graphics)
{
    // 조심/패턴 안내 글자는 출력하지 않음. 빨간 표시만 사용.
    return;
}

void DrawBossHitRedFlash(Graphics& graphics)
{
    if (g_currentStage != 4 || !g_boss.active)
        return;

    if (g_boss.redFlashTick <= 0)
        return;

    int alpha = 80 + g_boss.redFlashTick * 22;
    if (alpha > 220) alpha = 220;

    SolidBrush redBrush(Color(alpha, 255, 0, 0));
    graphics.FillEllipse(&redBrush, g_boss.x, g_boss.y, g_boss.w, g_boss.h);
}

void DrawNightmareParticles(Graphics& graphics)
{
    if (g_currentStage != 4)
        return;

    // 간단한 보라색 입자. 리소스 없이도 보스맵 분위기를 살림
    for (int i = 0; i < 26; i++)
    {
        int x = (i * 73 + g_bossIntroTick * 3 + g_boss.hp) % BG_PART_W;
        int y = (i * 47 + g_bossIntroTick * 5 + g_boss.phase2 * 120) % 520;

        int size = g_boss.phase2 ? 4 : 3;
        int alpha = g_boss.phase2 ? 120 : 70;

        SolidBrush pBrush(Color(alpha, 180, 80, 255));
        graphics.FillEllipse(&pBrush, x, y, size, size);
    }
}

void DrawBossProjectiles(Graphics& graphics)
{
    for (int i = 0; i < BOSS_PROJECTILE_MAX; i++)
    {
        if (!g_bossProjectiles[i].active)
            continue;

        Image* img = g_bossMissileFrame;

        if (g_bossProjectiles[i].type == 1)
            img = g_bossMouthBombFrame;
        else if (g_bossProjectiles[i].type == 2)
            img = g_bossRainAttackFrame; // 상시 낙하
        else if (g_bossProjectiles[i].type == 3)
            img = g_bossRainBombFrame;   // 상시 낙하
        else if (g_bossProjectiles[i].type == 4 || g_bossProjectiles[i].type == 7 || g_bossProjectiles[i].type == 8 || g_bossProjectiles[i].type == 11)
            img = g_bossPatternBlueBallFrame; // 파란 공: 이동속도 감소
        else if (g_bossProjectiles[i].type == 5 || g_bossProjectiles[i].type == 6 || g_bossProjectiles[i].type == 9 || g_bossProjectiles[i].type == 10)
            img = g_bossPatternRedBallFrame;  // 빨간 공: 지속피해
        else if (g_bossProjectiles[i].type == 12)
            img = g_bossHalfFloorWarnFrame;   // 바닥 절반 경고
        else if (g_bossProjectiles[i].type == 13)
            img = g_bossHalfFloorBoomFrame;   // 바닥 절반 폭발

        if (img == NULL)
            continue;

        // 96번 미사일 원본 방향이 반대라서 vx > 0일 때 좌우반전
        if (g_bossProjectiles[i].type == 0 && g_bossProjectiles[i].vx > 0)
        {
            DrawImageFlipX(
                graphics,
                img,
                g_bossProjectiles[i].x,
                g_bossProjectiles[i].y,
                g_bossProjectiles[i].w,
                g_bossProjectiles[i].h
            );
        }
        else
        {
            DrawWorldImage(
                graphics,
                img,
                g_bossProjectiles[i].x,
                g_bossProjectiles[i].y,
                g_bossProjectiles[i].w,
                g_bossProjectiles[i].h
            );
        }
    }
}

void DrawBossObjects(Graphics& graphics)
{
    if (g_currentStage != 4)
        return;

    DrawNightmareParticles(graphics);
    DrawBossShadow(graphics);
    DrawBossLaserDanger(graphics);
    DrawBossWarnings(graphics);
    DrawBossProjectiles(graphics);
    DrawBossDeathEffect(graphics);
    DrawBossRewardObjects(graphics);

    if (!g_boss.active)
        return;

    Image* bossFrame = g_bossIdleMoveFrame;

    if (g_boss.state == BOSS_STATE_MISSILE && g_bossMissilePoseFrame != NULL)
        bossFrame = g_bossMissilePoseFrame;
    else if (g_boss.state == BOSS_STATE_DASH && g_bossDashFrame != NULL)
        bossFrame = g_bossDashFrame;
    else if (g_boss.state == BOSS_STATE_TOP_BOMB && g_bossTopAttackFrame != NULL)
        bossFrame = g_bossTopAttackFrame;
    else if (g_boss.phase2 && g_bossPhase2Frame != NULL)
        bossFrame = g_bossPhase2Frame;

    if (bossFrame == NULL)
        return;

    // 피격 시 완전히 사라지는 대신 빨간 플래시를 덮어서 더 명확하게 보이게 함
    // 보스 원본 프레임 방향이 기존 코드와 반대라서 좌우반전 조건을 뒤집음
    if (g_boss.dir > 0)
        DrawImageFlipX(graphics, bossFrame, g_boss.x, g_boss.y, g_boss.w, g_boss.h);
    else
        DrawWorldImage(graphics, bossFrame, g_boss.x, g_boss.y, g_boss.w, g_boss.h);

    DrawBossHitRedFlash(graphics);
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

        DrawWorldImage(
            graphics,
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

        DrawWorldImage(
            graphics,
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

        DrawWorldImage(
            graphics,
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

    // 4스테이지 보스전 배경: 92번 프레임
    g_stage4Background = LoadPNGFromResource(g_hInst, IDB_PNG92);

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

    g_backgroundScaled = CreateScaledBitmap(g_background, BG_PART_W, BG_PART_H);
    g_background2Scaled = CreateScaledBitmap(g_background2, BG_PART_W, BG_PART_H);
    g_stage2BackgroundFrontScaled = CreateScaledBitmap(g_stage2BackgroundFront, BG_PART_W, BG_PART_H);
    g_stage2BackgroundBackScaled = CreateScaledBitmap(g_stage2BackgroundBack, BG_PART_W, BG_PART_H);
    g_stage3BackgroundFrontScaled = CreateScaledBitmap(g_stage3BackgroundFront, BG_PART_W, BG_PART_H);
    g_stage3BackgroundBackScaled = CreateScaledBitmap(g_stage3BackgroundBack, BG_PART_W, BG_PART_H);
    g_stage4BackgroundScaled = CreateScaledBitmap(g_stage4Background, BG_PART_W, BG_PART_H);

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

    if (g_stage4Background != NULL)
    {
        delete g_stage4Background;
        g_stage4Background = NULL;
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
    // 현재 스테이지에 맞는 충돌체를 보여줘야 하므로 GetCurrentSolidBlocks() 사용
    int debugBlockCount = 0;
    SolidBlock* debugBlocks = GetCurrentSolidBlocks(&debugBlockCount);

    for (int i = 0; i < debugBlockCount; i++)
    {
        RECT rc = debugBlocks[i].rc;
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

    if (!PrepareBackBuffer(hdc, rt.right, rt.bottom))
        return;

    HDC memDC = g_backDC;

    // 이전 프레임 잔상이 남지 않게 먼저 전체를 지움
    HBRUSH clearBrush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(memDC, &rt, clearBrush);
    DeleteObject(clearBrush);

    Graphics graphics(memDC);
    graphics.SetCompositingQuality(CompositingQualityHighSpeed);
    graphics.SetSmoothingMode(SmoothingModeNone);
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

        }

        BitBlt(hdc, 0, 0, rt.right, rt.bottom, memDC, 0, 0, SRCCOPY);

        return;
    }

    // 스토리 화면 상태이면 73~79번 PNG를 순서대로 보여주고 게임 화면은 아직 그리지 않음
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

        }

        BitBlt(hdc, 0, 0, rt.right, rt.bottom, memDC, 0, 0, SRCCOPY);

        return;
    }

    // 배경은 월드 좌표 기준으로 이어 붙여서 그리고, 화면에는 cameraX만큼 밀려 보이게 함
    int bg1X = -cameraX;
    int bg2X = BG_PART_W - cameraX;

    Image* bg1Image = NULL;
    Image* bg2Image = NULL;

    if (g_currentStage == 2)
    {
        // 2스테이지: 88번(달 있는 앞쪽) + 89번(달 없는 뒤쪽)
        bg1Image = (g_stage2BackgroundFrontScaled != NULL) ? (Image*)g_stage2BackgroundFrontScaled : g_stage2BackgroundFront;
        bg2Image = (g_stage2BackgroundBackScaled != NULL) ? (Image*)g_stage2BackgroundBackScaled : g_stage2BackgroundBack;
    }
    else if (g_currentStage == 3)
    {
        // 3스테이지: 90번 + 91번
        bg1Image = (g_stage3BackgroundFrontScaled != NULL) ? (Image*)g_stage3BackgroundFrontScaled : g_stage3BackgroundFront;
        bg2Image = (g_stage3BackgroundBackScaled != NULL) ? (Image*)g_stage3BackgroundBackScaled : g_stage3BackgroundBack;
    }
    else if (g_currentStage == 4)
    {
        // 4스테이지: 92번 보스전 배경 하나만 사용
        bg1Image = (g_stage4BackgroundScaled != NULL) ? (Image*)g_stage4BackgroundScaled : g_stage4Background;
        bg2Image = NULL;
    }
    else
    {
        bg1Image = (g_backgroundScaled != NULL) ? (Image*)g_backgroundScaled : g_background;
        bg2Image = (g_background2Scaled != NULL) ? (Image*)g_background2Scaled : g_background2;
    }

    if (bg1Image != NULL && bg1X + BG_PART_W > 0 && bg1X < rt.right)
    {
        graphics.DrawImage(bg1Image, bg1X, 0, BG_PART_W, BG_PART_H);
    }
    else if (bg1Image == NULL)
    {
        // 배경 리소스 로드 실패 시 파란 화면으로 보이지 않게 검은색으로 처리
        HBRUSH bgBrush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(memDC, &rt, bgBrush);
        DeleteObject(bgBrush);

        SetBkMode(memDC, TRANSPARENT);
        SetTextColor(memDC, RGB(255, 80, 180));
        if (g_currentStage == 3)
            TextOut(memDC, 20, 20, L"3스테이지 배경 IDB_PNG90 / IDB_PNG91 로드 실패", 42);
        else if (g_currentStage == 4)
            TextOut(memDC, 20, 20, L"4스테이지 배경 IDB_PNG92 로드 실패", 31);
    }

    if (bg2Image != NULL && bg2X + BG_PART_W > 0 && bg2X < rt.right)
    {
        graphics.DrawImage(bg2Image, bg2X, 0, BG_PART_W, BG_PART_H);
    }

    // 커비/몬스터/이펙트는 전부 월드 좌표로 움직이고,
    // 그릴 때만 -cameraX만큼 이동해서 화면에 표시
    GraphicsState worldState = graphics.Save();

    int shakeX = 0;
    int shakeY = 0;
    if (g_currentStage == 4 && g_screenShakeTick > 0)
    {
        shakeX = RandomRange(-4, 4);
        shakeY = RandomRange(-3, 3);
    }

    graphics.TranslateTransform((REAL)(-cameraX + shakeX), (REAL)shakeY);

    for (int i = 0; i < MONSTER_COUNT; i++)
    {
        g_monsters[i].Draw(graphics);
    }

    DrawRescueObjects(graphics);

    DrawDashWind(graphics);

    DrawPowerProjectile(graphics);
    DrawAbilityStar(graphics);
    DrawFireBall(graphics);
    DrawBombObjects(graphics);
    DrawBombExplosions(graphics);
    DrawEnemyFireBalls(graphics);
    DrawBossObjects(graphics);

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
                DrawWorldImage(
                    graphics,
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

    if (g_invincibleMode)
    {
        Pen invPen(Color(220, 255, 255, 80), 3);
        graphics.DrawEllipse(&invPen, kirbyX - 5, kirbyY - 5, kirbyW + 10, kirbyH + 10);
    }

    DrawFireBreath(graphics);

    graphics.Restore(worldState);

    // 화면 고정 UI: 카메라 영향을 받지 않는 체력바
    if (g_currentStage == 4)
    {
        // 2페이즈에는 게임 화면에만 살짝 어두운 보라색 분위기를 덮어서 보스전 느낌 강화
        if (g_boss.phase2)
        {
            SolidBrush phaseBrush(Color(34, 70, 0, 110));
            graphics.FillRectangle(&phaseBrush, 0, 0, rt.right, rt.bottom);
        }
    }

    DrawHPBar(graphics);
    DrawKirbyStatusUI(graphics);
    DrawBossHpBar(graphics);
    DrawBossPatternText(graphics);

    graphics.Flush();

    DrawDebugInfo(memDC, hWnd);

    BitBlt(hdc, 0, 0, rt.right, rt.bottom, memDC, 0, 0, SRCCOPY);

}



LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    switch (iMessage)
    {
    case WM_CREATE:
        srand((unsigned int)time(NULL));

        // 배경 이미지 크기(1000 x 650)에 맞춰 클라이언트 영역을 딱 맞춤.
        ResizeWindowToClient(hWnd, BG_PART_W, BG_PART_H);

        LoadAllImages(hWnd);
        g_currentStage = 1;
        InitMonsters();
        InitRescueObjects();

        // WAV 리소스 배경음악 재생
        // resource.h에 있는 실제 소리 ID 이름이 다르면 IDR_WAVE1만 바꾸면 됨
        PlaySound(
            MAKEINTRESOURCE(IDR_WAVE1),
            g_hInst,
            SND_RESOURCE | SND_ASYNC | SND_LOOP
        );

        UpdateCamera(hWnd);

        SetTimer(hWnd, 1, GAME_TIMER_MS, NULL);
        SetTimer(hWnd, 2, 150, NULL); // 걷기 프레임. 너무 빠르면 불필요하게 페인트가 많아짐
        SetTimer(hWnd, 3, 200, NULL); // 풍선 프레임
        SetTimer(hWnd, 5, 180, NULL); // 흡수 프레임
        SetTimer(hWnd, 7, 160, NULL); // 몬스터 프레임
        break;

    case WM_TIMER:
        if (g_isOpening)
        {
            // 오프닝은 정지 화면이라 계속 다시 그릴 필요 없음
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

                    InvalidateRect(hWnd, NULL, FALSE);
                }
            }

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
            UpdateKirbyStatusEffects();
            UpdateHPBarAnimation();
            UpdatePowerProjectile();
            UpdateAbilityStar();
            CheckRescueChildTouch();
            UpdateRescueObjects();
            CheckDoorTouch(hWnd);
            // 폭탄병은 제거했지만, 나중에 폭탄 커비를 다시 쓸 수 있으니 투사체 갱신 코드는 유지
            if (g_bombKCooldownTick > 0)
                g_bombKCooldownTick--;

            if (g_bombICooldownTick > 0)
                g_bombICooldownTick--;

            UpdateBombAttack();
            UpdateBombObjects();
            UpdateEnemyFireBalls();
            if (g_currentStage == 4)
                g_bossIntroTick++;

            UpdateBossObjects();
            CheckPowerProjectileHitMonsters();
            CheckFireAttacksHitMonsters();
            CheckKirbyAttacksHitBoss();

            for (int i = 0; i < MONSTER_COUNT; i++)
            {
                g_monsters[i].Update();
            }

            CheckKirbyHitByMonsters();
            CheckEnemyFireBallsHitKirby();

            if (isGameOver && !g_gameOverHandled)
            {
                gameOverTick++;

                if (gameOverTick >= GAME_OVER_DELAY)
                {
                    // 메시지박스가 떠 있는 동안 타이머가 또 돌면서 MessageBox가 반복 생성되는 것을 막음
                    g_gameOverHandled = true;

                    KillTimer(hWnd, 1);
                    KillTimer(hWnd, 2);
                    KillTimer(hWnd, 3);
                    KillTimer(hWnd, 5);
                    KillTimer(hWnd, 7);

                    if (g_kirbyFallGameOver)
                        MessageBox(hWnd, L"아래로 떨어졌습니다. 게임 오버!", L"GAME OVER", MB_OK);
                    else
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

            // 메인 타이머에서만 다시 그려서 중복 페인트를 줄임
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

            // 메인 타이머에서만 다시 그려서 중복 페인트를 줄임
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

                // 메인 타이머에서만 다시 그려서 중복 페인트를 줄임
            }
        }
        else if (wParam == 7)
        {
            for (int i = 0; i < MONSTER_COUNT; i++)
            {
                g_monsters[i].NextFrame();
            }
            // 메인 타이머에서만 다시 그려서 중복 페인트를 줄임
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

            int currentWorldW = GetCurrentWorldW();

            if (kirbyX + kirbyW > currentWorldW)
                kirbyX = currentWorldW - kirbyW;

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

        if (wParam == VK_F2)
        {
            g_invincibleMode = !g_invincibleMode;
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
            else if (isBombKirby)
            {
                // 폭탄 커비 I 필살기: 3배 크기 폭탄이 바닥을 튕기며 이동
                StartBombSpecialAttack();
            }
            break;

        case 'U':
            TryBossRewardInteraction();
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

    case WM_SIZE:
        ReleaseBackBuffer();
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_DESTROY:
        ReleaseBackBuffer();

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