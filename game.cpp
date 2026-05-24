#include "Game.h"
#include "resource.h"
#include <mmsystem.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "winmm.lib")

using namespace Gdiplus;

#include "collision.h"
#include "stage.h"
#include "player.h"
#include "resource_manager.h"
#include "effect_manager.h"


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

// 115~137번: 엔딩/임시맵 커비 춤 프레임
#ifndef IDB_PNG115
#define IDB_PNG115 115
#endif
#ifndef IDB_PNG116
#define IDB_PNG116 116
#endif
#ifndef IDB_PNG117
#define IDB_PNG117 117
#endif
#ifndef IDB_PNG118
#define IDB_PNG118 118
#endif
#ifndef IDB_PNG119
#define IDB_PNG119 119
#endif
#ifndef IDB_PNG120
#define IDB_PNG120 120
#endif
#ifndef IDB_PNG121
#define IDB_PNG121 121
#endif
#ifndef IDB_PNG122
#define IDB_PNG122 122
#endif
#ifndef IDB_PNG123
#define IDB_PNG123 123
#endif
#ifndef IDB_PNG124
#define IDB_PNG124 124
#endif
#ifndef IDB_PNG125
#define IDB_PNG125 125
#endif
#ifndef IDB_PNG126
#define IDB_PNG126 126
#endif
#ifndef IDB_PNG127
#define IDB_PNG127 127
#endif
#ifndef IDB_PNG128
#define IDB_PNG128 128
#endif
#ifndef IDB_PNG129
#define IDB_PNG129 129
#endif
#ifndef IDB_PNG130
#define IDB_PNG130 130
#endif
#ifndef IDB_PNG131
#define IDB_PNG131 131
#endif
#ifndef IDB_PNG132
#define IDB_PNG132 132
#endif
#ifndef IDB_PNG133
#define IDB_PNG133 133
#endif
#ifndef IDB_PNG134
#define IDB_PNG134 134
#endif
#ifndef IDB_PNG135
#define IDB_PNG135 135
#endif
#ifndef IDB_PNG136
#define IDB_PNG136 136
#endif
#ifndef IDB_PNG137
#define IDB_PNG137 137
#endif
#ifndef IDB_PNG138
#define IDB_PNG138 138
#endif


HINSTANCE g_hInst;
LPCTSTR lpszClass = L"WindowClass";
LPCTSTR lpszWindowName = L"GDI+ Kirby Animation";

ULONG_PTR g_gdiplusToken;

// 115~137번: 커비 춤 프레임
const int DANCE_FRAME_COUNT = 23;
const int DANCE_FRAME_DURATION = 6;   // 춤 프레임 전환 속도. 값이 클수록 천천히 춤
const int DANCE_SPIN_TICK = 52;       // 초반 빙글빙글 도는 시간
const int DANCE_DRAW_W = 72;
const int DANCE_DRAW_H = 72;

// 춤 위치 이동용 값
// 영상처럼 제자리에서 프레임만 바뀌는 게 아니라
// 오른쪽 -> 왼쪽 -> 가운데 순서로 x값이 움직이게 함.
const int DANCE_FLOOR_Y = 375; // 춤 위치 Y좌표. 값을 줄이면 위로 올라감
const int DANCE_CENTER_X = 500;
const int DANCE_RIGHT_X = 555;
const int DANCE_LEFT_X = 445;
const int DANCE_END_TICK = 332;

Image* g_danceFrames[DANCE_FRAME_COUNT] = { NULL };
int g_danceFrameIndex = 0;
int g_danceFrameTick = 0;
int g_danceTick = 0;
float g_danceAngle = 0.0f;
int g_danceX = DANCE_CENTER_X;
int g_danceY = DANCE_FLOOR_Y - DANCE_DRAW_H;
bool g_danceFinished = false; // 마지막 프레임에서 멈추기용

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

// 138번: 마지막 클리어 스테이지 배경
Image* g_stage5ClearBackground = NULL;
Bitmap* g_stage5ClearBackgroundScaled = NULL;

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

const int KIRBY_MAX_LIVES = 7;
const int RETRY_COUNTDOWN_TICKS = 250; // GAME_TIMER_MS 40ms 기준 10초
const int PAUSE_MENU_COUNT = 3;
const int CONTROL_GUIDE_TICK_MAX = 150; // GAME_TIMER_MS 40ms 기준 약 6초
int g_kirbyLives = KIRBY_MAX_LIVES;
bool g_isPaused = false;
int g_pauseMenuIndex = 0;
bool g_retryActive = false;
bool g_finalGameOver = false;
int g_retryCountdownTick = RETRY_COUNTDOWN_TICKS;
int g_retryRespawnX = 55;
int g_retryRespawnY = 470;
int g_lastSafeKirbyX = 55;
int g_lastSafeKirbyY = 470;
int g_controlGuideTick = CONTROL_GUIDE_TICK_MAX;
int g_currentBgmMode = -1;

bool g_playTimerStarted = false;
int g_playTimeTick = 0;
int g_clearTimeTick = 0;
bool g_clearTimeSaved = false;

// Stage gimmick state
const int WIND_DURATION = 50;      // about 2 seconds
const int WIND_COOLDOWN = 125;     // about 5 seconds
bool g_windActive = false;
int g_windDir = 1;
int g_windTick = 0;
int g_windCooldownTick = WIND_COOLDOWN;

const int FALLING_ROCK_MAX = 3;
const int FALLING_ROCK_WARNING_TICK = 20;
struct FallingRock
{
    bool active;
    bool warning;
    int x;
    int y;
    int targetY;
    int w;
    int h;
    float vy;
    int warningTick;
};
FallingRock g_fallingRocks[FALLING_ROCK_MAX];
int g_fallingRockSpawnTick = 70;

enum GameSoundId
{
    SFX_JUMP = 0,
    SFX_HIT,
    SFX_RESCUE,
    SFX_DOOR,
    SFX_CLEAR,
    SFX_BOSS_PHASE2,
    SFX_PAUSE,
    SFX_RETRY,
    SFX_ATTACK
};

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

#include "collision.cpp"

// 카메라 / 월드 크기
// 배경 PNG22가 0~999, PNG23이 1000~1999에 붙는 구조
const int BG_PART_W = 1000;
const int BG_PART_H = 650;
const int WORLD_W = BG_PART_W * 2;
const int WORLD_H = BG_PART_H;

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

const int STAGE_FADE_TICK_MAX = 28;
const int STAGE_TITLE_TICK_MAX = 70;
const int STAGE_CLEAR_TICK_MAX = 55;
const int RESCUE_EFFECT_TICK_MAX = 28;
int g_stageFadeTick = STAGE_FADE_TICK_MAX;
int g_stageTitleTick = STAGE_TITLE_TICK_MAX;
int g_stageClearTick = 0;
int g_rescueEffectTick = 0;
int g_rescueEffectX = 0;
int g_rescueEffectY = 0;

const int STAR_TRANSITION_CLOSE_TICK = 20;
const int STAR_TRANSITION_OPEN_TICK = 20;
bool g_starTransitionActive = false;
bool g_starTransitionMapChanged = false;
int g_starTransitionTick = 0;
int g_starTransitionTargetStage = 1;
HWND g_starTransitionHwnd = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

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
void UpdateCamera(HWND hWnd);
void DrawWorldImage(Graphics& graphics, Image* image, int x, int y, int w, int h);

void InitMonsters();

void InitBossObjects();
void UpdateBossObjects();
void DrawBossObjects(Graphics& graphics);
void CheckKirbyAttacksHitBoss();
void ResetBossProjectiles();
void ResetDanceStage();
void UpdateDanceStage();
void DrawDanceKirby(Graphics& graphics);
void StartStageTransitionEffect();
void StartStarStageTransition(HWND hWnd, int targetStage);
void UpdateStarStageTransition(HWND hWnd);
void DrawStarStageTransition(Graphics& graphics, int screenW, int screenH);
void ChangeStageNow(HWND hWnd, int targetStage);
void StartStageClearMessage();
void StartRescueEffect(int x, int y);
void PlayGameSound(int soundId);
void RestartCurrentStage(HWND hWnd);
void StartRetrySequence();
void RespawnKirbyAtRetryPoint(HWND hWnd);
void UpdateRetryCountdown(HWND hWnd);
void ResetStageProjectiles();
void SyncStageBGM();
void ResetStageGimmicks();
void UpdateStageGimmicks(HWND hWnd);
void DrawStageGimmicks(Graphics& graphics, int screenW, int screenH);
void DrawDarkVisionOverlay(Graphics& graphics, int screenW, int screenH);

// 보스 보상 문 변수는 아래쪽 보스전 코드에서 실제로 정의됨.
// CheckDoorTouch가 그보다 위에 있어서 여기서는 미리 알려만 줌.
extern bool g_rewardDoorActive;
extern bool g_rewardDoorOpened;
extern int g_rewardDoorX;
extern int g_rewardDoorY;
extern int g_rewardDoorW;
extern int g_rewardDoorH;

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

#include "monster.h"
#include "monster.cpp"
#include "player.cpp"
#include "stage.cpp"
#include "effect_manager.cpp"

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
const int BOSS_PHASE2_TRANSITION_TOTAL = 92;
const int BOSS_PHASE2_SHAKE_TICKS = 18;
const int BOSS_PHASE2_BLACK_END = 44;
const int BOSS_PHASE2_DROP_END = 74;
const int BOSS_PHASE2_DROP_START_Y = 8;

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

    ResetBossProjectiles();
    ResetBossWarnings();
    g_bossPhase2Transition = true;
    g_bossPhase2TransitionTick = BOSS_PHASE2_TRANSITION_TOTAL;
    g_screenShakeTick = BOSS_PHASE2_SHAKE_TICKS;
    PlayGameSound(SFX_BOSS_PHASE2);

    // 2페이즈 패턴은 전환 연출이 끝난 뒤부터 시작되도록 여유를 둠.
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
    g_boss.vx = 0.0f;
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
        int elapsed = BOSS_PHASE2_TRANSITION_TOTAL - g_bossPhase2TransitionTick;
        g_bossPhase2TransitionTick--;

        ResetBossProjectiles();
        ResetBossWarnings();

        if (elapsed < BOSS_PHASE2_SHAKE_TICKS)
        {
            g_screenShakeTick = 2;
            if (elapsed % 4 == 0)
                g_boss.redFlashTick = 8;
            return;
        }

        if (elapsed < BOSS_PHASE2_BLACK_END)
        {
            g_boss.x = BG_PART_W / 2 - g_boss.w / 2;
            g_boss.y = BOSS_PHASE2_DROP_START_Y;
            g_boss.vx = 0.0f;
            g_boss.vy = 0.0f;
            return;
        }

        if (elapsed < BOSS_PHASE2_DROP_END)
        {
            int dropTick = elapsed - BOSS_PHASE2_BLACK_END;
            int dropTotal = BOSS_PHASE2_DROP_END - BOSS_PHASE2_BLACK_END;
            int targetY = BOSS_PHASE2_GROUND_Y;

            g_boss.x = BG_PART_W / 2 - g_boss.w / 2;
            g_boss.y = BOSS_PHASE2_DROP_START_Y + (targetY - BOSS_PHASE2_DROP_START_Y) * dropTick / dropTotal;
            FaceBossToKirby();

            if (dropTick >= dropTotal - 4)
                g_screenShakeTick = 8;

            return;
        }

        if (elapsed < BOSS_PHASE2_TRANSITION_TOTAL)
        {
            g_boss.y = BOSS_PHASE2_GROUND_Y;
            FaceBossToKirby();
            if (elapsed == BOSS_PHASE2_DROP_END)
                g_screenShakeTick = 10;
            return;
        }

        g_bossPhase2Transition = false;
        g_bossPhase2TransitionTick = 0;
        g_boss.y = BOSS_PHASE2_GROUND_Y;
        FaceBossToKirby();
        g_boss.vx = (float)(2 * g_boss.dir);
        g_boss.vy = 0.0f;
        g_screenShakeTick = 10;
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
                PlayGameSound(SFX_DOOR);
                StartStageClearMessage();
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

void DrawBossPhase2TransitionOverlay(Graphics& graphics, int screenW, int screenH)
{
    if (g_currentStage != 4 || !g_bossPhase2Transition)
        return;

    int elapsed = BOSS_PHASE2_TRANSITION_TOTAL - g_bossPhase2TransitionTick;
    if (elapsed < 0)
        elapsed = 0;

    int alpha = 0;
    bool redFlash = false;

    if (elapsed < BOSS_PHASE2_SHAKE_TICKS)
    {
        alpha = (elapsed % 4 < 2) ? 65 : 20;
        redFlash = true;
    }
    else if (elapsed < BOSS_PHASE2_BLACK_END)
    {
        alpha = 248;
    }
    else if (elapsed < BOSS_PHASE2_DROP_END)
    {
        int fadeTick = elapsed - BOSS_PHASE2_BLACK_END;
        int fadeTotal = BOSS_PHASE2_DROP_END - BOSS_PHASE2_BLACK_END;
        alpha = 248 - 210 * fadeTick / fadeTotal;
    }
    else if (elapsed < BOSS_PHASE2_TRANSITION_TOTAL)
    {
        alpha = 30;
        redFlash = true;
    }

    if (alpha > 0)
    {
        SolidBrush darkBrush(redFlash ? Color(alpha, 120, 0, 20) : Color(alpha, 0, 0, 0));
        graphics.FillRectangle(&darkBrush, 0, 0, screenW, screenH);
    }

    if (elapsed >= BOSS_PHASE2_SHAKE_TICKS && elapsed < BOSS_PHASE2_DROP_END)
    {
        FontFamily fontFamily(L"Arial");
        Font dangerFont(&fontFamily, 42, FontStyleBold, UnitPixel);
        Font smallFont(&fontFamily, 18, FontStyleBold, UnitPixel);
        StringFormat format;
        format.SetAlignment(StringAlignmentCenter);
        format.SetLineAlignment(StringAlignmentCenter);

        int textAlpha = 255;
        if (elapsed >= BOSS_PHASE2_BLACK_END)
        {
            int fadeTick = elapsed - BOSS_PHASE2_BLACK_END;
            int fadeTotal = BOSS_PHASE2_DROP_END - BOSS_PHASE2_BLACK_END;
            textAlpha = 255 - 210 * fadeTick / fadeTotal;
        }

        SolidBrush dangerBrush(Color(textAlpha, 255, 40, 70));
        SolidBrush smallBrush(Color(textAlpha, 255, 230, 230));
        RectF dangerRect(0.0f, 150.0f, (REAL)screenW, 52.0f);
        RectF smallRect(0.0f, 200.0f, (REAL)screenW, 30.0f);
        graphics.DrawString(L"DANGER", -1, &dangerFont, dangerRect, &format, &dangerBrush);
        graphics.DrawString(L"NIGHTMARE PHASE 2", -1, &smallFont, smallRect, &format, &smallBrush);
    }
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

#include "resource_manager.cpp"

void DrawHPBar(Graphics& graphics);

const wchar_t* GetStageHudName()
{
    if (g_currentStage == 1) return L"STAGE 1  NIGHTMARE WOODS";
    if (g_currentStage == 2) return L"STAGE 2  MOONLESS HILL";
    if (g_currentStage == 3) return L"STAGE 3  BROKEN DREAM SKY";
    if (g_currentStage == 4) return L"FINAL STAGE  NIGHTMARE CORE";
    if (g_currentStage == 5) return L"ENDING  CLEAR DANCE";
    return L"KIRBY ADVENTURE";
}

void GetRescueCount(int* rescued, int* total)
{
    if (g_currentStage == 1)
    {
        *rescued = g_stage1ChildRescued;
        *total = g_stage1ChildTotal;
        return;
    }

    if (g_currentStage == 2)
    {
        *rescued = g_stage2ChildRescued;
        *total = g_stage2ChildTotal;
        return;
    }

    *rescued = 0;
    *total = 0;
}

void StartStageTransitionEffect()
{
    g_stageFadeTick = STAGE_FADE_TICK_MAX;
    g_stageTitleTick = STAGE_TITLE_TICK_MAX;
}

void StartStarStageTransition(HWND hWnd, int targetStage)
{
    if (g_starTransitionActive)
        return;

    g_starTransitionActive = true;
    g_starTransitionMapChanged = false;
    g_starTransitionTick = 0;
    g_starTransitionTargetStage = targetStage;
    g_starTransitionHwnd = hWnd;
    g_isChangingMap = true;

    StopMove();
    isAbsorb = false;
    isSpace = false;
    isSpaceRelease = false;
    isCrouch = false;
    balloonTick = 0;
    spaceKeyHeld = false;
    ResetStageProjectiles();
}

void UpdateStarStageTransition(HWND hWnd)
{
    if (!g_starTransitionActive)
        return;

    g_starTransitionTick++;

    if (!g_starTransitionMapChanged &&
        g_starTransitionTick >= STAR_TRANSITION_CLOSE_TICK)
    {
        ChangeStageNow(hWnd, g_starTransitionTargetStage);
        g_starTransitionMapChanged = true;
    }

    if (g_starTransitionTick >= STAR_TRANSITION_CLOSE_TICK + STAR_TRANSITION_OPEN_TICK)
    {
        g_starTransitionActive = false;
        g_starTransitionMapChanged = false;
        g_starTransitionTick = 0;
        g_isChangingMap = false;
    }
}

void MakeStarPoints(PointF* points, int cx, int cy, float outerR, float innerR)
{
    const double PI = 3.14159265358979323846;

    for (int i = 0; i < 10; i++)
    {
        double angle = -PI / 2.0 + i * PI / 5.0;
        float r = (i % 2 == 0) ? outerR : innerR;

        points[i].X = (REAL)(cx + cos(angle) * r);
        points[i].Y = (REAL)(cy + sin(angle) * r);
    }
}

void DrawStarStageTransition(Graphics& graphics, int screenW, int screenH)
{
    if (!g_starTransitionActive)
        return;

    float progress = 0.0f;

    if (g_starTransitionTick < STAR_TRANSITION_CLOSE_TICK)
        progress = (float)g_starTransitionTick / (float)STAR_TRANSITION_CLOSE_TICK;
    else
    {
        int openTick = g_starTransitionTick - STAR_TRANSITION_CLOSE_TICK;
        progress = 1.0f - (float)openTick / (float)STAR_TRANSITION_OPEN_TICK;
    }

    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;

    float maxRadius = (float)(screenW > screenH ? screenW : screenH) * 0.95f;
    float outerRadius = maxRadius * (1.0f - progress);
    float innerRadius = outerRadius * 0.45f;
    SolidBrush outsideBrush(Color(230, 20, 15, 45));
    if (outerRadius <= 2.0f)
    {
        graphics.FillRectangle(&outsideBrush, 0, 0, screenW, screenH);
        return;
    }

    PointF starPoints[10];
    MakeStarPoints(starPoints, screenW / 2, screenH / 2, outerRadius, innerRadius);

    GraphicsPath starPath;
    starPath.AddPolygon(starPoints, 10);

    Region darkRegion(Rect(0, 0, screenW, screenH));
    darkRegion.Exclude(&starPath);

    graphics.FillRegion(&outsideBrush, &darkRegion);

    Pen starPen(Color(220, 255, 230, 80), 4);
    graphics.DrawPolygon(&starPen, starPoints, 10);
}

void ResetPlayTimer()
{
    g_playTimerStarted = false;
    g_playTimeTick = 0;
    g_clearTimeTick = 0;
    g_clearTimeSaved = false;
}

void StartPlayTimer()
{
    if (!g_playTimerStarted)
        g_playTimerStarted = true;
}

void UpdatePlayTimer()
{
    if (!g_playTimerStarted || g_clearTimeSaved)
        return;

    g_playTimeTick++;
}

void SaveFinalClearTime()
{
    if (g_clearTimeSaved)
        return;

    g_clearTimeTick = g_playTimeTick;
    g_clearTimeSaved = true;
}

void FormatClearTimeText(wchar_t* buffer, int tick)
{
    int totalSeconds = tick * GAME_TIMER_MS / 1000;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    wsprintf(buffer, L"CLEAR TIME : %02d:%02d", minutes, seconds);
}

void StartStageClearMessage()
{
    g_stageClearTick = STAGE_CLEAR_TICK_MAX;

    if (g_currentStage == 4)
        SaveFinalClearTime();

    PlayGameSound(SFX_CLEAR);
}

void StartRescueEffect(int x, int y)
{
    g_rescueEffectX = x;
    g_rescueEffectY = y;
    g_rescueEffectTick = RESCUE_EFFECT_TICK_MAX;
    PlayGameSound(SFX_RESCUE);
}

void UpdateScreenEffects()
{
    if (g_stageFadeTick > 0)
        g_stageFadeTick--;

    if (g_stageTitleTick > 0)
        g_stageTitleTick--;

    if (g_stageClearTick > 0)
        g_stageClearTick--;

    if (g_controlGuideTick > 0)
        g_controlGuideTick--;

    if (g_rescueEffectTick > 0)
        g_rescueEffectTick--;

    UpdateStageAtmosphereEffects(g_currentStage);
}

void DrawSparkleStar(Graphics& graphics, int cx, int cy, int size, int alpha, bool whiteStar)
{
    if (alpha <= 0)
        return;

    if (size < 3)
        size = 3;

    int inner = size / 2;
    if (inner < 2)
        inner = 2;

    Point points[8] =
    {
        Point(cx, cy - size),
        Point(cx + inner, cy - inner),
        Point(cx + size, cy),
        Point(cx + inner, cy + inner),
        Point(cx, cy + size),
        Point(cx - inner, cy + inner),
        Point(cx - size, cy),
        Point(cx - inner, cy - inner)
    };

    if (alpha > 255)
        alpha = 255;

    Color fillColor = whiteStar ? Color(alpha, 255, 255, 255) : Color(alpha, 255, 230, 70);
    SolidBrush fillBrush(fillColor);
    graphics.FillPolygon(&fillBrush, points, 8);

    if (!whiteStar)
    {
        Pen shinePen(Color(alpha, 255, 255, 220), 1);
        graphics.DrawPolygon(&shinePen, points, 8);
    }
}

void DrawRescueEffect(Graphics& graphics)
{
    if (g_rescueEffectTick <= 0)
        return;

    struct RescueSparkle
    {
        int dx;
        int dy;
        int moveX;
        int moveY;
        int delay;
        int size;
        bool whiteStar;
    };

    static const RescueSparkle sparkles[] =
    {
        {  0, -12,  0, -4,  0, 6, false },
        { -14,  -5, -5, -2,  2, 4, true  },
        {  15,  -6,  5, -2,  3, 4, false },
        {  -9,  10, -4,  3,  5, 3, false },
        {  10,  11,  4,  3,  6, 3, true  },
        { -22,   2, -3,  0,  8, 3, true  },
        {  22,   1,  3,  0,  9, 3, false }
    };

    int elapsed = RESCUE_EFFECT_TICK_MAX - g_rescueEffectTick;
    int count = sizeof(sparkles) / sizeof(sparkles[0]);

    for (int i = 0; i < count; i++)
    {
        int life = elapsed - sparkles[i].delay;
        if (life < 0 || life > 18)
            continue;

        int alpha = 240 - life * 13;
        int twinkle = (life % 6 < 3) ? 1 : 0;
        int x = g_rescueEffectX + sparkles[i].dx + sparkles[i].moveX * life / 18;
        int y = g_rescueEffectY + sparkles[i].dy + sparkles[i].moveY * life / 18;

        DrawSparkleStar(graphics, x, y, sparkles[i].size + twinkle, alpha, sparkles[i].whiteStar);
    }
}

void DrawKirbyDamageFlash(Graphics& graphics)
{
    if (!isKirbyHit)
        return;

    int alpha = 80 + (kirbyHitTick % 6) * 18;
    if (alpha > 180) alpha = 180;

    SolidBrush flashBrush(Color(alpha, 255, 80, 120));
    graphics.FillEllipse(&flashBrush, kirbyX - 4, kirbyY - 4, kirbyW + 8, kirbyH + 8);
}

void DrawGameHUD(Graphics& graphics)
{
    if (g_currentStage == 5)
        return;

    SolidBrush panelBrush(Color(150, 15, 20, 35));
    Pen panelPen(Color(220, 255, 235, 160), 2);
    graphics.FillRectangle(&panelBrush, 12, 12, 382, 128);
    graphics.DrawRectangle(&panelPen, 12, 12, 382, 128);

    FontFamily fontFamily(L"Arial");
    Font stageFont(&fontFamily, 18, FontStyleBold, UnitPixel);
    Font smallFont(&fontFamily, 16, FontStyleBold, UnitPixel);
    SolidBrush titleBrush(Color(245, 255, 245, 210));
    SolidBrush subBrush(Color(240, 230, 240, 255));

    RectF stageRect(28.0f, 18.0f, 340.0f, 24.0f);
    graphics.DrawString(GetStageHudName(), -1, &stageFont, stageRect, NULL, &titleBrush);

    DrawHPBar(graphics);

    int rescued = 0;
    int total = 0;
    GetRescueCount(&rescued, &total);

    if (total > 0)
    {
        SolidBrush rescueBackBrush(Color(105, 35, 28, 40));
        Pen rescuePen(Color(180, 255, 235, 120), 1);
        graphics.FillRectangle(&rescueBackBrush, 286, 84, 82, 30);
        graphics.DrawRectangle(&rescuePen, 286, 84, 82, 30);

        if (g_powerProjectileFrame != NULL)
            graphics.DrawImage(g_powerProjectileFrame, 292, 89, 22, 22);
        else
            DrawSparkleStar(graphics, 302, 99, 8, 255, false);

        wchar_t rescueText[32];
        wsprintf(rescueText, L"%d/%d", rescued, total);
        RectF rescueRect(316.0f, 89.0f, 48.0f, 22.0f);
        graphics.DrawString(rescueText, -1, &smallFont, rescueRect, NULL, &subBrush);
    }
    else if (g_currentStage == 4)
    {
        RectF bossRect(286.0f, 88.0f, 92.0f, 22.0f);
        graphics.DrawString(L"BOSS", -1, &smallFont, bossRect, NULL, &subBrush);
    }

    wchar_t lifeText[32];
    wsprintf(lifeText, L"LIFE  %d", g_kirbyLives);
    RectF lifeRect(28.0f, 116.0f, 110.0f, 20.0f);
    graphics.DrawString(lifeText, -1, &smallFont, lifeRect, NULL, &subBrush);

    if (g_debugMode)
    {
        SolidBrush debugBrush(Color(230, 255, 120, 120));
        RectF debugRect(250.0f, 116.0f, 130.0f, 20.0f);
        graphics.DrawString(L"F1 HITBOX", -1, &smallFont, debugRect, NULL, &debugBrush);
    }
}

void DrawStageMessage(Graphics& graphics, int screenW, int y, const wchar_t* text, int alpha)
{
    if (alpha <= 0)
        return;

    int boxW = 460;
    int boxH = 58;
    int boxX = screenW / 2 - boxW / 2;

    SolidBrush boxBrush(Color(alpha * 150 / 255, 20, 20, 35));
    Pen boxPen(Color(alpha, 255, 240, 180), 2);
    graphics.FillRectangle(&boxBrush, boxX, y, boxW, boxH);
    graphics.DrawRectangle(&boxPen, boxX, y, boxW, boxH);

    FontFamily fontFamily(L"Arial");
    Font font(&fontFamily, 26, FontStyleBold, UnitPixel);
    SolidBrush textBrush(Color(alpha, 255, 250, 210));
    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);
    RectF rect((REAL)boxX, (REAL)y, (REAL)boxW, (REAL)boxH);
    graphics.DrawString(text, -1, &font, rect, &format, &textBrush);
}

void DrawTransitionOverlay(Graphics& graphics, int screenW, int screenH)
{
    if (g_stageFadeTick > 0)
    {
        int alpha = g_stageFadeTick * 255 / STAGE_FADE_TICK_MAX;
        if (alpha > 255) alpha = 255;
        SolidBrush fadeBrush(Color(alpha, 0, 0, 0));
        graphics.FillRectangle(&fadeBrush, 0, 0, screenW, screenH);
    }

    if (g_stageTitleTick > 0)
    {
        int alpha = g_stageTitleTick > 18 ? 230 : g_stageTitleTick * 230 / 18;
        DrawStageMessage(graphics, screenW, 145, GetStageHudName(), alpha);
    }

    if (g_stageClearTick > 0)
    {
        int alpha = g_stageClearTick > 15 ? 245 : g_stageClearTick * 245 / 15;
        DrawStageMessage(graphics, screenW, 210, L"STAGE CLEAR", alpha);

        if (g_currentStage == 4 && g_clearTimeSaved)
        {
            wchar_t clearTimeText[64];
            FormatClearTimeText(clearTimeText, g_clearTimeTick);
            DrawStageMessage(graphics, screenW, 276, clearTimeText, alpha);
        }
    }
}

const wchar_t* GetGameSoundFileName(int soundId)
{
    switch (soundId)
    {
    case SFX_JUMP: return L"jump.wav";
    case SFX_HIT: return L"hit.wav";
    case SFX_RESCUE: return L"rescue.wav";
    case SFX_DOOR: return L"door.wav";
    case SFX_CLEAR: return L"clear.wav";
    case SFX_BOSS_PHASE2: return L"boss_phase2.wav";
    case SFX_PAUSE: return L"pause.wav";
    case SFX_RETRY: return L"retry.wav";
    case SFX_ATTACK: return L"attack.wav";
    }

    return NULL;
}

const wchar_t* GetGameSoundAlias(int soundId)
{
    switch (soundId)
    {
    case SFX_JUMP: return L"sfx_jump";
    case SFX_HIT: return L"sfx_hit";
    case SFX_RESCUE: return L"sfx_rescue";
    case SFX_DOOR: return L"sfx_door";
    case SFX_CLEAR: return L"sfx_clear";
    case SFX_BOSS_PHASE2: return L"sfx_phase2";
    case SFX_PAUSE: return L"sfx_pause";
    case SFX_RETRY: return L"sfx_retry";
    case SFX_ATTACK: return L"sfx_attack";
    }

    return NULL;
}

bool IsFileExistsW(const wchar_t* path)
{
    DWORD attr = GetFileAttributesW(path);
    return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

void GetExeFolder(wchar_t* folder)
{
    GetModuleFileNameW(NULL, folder, MAX_PATH);

    int len = lstrlen(folder);
    for (int i = len - 1; i >= 0; i--)
    {
        if (folder[i] == L'\\' || folder[i] == L'/')
        {
            folder[i] = 0;
            return;
        }
    }

    folder[0] = 0;
}

bool BuildGameSoundPath(const wchar_t* fileName, wchar_t* outPath)
{
    wchar_t exeFolder[MAX_PATH];
    GetExeFolder(exeFolder);

    wsprintf(outPath, L"sound\\%s", fileName);
    if (IsFileExistsW(outPath))
        return true;

    wsprintf(outPath, L"..\\..\\sound\\%s", fileName);
    if (IsFileExistsW(outPath))
        return true;

    wsprintf(outPath, L"%s\\sound\\%s", exeFolder, fileName);
    if (IsFileExistsW(outPath))
        return true;

    wsprintf(outPath, L"%s\\..\\..\\sound\\%s", exeFolder, fileName);
    if (IsFileExistsW(outPath))
        return true;

    return false;
}

int GetStageBgmMode()
{
    if (g_currentStage == 4)
        return 1;

    if (g_currentStage == 5)
        return 2;

    return 0;
}

void StopFileBGM()
{
    mciSendStringW(L"stop bgm_music", NULL, 0, NULL);
    mciSendStringW(L"close bgm_music", NULL, 0, NULL);
}

void PlayDefaultStageBGM()
{
    StopFileBGM();
    PlaySound(MAKEINTRESOURCE(IDR_WAVE1), g_hInst, SND_RESOURCE | SND_ASYNC | SND_LOOP);
}

void PlayFileBGM(const wchar_t* fileName)
{
    wchar_t path[MAX_PATH];
    if (!BuildGameSoundPath(fileName, path))
        return;

    PlaySound(NULL, NULL, 0);
    StopFileBGM();

    wchar_t command[512];
    wsprintf(command, L"open \"%s\" type waveaudio alias bgm_music", path);
    if (mciSendStringW(command, NULL, 0, NULL) != 0)
        return;

    mciSendStringW(L"setaudio bgm_music volume to 650", NULL, 0, NULL);
    mciSendStringW(L"play bgm_music repeat", NULL, 0, NULL);
}

void SyncStageBGM()
{
    int nextMode = GetStageBgmMode();
    if (g_currentBgmMode == nextMode)
        return;

    g_currentBgmMode = nextMode;

    if (nextMode == 1)
        PlayFileBGM(L"kirby_final_boss_nightmare.wav");
    else if (nextMode == 2)
        PlayFileBGM(L"kirby_stage_clear_theme.wav");
    else
        PlayDefaultStageBGM();
}

void PlayGameSound(int soundId)
{
    const wchar_t* fileName = GetGameSoundFileName(soundId);
    const wchar_t* alias = GetGameSoundAlias(soundId);

    if (fileName == NULL || alias == NULL)
        return;

    wchar_t path[MAX_PATH];
    if (!BuildGameSoundPath(fileName, path))
        return;

    wchar_t command[512];
    wsprintf(command, L"stop %s", alias);
    mciSendStringW(command, NULL, 0, NULL);

    wsprintf(command, L"close %s", alias);
    mciSendStringW(command, NULL, 0, NULL);

    wsprintf(command, L"open \"%s\" type waveaudio alias %s", path, alias);
    if (mciSendStringW(command, NULL, 0, NULL) != 0)
        return;

    wsprintf(command, L"play %s from 0", alias);
    mciSendStringW(command, NULL, 0, NULL);
}

void ResetStageProjectiles()
{
    isPowerProjectileActive = false;
    isFireBallActive = false;
    isFireBreath = false;
    isFireAttackPose = false;
    isBombAttack = false;
    g_bombSpecialAttackMode = false;
    bombAttackFrameIndex = 0;
    bombAttackTick = 0;
    bombAttackBombSpawned = false;
    isAbilityStarActive = false;

    for (int i = 0; i < ENEMY_FIREBALL_MAX; i++)
        g_enemyFireBalls[i].active = false;

    for (int i = 0; i < BOMB_OBJECT_MAX; i++)
        g_bombs[i].active = false;

    for (int i = 0; i < BOMB_EXPLOSION_MAX; i++)
    {
        g_bombExplosions[i].active = false;
        g_bombExplosions[i].tick = 0;
    }
}

void ResetStageGimmicks()
{
    g_windActive = false;
    g_windDir = 1;
    g_windTick = 0;
    g_windCooldownTick = WIND_COOLDOWN;

    g_fallingRockSpawnTick = 70;
    for (int i = 0; i < FALLING_ROCK_MAX; i++)
    {
        g_fallingRocks[i].active = false;
        g_fallingRocks[i].warning = false;
        g_fallingRocks[i].x = 0;
        g_fallingRocks[i].y = 0;
        g_fallingRocks[i].targetY = 0;
        g_fallingRocks[i].w = 40;
        g_fallingRocks[i].h = 40;
        g_fallingRocks[i].vy = 0.0f;
        g_fallingRocks[i].warningTick = 0;
    }
}

void PushKirbyByWind()
{
    if (!g_windActive || g_currentStage != 1 || isGameOver || g_retryActive)
        return;

    int push = 1;
    if (!isOnGround)
        push = 2;
    if (isSpace)
        push = 3;

    int nextX = kirbyX + g_windDir * push;
    int currentWorldW = GetCurrentWorldW();

    if (nextX < 0)
        nextX = 0;
    if (nextX + kirbyW > currentWorldW)
        nextX = currentWorldW - kirbyW;

    RECT nextRc = GetKirbyHitBox(nextX, kirbyY);
    RECT hitBlock;
    if (!HitSolidBlock(nextRc, &hitBlock))
        kirbyX = nextX;
}

void UpdateStageWind()
{
    if (g_currentStage != 1)
    {
        g_windActive = false;
        return;
    }

    if (g_windActive)
    {
        g_windTick--;
        PushKirbyByWind();

        if (g_windTick <= 0)
        {
            g_windActive = false;
            g_windCooldownTick = WIND_COOLDOWN;
        }

        return;
    }

    if (g_windCooldownTick > 0)
    {
        g_windCooldownTick--;
        return;
    }

    g_windActive = true;
    g_windTick = WIND_DURATION;
    g_windDir = (RandomRange(0, 1) == 0) ? -1 : 1;
}

int GetRockTargetY(int x, int w, int h)
{
    RECT testRc;
    testRc.left = x;
    testRc.top = 0;
    testRc.right = x + w;
    testRc.bottom = h;

    int groundY = 545;
    if (FindGroundUnderHitBox(testRc, &groundY))
        return groundY;

    return 545;
}

void SpawnFallingRock()
{
    if (g_currentStage != 3)
        return;

    for (int i = 0; i < FALLING_ROCK_MAX; i++)
    {
        if (g_fallingRocks[i].active)
            continue;

        int rockW = 42;
        int rockH = 42;
        int currentWorldW = GetCurrentWorldW();
        int x = kirbyX + RandomRange(-160, 160);

        if (RandomRange(0, 3) == 0)
            x = cameraX + RandomRange(80, 850);

        if (x < 20) x = 20;
        if (x + rockW > currentWorldW - 20) x = currentWorldW - rockW - 20;

        g_fallingRocks[i].active = true;
        g_fallingRocks[i].warning = true;
        g_fallingRocks[i].x = x;
        g_fallingRocks[i].y = -70;
        g_fallingRocks[i].targetY = GetRockTargetY(x, rockW, rockH);
        g_fallingRocks[i].w = rockW;
        g_fallingRocks[i].h = rockH;
        g_fallingRocks[i].vy = (float)RandomRange(8, 12);
        g_fallingRocks[i].warningTick = FALLING_ROCK_WARNING_TICK;
        return;
    }
}

void UpdateFallingRocks()
{
    if (g_currentStage != 3)
    {
        for (int i = 0; i < FALLING_ROCK_MAX; i++)
            g_fallingRocks[i].active = false;
        return;
    }

    g_fallingRockSpawnTick--;
    if (g_fallingRockSpawnTick <= 0)
    {
        SpawnFallingRock();
        g_fallingRockSpawnTick = RandomRange(50, 75);
    }

    RECT kirbyRc = GetKirbyBodyRect();

    for (int i = 0; i < FALLING_ROCK_MAX; i++)
    {
        if (!g_fallingRocks[i].active)
            continue;

        if (g_fallingRocks[i].warning)
        {
            g_fallingRocks[i].warningTick--;
            if (g_fallingRocks[i].warningTick <= 0)
            {
                g_fallingRocks[i].warning = false;
                g_fallingRocks[i].y = -g_fallingRocks[i].h;
            }
            continue;
        }

        g_fallingRocks[i].y += (int)g_fallingRocks[i].vy;
        g_fallingRocks[i].vy += 0.35f;

        RECT rockRc;
        rockRc.left = g_fallingRocks[i].x;
        rockRc.top = g_fallingRocks[i].y;
        rockRc.right = g_fallingRocks[i].x + g_fallingRocks[i].w;
        rockRc.bottom = g_fallingRocks[i].y + g_fallingRocks[i].h;

        if (!isKirbyHit && kirbyHitCooldownTick <= 0 && IsRectHit(kirbyRc, rockRc))
        {
            StartKirbyHitEffect();
            g_fallingRocks[i].active = false;
            continue;
        }

        RECT hitBlock;
        if ((HitSolidBlock(rockRc, &hitBlock) && g_fallingRocks[i].vy >= 0.0f) ||
            g_fallingRocks[i].y > WORLD_H + 80)
        {
            g_fallingRocks[i].active = false;
        }
    }
}

void UpdateStageGimmicks(HWND hWnd)
{
    (void)hWnd;
    UpdateStageWind();
    UpdateFallingRocks();
}

void DrawWindEffect(Graphics& graphics, int screenW, int screenH)
{
    if (g_currentStage != 1 || !g_windActive)
        return;

    Pen windPen(Color(150, 220, 245, 255), 2);
    int move = (WIND_DURATION - g_windTick) * 18 * g_windDir;

    for (int i = 0; i < 12; i++)
    {
        int baseX = i * 170 + move;
        while (baseX < -220) baseX += screenW + 260;
        while (baseX > screenW + 220) baseX -= screenW + 260;

        int worldX = cameraX + baseX;
        int y = 95 + (i % 5) * 34;
        int len = 80 + (i % 3) * 22;

        if (g_windDir > 0)
            graphics.DrawLine(&windPen, worldX, y, worldX + len, y - 8);
        else
            graphics.DrawLine(&windPen, worldX + len, y - 8, worldX, y);
    }

    FontFamily fontFamily(L"Arial");
    Font font(&fontFamily, 15, FontStyleBold, UnitPixel);
    SolidBrush textBrush(Color(220, 235, 245, 255));
    RectF rect((REAL)(cameraX + 28), 148.0f, 210.0f, 24.0f);
    graphics.DrawString(g_windDir > 0 ? L"WIND  >>" : L"<<  WIND", -1, &font, rect, NULL, &textBrush);
}

void DrawFallingRocks(Graphics& graphics)
{
    if (g_currentStage != 3)
        return;

    for (int i = 0; i < FALLING_ROCK_MAX; i++)
    {
        if (!g_fallingRocks[i].active)
            continue;

        if (g_fallingRocks[i].warning)
        {
            int cx = g_fallingRocks[i].x + g_fallingRocks[i].w / 2;
            int y = g_fallingRocks[i].targetY - 10;
            int alpha = 120 + (g_fallingRocks[i].warningTick % 6) * 18;
            SolidBrush warnBrush(Color(alpha, 255, 40, 80));
            Pen warnPen(Color(220, 255, 210, 130), 2);
            graphics.FillEllipse(&warnBrush, cx - 26, y - 8, 52, 16);
            graphics.DrawEllipse(&warnPen, cx - 26, y - 8, 52, 16);
            graphics.DrawLine(&warnPen, cx, y - 55, cx, y - 18);
            graphics.DrawLine(&warnPen, cx - 10, y - 45, cx, y - 58);
            graphics.DrawLine(&warnPen, cx + 10, y - 45, cx, y - 58);
            continue;
        }

        int x = g_fallingRocks[i].x;
        int y = g_fallingRocks[i].y;
        int w = g_fallingRocks[i].w;
        int h = g_fallingRocks[i].h;

        SolidBrush rockBrush(Color(245, 95, 95, 110));
        Pen rockPen(Color(230, 45, 45, 60), 2);
        graphics.FillEllipse(&rockBrush, x, y, w, h);
        graphics.DrawEllipse(&rockPen, x, y, w, h);

        Pen crackPen(Color(170, 40, 40, 50), 1);
        graphics.DrawLine(&crackPen, x + 12, y + 10, x + 22, y + 20);
        graphics.DrawLine(&crackPen, x + 22, y + 20, x + 16, y + 31);
        graphics.DrawLine(&crackPen, x + 28, y + 12, x + 32, y + 25);
    }
}

void DrawStageGimmicks(Graphics& graphics, int screenW, int screenH)
{
    DrawWindEffect(graphics, screenW, screenH);
    DrawFallingRocks(graphics);
}

void DrawDarkVisionOverlay(Graphics& graphics, int screenW, int screenH)
{
    if (g_currentStage != 2)
        return;

    int centerX = kirbyX - cameraX + kirbyW / 2;
    int centerY = kirbyY + kirbyH / 2;
    int radius = 205;

    GraphicsPath viewPath;
    viewPath.AddEllipse(centerX - radius, centerY - radius, radius * 2, radius * 2);

    Region darkRegion(Rect(0, 0, screenW, screenH));
    darkRegion.Exclude(&viewPath);

    SolidBrush darkBrush(Color(180, 0, 0, 0));
    graphics.FillRegion(&darkBrush, &darkRegion);

    Pen softEdgePen(Color(95, 170, 140, 255), 3);
    graphics.DrawEllipse(&softEdgePen, centerX - radius, centerY - radius, radius * 2, radius * 2);
}

void ResetKirbyPlayState(bool recoverHp)
{
    StopMove();
    isDash = false;
    isDragging = false;
    jumpKeyDown = false;
    isAbsorb = false;
    isSpace = false;
    isSpaceRelease = false;
    isCrouch = false;
    balloonTick = 0;
    spaceKeyHeld = false;
    spaceFrameIndex = 0;
    spaceStartFrameDone = false;
    fireBalloonFrameIndex = 0;
    fireBalloonStartFrameDone = false;
    bombBalloonFrameIndex = 0;
    bombBalloonStartFrameDone = false;
    kirbyVY = 0.0f;
    isOnGround = false;

    isGameOver = false;
    g_gameOverHandled = false;
    g_kirbyFallGameOver = false;
    gameOverTick = 0;
    isKirbyHit = false;
    kirbyHitTick = 0;
    kirbyHitCooldownTick = KIRBY_HIT_COOLDOWN;
    g_kirbySlowTick = 0;
    g_kirbyBurnTick = 0;
    g_kirbyBurnDamageTick = 0;

    ResetStageProjectiles();

    if (recoverHp)
    {
        kirbyHP = kirbyMaxHP;
        kirbyDisplayHP = (float)kirbyMaxHP;
    }
}

void SetKirbyStageStartPosition()
{
    if (g_currentStage == 1)
    {
        kirbyX = 55;
        kirbyY = 470;
    }
    else if (g_currentStage == 2)
    {
        kirbyX = 70;
        kirbyY = 330;
    }
    else if (g_currentStage == 3)
    {
        kirbyX = 145;
        kirbyY = 500;
    }
    else if (g_currentStage == 4)
    {
        kirbyX = 80;
        kirbyY = 480;
    }
    else if (g_currentStage == 5)
    {
        kirbyX = DANCE_CENTER_X - NORMAL_KIRBY_W / 2;
        kirbyY = DANCE_FLOOR_Y - NORMAL_KIRBY_H;
        SetKirbyNormalSizeKeepBottom();
    }

    g_lastSafeKirbyX = kirbyX;
    g_lastSafeKirbyY = kirbyY;
}

void RestartCurrentStage(HWND hWnd)
{
    g_isPaused = false;
    g_retryActive = false;
    g_finalGameOver = false;
    g_pauseMenuIndex = 0;

    ResetKirbyPlayState(true);
    ResetStageGimmicks();
    SetKirbyStageStartPosition();
    cameraX = 0;

    InitRescueObjects();
    InitMonsters();
    StartStageTransitionEffect();

    if (g_currentStage == 1)
        g_controlGuideTick = CONTROL_GUIDE_TICK_MAX;
    else
        g_controlGuideTick = 0;

    UpdateCamera(hWnd);
    PlayGameSound(SFX_RETRY);
}

void StartRetrySequence()
{
    if (g_retryActive || g_finalGameOver)
        return;

    g_gameOverHandled = true;
    StopMove();
    isSpace = false;
    isSpaceRelease = false;
    isAbsorb = false;
    isCrouch = false;
    balloonTick = 0;
    spaceKeyHeld = false;
    kirbyVY = 0.0f;

    if (g_kirbyFallGameOver)
    {
        g_retryRespawnX = g_lastSafeKirbyX;
        g_retryRespawnY = g_lastSafeKirbyY;
    }
    else
    {
        g_retryRespawnX = kirbyX;
        g_retryRespawnY = kirbyY;
    }

    if (g_kirbyLives > 0)
        g_kirbyLives--;

    PlayGameSound(SFX_HIT);

    if (g_kirbyLives <= 0)
    {
        g_finalGameOver = true;
        g_retryActive = false;
        return;
    }

    g_retryActive = true;
    g_retryCountdownTick = RETRY_COUNTDOWN_TICKS;
}

void RespawnKirbyAtRetryPoint(HWND hWnd)
{
    if (!g_retryActive || g_kirbyLives <= 0)
        return;

    ResetKirbyPlayState(true);
    ResetStageGimmicks();

    kirbyX = g_retryRespawnX;
    kirbyY = g_retryRespawnY;

    int currentWorldW = GetCurrentWorldW();
    if (kirbyX < 0)
        kirbyX = 0;

    if (kirbyX + kirbyW > currentWorldW)
        kirbyX = currentWorldW - kirbyW;

    if (kirbyY < 0)
        kirbyY = 0;

    if (kirbyY + kirbyH > WORLD_H)
        kirbyY = WORLD_H - kirbyH;

    g_retryActive = false;
    g_finalGameOver = false;
    g_retryCountdownTick = RETRY_COUNTDOWN_TICKS;
    g_lastSafeKirbyX = kirbyX;
    g_lastSafeKirbyY = kirbyY;

    StartStageTransitionEffect();
    UpdateCamera(hWnd);
    PlayGameSound(SFX_RETRY);
}

void UpdateRetryCountdown(HWND hWnd)
{
    if (!g_retryActive)
        return;

    if (g_retryCountdownTick > 0)
        g_retryCountdownTick--;
    else
    {
        g_retryActive = false;
        g_finalGameOver = true;
    }

    InvalidateRect(hWnd, NULL, FALSE);
}

void DrawControlGuide(Graphics& graphics, int screenW)
{
    if (g_currentStage != 1 || g_controlGuideTick <= 0)
        return;

    int boxW = 230;
    int boxH = 172;
    int boxX = screenW - boxW - 16;
    int boxY = 14;

    int alpha = 155;
    if (g_controlGuideTick < 35)
        alpha = 155 * g_controlGuideTick / 35;

    SolidBrush boxBrush(Color(alpha, 12, 16, 30));
    Pen boxPen(Color(alpha + 70 > 255 ? 255 : alpha + 70, 255, 235, 150), 2);
    graphics.FillRectangle(&boxBrush, boxX, boxY, boxW, boxH);
    graphics.DrawRectangle(&boxPen, boxX, boxY, boxW, boxH);

    FontFamily fontFamily(L"Arial");
    Font titleFont(&fontFamily, 16, FontStyleBold, UnitPixel);
    Font lineFont(&fontFamily, 14, FontStyleBold, UnitPixel);
    SolidBrush titleBrush(Color(alpha + 90 > 255 ? 255 : alpha + 90, 255, 245, 210));
    SolidBrush lineBrush(Color(alpha + 80 > 255 ? 255 : alpha + 80, 230, 235, 255));

    graphics.DrawString(L"CONTROL", -1, &titleFont, PointF((REAL)(boxX + 14), (REAL)(boxY + 10)), &titleBrush);
    graphics.DrawString(L"\x2190 \x2192  이동", -1, &lineFont, PointF((REAL)(boxX + 18), (REAL)(boxY + 36)), &lineBrush);
    graphics.DrawString(L"Space  풍선 날기", -1, &lineFont, PointF((REAL)(boxX + 18), (REAL)(boxY + 58)), &lineBrush);
    graphics.DrawString(L"K  공격", -1, &lineFont, PointF((REAL)(boxX + 18), (REAL)(boxY + 80)), &lineBrush);
    graphics.DrawString(L"I  특별 공격", -1, &lineFont, PointF((REAL)(boxX + 18), (REAL)(boxY + 102)), &lineBrush);
    graphics.DrawString(L"O  변신 해제", -1, &lineFont, PointF((REAL)(boxX + 18), (REAL)(boxY + 124)), &lineBrush);
    graphics.DrawString(L"ESC  일시정지", -1, &lineFont, PointF((REAL)(boxX + 18), (REAL)(boxY + 146)), &lineBrush);
}

void DrawPauseMenu(Graphics& graphics, int screenW, int screenH)
{
    if (!g_isPaused)
        return;

    SolidBrush darkBrush(Color(170, 0, 0, 0));
    graphics.FillRectangle(&darkBrush, 0, 0, screenW, screenH);

    int boxW = 360;
    int boxH = 260;
    int boxX = screenW / 2 - boxW / 2;
    int boxY = screenH / 2 - boxH / 2;

    SolidBrush boxBrush(Color(220, 18, 18, 32));
    Pen boxPen(Color(240, 255, 230, 120), 3);
    graphics.FillRectangle(&boxBrush, boxX, boxY, boxW, boxH);
    graphics.DrawRectangle(&boxPen, boxX, boxY, boxW, boxH);

    FontFamily fontFamily(L"Arial");
    Font titleFont(&fontFamily, 34, FontStyleBold, UnitPixel);
    Font itemFont(&fontFamily, 22, FontStyleBold, UnitPixel);
    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);

    SolidBrush titleBrush(Color(255, 255, 245, 210));
    RectF titleRect((REAL)boxX, (REAL)(boxY + 22), (REAL)boxW, 48.0f);
    graphics.DrawString(L"PAUSE", -1, &titleFont, titleRect, &format, &titleBrush);

    const wchar_t* items[PAUSE_MENU_COUNT] = { L"Resume", L"Restart Stage", L"Quit" };
    for (int i = 0; i < PAUSE_MENU_COUNT; i++)
    {
        int y = boxY + 92 + i * 48;
        if (i == g_pauseMenuIndex)
        {
            SolidBrush selBrush(Color(160, 255, 80, 110));
            graphics.FillRectangle(&selBrush, boxX + 48, y, boxW - 96, 36);
        }

        SolidBrush itemBrush(i == g_pauseMenuIndex ? Color(255, 255, 255, 255) : Color(230, 220, 220, 235));
        RectF itemRect((REAL)boxX, (REAL)y, (REAL)boxW, 36.0f);
        graphics.DrawString(items[i], -1, &itemFont, itemRect, &format, &itemBrush);
    }
}

void DrawRetryOverlay(Graphics& graphics, int screenW, int screenH)
{
    if (!g_retryActive && !g_finalGameOver)
        return;

    SolidBrush darkBrush(Color(205, 0, 0, 0));
    graphics.FillRectangle(&darkBrush, 0, 0, screenW, screenH);

    FontFamily fontFamily(L"Arial");
    Font titleFont(&fontFamily, 42, FontStyleBold, UnitPixel);
    Font subFont(&fontFamily, 22, FontStyleBold, UnitPixel);
    Font smallFont(&fontFamily, 16, FontStyleBold, UnitPixel);
    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);

    SolidBrush titleBrush(Color(255, 255, 240, 200));
    SolidBrush subBrush(Color(240, 230, 230, 255));
    SolidBrush warnBrush(Color(255, 255, 80, 110));

    if (g_finalGameOver)
    {
        RectF titleRect(0.0f, 205.0f, (REAL)screenW, 58.0f);
        RectF subRect(0.0f, 270.0f, (REAL)screenW, 32.0f);
        graphics.DrawString(L"GAME OVER", -1, &titleFont, titleRect, &format, &warnBrush);
        graphics.DrawString(L"ESC  Quit", -1, &subFont, subRect, &format, &subBrush);
        return;
    }

    int seconds = (g_retryCountdownTick + 24) / 25;
    if (seconds < 0)
        seconds = 0;

    wchar_t lifeText[64];
    wchar_t timeText[64];
    wsprintf(lifeText, L"LIFE  %d", g_kirbyLives);
    wsprintf(timeText, L"%d", seconds);

    RectF titleRect(0.0f, 185.0f, (REAL)screenW, 58.0f);
    RectF lifeRect(0.0f, 248.0f, (REAL)screenW, 32.0f);
    RectF timeRect(0.0f, 288.0f, (REAL)screenW, 42.0f);
    RectF guideRect(0.0f, 346.0f, (REAL)screenW, 30.0f);

    graphics.DrawString(L"RETRY?", -1, &titleFont, titleRect, &format, &titleBrush);
    graphics.DrawString(lifeText, -1, &subFont, lifeRect, &format, &subBrush);
    graphics.DrawString(timeText, -1, &titleFont, timeRect, &format, &warnBrush);
    graphics.DrawString(L"SPACE  다시 태어나기     ESC  포기", -1, &smallFont, guideRect, &format, &subBrush);
}
void UpdatePlayer(HWND hWnd)
{
    if (!isDragging && g_currentStage != 5)
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

    if (!isGameOver && !g_retryActive && isOnGround && kirbyY < WORLD_H)
    {
        g_lastSafeKirbyX = kirbyX;
        g_lastSafeKirbyY = kirbyY;
    }
}

void UpdateStage(HWND hWnd)
{
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
    UpdateDanceStage();
}

void CheckCollision()
{
    CheckPowerProjectileHitMonsters();
    CheckFireAttacksHitMonsters();
    CheckKirbyAttacksHitBoss();
}

void UpdateMonster()
{
    for (int i = 0; i < MONSTER_COUNT; i++)
    {
        g_monsters[i].Update();
    }
}

void CheckKirbyCollision()
{
    CheckKirbyHitByMonsters();
    CheckEnemyFireBallsHitKirby();
}

void DrawHUD(Graphics& graphics, int screenW, int screenH)
{
    DrawGameHUD(graphics);
    DrawControlGuide(graphics, screenW);
    DrawKirbyStatusUI(graphics);
    DrawBossHpBar(graphics);
    DrawBossPatternText(graphics);
    DrawTransitionOverlay(graphics, screenW, screenH);
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
    int frameY = 34;
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

    // 5스테이지는 138번 클리어 배경을 사용함

    int shakeX = 0;
    int shakeY = 0;
    if (g_currentStage == 4 && g_screenShakeTick > 0)
    {
        int powerX = 4;
        int powerY = 3;

        if (g_bossPhase2Transition)
        {
            int phaseElapsed = BOSS_PHASE2_TRANSITION_TOTAL - g_bossPhase2TransitionTick;
            if (phaseElapsed < BOSS_PHASE2_SHAKE_TICKS)
            {
                powerX = 12;
                powerY = 8;
            }
            else
            {
                powerX = 7;
                powerY = 5;
            }
        }

        shakeX = RandomRange(-powerX, powerX);
        shakeY = RandomRange(-powerY, powerY);
    }

    // 배경도 같이 흔들리도록 cameraX와 화면 흔들림을 함께 적용
    int bg1X = -cameraX + shakeX;
    int bg2X = BG_PART_W - cameraX + shakeX;

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
    else if (g_currentStage == 5)
    {
        // 5스테이지: 138번 마지막 클리어 배경 하나만 사용
        bg1Image = (g_stage5ClearBackgroundScaled != NULL) ? (Image*)g_stage5ClearBackgroundScaled : g_stage5ClearBackground;
        bg2Image = NULL;
    }
    else
    {
        bg1Image = (g_backgroundScaled != NULL) ? (Image*)g_backgroundScaled : g_background;
        bg2Image = (g_background2Scaled != NULL) ? (Image*)g_background2Scaled : g_background2;
    }

    if (bg1Image != NULL && bg1X + BG_PART_W > 0 && bg1X < rt.right)
    {
        graphics.DrawImage(bg1Image, bg1X, shakeY, BG_PART_W, BG_PART_H);
    }
    else if (bg1Image == NULL && g_currentStage != 5)
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
        graphics.DrawImage(bg2Image, bg2X, shakeY, BG_PART_W, BG_PART_H);
    }

    DrawStageAtmosphereEffects(graphics, g_currentStage, cameraX - shakeX, rt.right, rt.bottom);

    // 커비/몬스터/이펙트는 전부 월드 좌표로 움직이고,
    // 그릴 때만 -cameraX만큼 이동해서 화면에 표시
    GraphicsState worldState = graphics.Save();

    graphics.TranslateTransform((REAL)(-cameraX + shakeX), (REAL)shakeY);

    DrawStageGimmicks(graphics, rt.right, rt.bottom);

    for (int i = 0; i < MONSTER_COUNT; i++)
    {
        g_monsters[i].Draw(graphics);
    }

    DrawRescueObjects(graphics);
    DrawRescueEffect(graphics);

    DrawDashWind(graphics);

    DrawPowerProjectile(graphics);
    DrawAbilityStar(graphics);
    DrawFireBall(graphics);
    DrawBombObjects(graphics);
    DrawBombExplosions(graphics);
    DrawEnemyFireBalls(graphics);
    DrawBossObjects(graphics);

    bool hideKirbyBlink = isKirbyHit && g_currentStage != 5 && ((kirbyHitTick / 3) % 2 == 1);
    if (!hideKirbyBlink)
    {
    if (g_currentStage == 5)
    {
        DrawDanceKirby(graphics);
    }
    else if (isKirbyHit)
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

    }
    DrawKirbyDamageFlash(graphics);
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

    DrawDarkVisionOverlay(graphics, rt.right, rt.bottom);

    DrawHUD(graphics, rt.right, rt.bottom);
    DrawBossPhase2TransitionOverlay(graphics, rt.right, rt.bottom);
    DrawPauseMenu(graphics, rt.right, rt.bottom);
    DrawRetryOverlay(graphics, rt.right, rt.bottom);
    DrawStarStageTransition(graphics, rt.right, rt.bottom);

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
        g_kirbyLives = KIRBY_MAX_LIVES;
        ResetPlayTimer();
        ResetStageAtmosphereEffects();
        ResetStageGimmicks();
        g_lastSafeKirbyX = kirbyX;
        g_lastSafeKirbyY = kirbyY;
        InitMonsters();
        InitRescueObjects();

        // WAV 리소스 배경음악 재생
        // resource.h에 있는 실제 소리 ID 이름이 다르면 IDR_WAVE1만 바꾸면 됨
        SyncStageBGM();

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
                        StartStageTransitionEffect();
                        StartPlayTimer();
                        g_controlGuideTick = CONTROL_GUIDE_TICK_MAX;
                    }

                    InvalidateRect(hWnd, NULL, FALSE);
                }
            }

            return 0;
        }

        if (g_starTransitionActive)
        {
            if (wParam == 1)
            {
                UpdateStarStageTransition(hWnd);
                SyncStageBGM();
                InvalidateRect(hWnd, NULL, FALSE);
            }

            return 0;
        }

        if (g_isPaused || g_retryActive || g_finalGameOver)
        {
            if (wParam == 1)
            {
                if (g_retryActive)
                {
                    UpdatePlayTimer();
                    UpdateRetryCountdown(hWnd);
                }
                else
                {
                    InvalidateRect(hWnd, NULL, FALSE);
                }
            }

            return 0;
        }


        if (wParam == 1)
        {
            UpdatePlayTimer();
            UpdatePlayer(hWnd);
            UpdateStageGimmicks(hWnd);
            UpdateStage(hWnd);
            SyncStageBGM();
            CheckCollision();
            UpdateMonster();
            CheckKirbyCollision();

            if (isGameOver && !g_gameOverHandled)
            {
                gameOverTick++;

                if (gameOverTick >= GAME_OVER_DELAY)
                {
                    StartRetrySequence();
                    InvalidateRect(hWnd, NULL, FALSE);
                    return 0;
                }
            }

            UpdateScreenEffects();
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
        if (g_isOpening || g_isStory || g_isPaused || g_retryActive || g_finalGameOver)
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
        if (g_isOpening || g_isStory || g_isPaused || g_retryActive || g_finalGameOver)
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

        if (g_finalGameOver)
        {
            if (wParam == VK_ESCAPE)
                DestroyWindow(hWnd);

            return 0;
        }

        if (g_retryActive)
        {
            if (wParam == VK_SPACE)
            {
                RespawnKirbyAtRetryPoint(hWnd);
                InvalidateRect(hWnd, NULL, FALSE);
            }
            else if (wParam == VK_ESCAPE)
            {
                g_retryActive = false;
                g_finalGameOver = true;
                InvalidateRect(hWnd, NULL, FALSE);
            }

            return 0;
        }

        if (g_isPaused)
        {
            if (wParam == VK_ESCAPE)
            {
                g_isPaused = false;
                PlayGameSound(SFX_PAUSE);
            }
            else if (wParam == VK_UP || wParam == 'W')
            {
                g_pauseMenuIndex--;
                if (g_pauseMenuIndex < 0)
                    g_pauseMenuIndex = PAUSE_MENU_COUNT - 1;
                PlayGameSound(SFX_PAUSE);
            }
            else if (wParam == VK_DOWN || wParam == 'S')
            {
                g_pauseMenuIndex++;
                if (g_pauseMenuIndex >= PAUSE_MENU_COUNT)
                    g_pauseMenuIndex = 0;
                PlayGameSound(SFX_PAUSE);
            }
            else if (wParam == VK_RETURN || wParam == VK_SPACE)
            {
                if (g_pauseMenuIndex == 0)
                {
                    g_isPaused = false;
                    PlayGameSound(SFX_PAUSE);
                }
                else if (g_pauseMenuIndex == 1)
                {
                    RestartCurrentStage(hWnd);
                }
                else
                {
                    DestroyWindow(hWnd);
                }
            }

            InvalidateRect(hWnd, NULL, FALSE);
            return 0;
        }

        if (wParam == VK_ESCAPE)
        {
            g_isPaused = true;
            g_pauseMenuIndex = 0;
            StopMove();
            PlayGameSound(SFX_PAUSE);
            InvalidateRect(hWnd, NULL, FALSE);
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
        case VK_LEFT:
            moveLeft = true;
            kirbyFaceLeft = true;
            break;

        case 'D':
        case VK_RIGHT:
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
                if (isOnGround && !isSpace)
                {
                    StartJump();
                    break;
                }

                if (!isSpace)
                {
                    PlayGameSound(SFX_JUMP);
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

                if (GetAsyncKeyState('A') & 0x8000 || GetAsyncKeyState(VK_LEFT) & 0x8000)
                    kirbyFaceLeft = true;

                if (GetAsyncKeyState('D') & 0x8000 || GetAsyncKeyState(VK_RIGHT) & 0x8000)
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
            break;
        }

        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_KEYUP:
        if (g_isOpening || g_isStory || g_isPaused || g_retryActive || g_finalGameOver)
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
        case VK_LEFT:
            moveLeft = false;

            if (GetAsyncKeyState('D') & 0x8000 || GetAsyncKeyState(VK_RIGHT) & 0x8000)
                kirbyFaceLeft = false;

            break;

        case 'D':
        case VK_RIGHT:
            moveRight = false;

            if (GetAsyncKeyState('A') & 0x8000 || GetAsyncKeyState(VK_LEFT) & 0x8000)
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
        StopFileBGM();
        mciSendStringW(L"close all", NULL, 0, NULL);

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
