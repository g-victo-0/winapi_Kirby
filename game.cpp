#include "Game.h"
#include "resource.h"
#include <mmsystem.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <strsafe.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "winmm.lib")

using namespace Gdiplus;

#include "collision.h"
#include "stage.h"
#include "player.h"
#include "resource_manager.h"
#include "effect_manager.h"


// PNG 리소스 번호는 resource.h에서 관리하므로 game.cpp에 중복 번호를 만들지 않음.

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
const int DANCE_FLOOR_Y = 392; // 춤 위치 Y좌표. 값을 줄이면 위로 올라감
const int DANCE_CENTER_X = 500;
const int DANCE_RIGHT_X = 555;
const int DANCE_LEFT_X = 445;
const int DANCE_END_TICK = 332;

Image* g_danceFrames[DANCE_FRAME_COUNT] = { NULL };
Image* g_clearTrophyFrame = NULL;
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
int g_openingTick = 0;


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

// 3스테이지 구출/문 상태
RescueChild g_stage3Child;
StageDoor g_stage3Door;
int g_stage3ChildTotal = 1;
int g_stage3ChildRescued = 0;

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
Image* g_stage3RockFrame = NULL;

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
Image* g_bossDoorFrames[4] = { NULL, NULL, NULL, NULL }; // 108~111번 문 열림
Image* g_bossKeyFrame = NULL;         // 112번 열쇠
Image* g_bossChestClosedFrame = NULL; // 113번 닫힌 상자
Image* g_bossChestOpenFrame = NULL;
Image* g_bossBerserkAbsorbFrame1 = NULL;
Image* g_bossBerserkAbsorbFrame2 = NULL;
Image* g_bossBerserkEnergyBallFrame = NULL;
Image* g_recoveryItemFrames[4] = { NULL, NULL, NULL, NULL };

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

// 143: bomb Kirby crouch frame
Image* g_bombCrouchFrame = NULL;

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

// 148~164번: 망치 커비 프레임
Image* g_hammerIdleFrame = NULL;
Image* g_hammerWalkFrames[4] = { NULL, NULL, NULL, NULL };
Image* g_hammerBalloonStartFrame = NULL;
Image* g_hammerBalloonFrames[2] = { NULL, NULL };
Image* g_hammerCrouchFrame = NULL;
Image* g_hammerKirbyHitFrame = NULL;
Image* g_hammerAttackFrames[7] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };

// 174~185번: 스파크 커비 프레임
Image* g_sparkIdleFrame = NULL;
Image* g_sparkWalkFrames[3] = { NULL, NULL, NULL };
Image* g_sparkBalloonStartFrame = NULL;
Image* g_sparkBalloonFrames[2] = { NULL, NULL };
Image* g_sparkCrouchFrame = NULL;
Image* g_sparkKirbyHitFrame = NULL;
Image* g_sparkAttackFrames[3] = { NULL, NULL, NULL };
Image* g_sparkSpecialAttackFrames[1] = { NULL };
Image* g_sparkLightningFrame = NULL;

// 165~173, 189번: 망치 몬스터 프레임
Image* g_hammerMonsterIdleFrame = NULL;
Image* g_hammerMonsterWalkFrames[4] = { NULL, NULL, NULL, NULL };
Image* g_hammerMonsterAttackFrames[4] = { NULL, NULL, NULL, NULL };
Image* g_hammerMonsterDeadFrame = NULL;

// 191~201번: 스파크 몬스터 프레임
Image* g_sparkMonsterIdleFrame = NULL;
Image* g_sparkMonsterWalkFrames[5] = { NULL, NULL, NULL, NULL, NULL };
Image* g_sparkMonsterAttackFrames[4] = { NULL, NULL, NULL, NULL };
Image* g_sparkMonsterDeadFrame = NULL;

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
const int HAMMER_KIRBY_W = 58;
const int HAMMER_KIRBY_H = 58;

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

// 망치 커비 상태. 3 = 망치 속성
bool isHammerKirby = false;
int hammerWalkFrameIndex = 0;
const int HAMMER_WALK_FRAME_COUNT = 4;
int hammerBalloonFrameIndex = 0;
bool hammerBalloonStartFrameDone = false;
bool isHammerAttack = false;
int hammerAttackFrameIndex = 0;
int hammerAttackTick = 0;
const int HAMMER_ATTACK_FRAME_COUNT = 7;
const int HAMMER_ATTACK_FRAME_DURATION = 2;
bool hammerAttackHitDone = false;
bool isHammerInvincibleSkill = false;
int hammerInvincibleSkillTick = 0;
int hammerInvincibleFrameIndex = 0;
int hammerInvincibleFrameTick = 0;
int hammerInvincibleCooldownTick = 0;
const int HAMMER_INVINCIBLE_DURATION_TICK = 50;
const int HAMMER_INVINCIBLE_COOLDOWN_TICK = 250;
const int HAMMER_INVINCIBLE_FRAME_DURATION = 6;
const int HAMMER_MONSTER_WALK_FRAME_COUNT = 4;
const int HAMMER_MONSTER_ATTACK_FRAME_COUNT = 4;
const int HAMMER_MONSTER_W = 48;
const int HAMMER_MONSTER_H = 48;
const int SPARK_MONSTER_WALK_FRAME_COUNT = 5;
const int SPARK_MONSTER_ATTACK_FRAME_COUNT = 4;
const int SPARK_MONSTER_W = 48;
const int SPARK_MONSTER_H = 48;

// 스파크 커비 상태. 4 = 스파크 속성
bool isSparkKirby = false;
int sparkWalkFrameIndex = 0;
const int SPARK_WALK_FRAME_COUNT = 3;
int sparkBalloonFrameIndex = 0;
bool sparkBalloonStartFrameDone = false;
bool isSparkAttack = false;
int sparkAttackFrameIndex = 0;
int sparkAttackTick = 0;
const int SPARK_ATTACK_FRAME_COUNT = 3;
const int SPARK_ATTACK_FRAME_DURATION = 5;
bool sparkAttackHitDone = false;
const int SPARK_ATTACK_DRAW_W = 82;
const int SPARK_ATTACK_DRAW_H = 74;
bool isSparkSpecialAttack = false;
int sparkSpecialAttackFrameIndex = 0;
int sparkSpecialAttackTick = 0;
bool sparkSpecialAttackHitDone = false;
const int SPARK_SPECIAL_ATTACK_FRAME_COUNT = 1;
const int SPARK_SPECIAL_ATTACK_FRAME_DURATION = 5;
const int SPARK_SPECIAL_ATTACK_DRAW_H = 52;
const int SPARK_SPECIAL_ATTACK_RANGE_W = 92;
const int SPARK_SPECIAL_ATTACK_RANGE_H = 34;
const int SPARK_LIGHTNING_MAX = 4;
const int SPARK_LIGHTNING_DRAW_W = 64;
const int SPARK_LIGHTNING_DRAW_H = 64;
const float SPARK_LIGHTNING_START_VY = 10.0f;
const float SPARK_LIGHTNING_GRAVITY = 0.35f;
struct SparkLightningObject
{
    bool active;
    int x;
    int y;
    int w;
    int h;
    float vy;
};
SparkLightningObject g_sparkLightnings[SPARK_LIGHTNING_MAX];

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
const int KIRBY_DAMAGE = 25;
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
const int PAUSE_MENU_COUNT = 4;
const int CONTROL_GUIDE_TICK_MAX = 150; // GAME_TIMER_MS 40ms 기준 약 6초
const int CONTROL_GUIDE_RESHOW_TICK = 125;
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
bool g_controlGuideForced = false;
int g_currentBgmMode = -1;

bool g_playTimerStarted = false;
int g_playTimeTick = 0;
int g_clearTimeTick = 0;
bool g_clearTimeSaved = false;
int g_totalDamageCount = 0;
int g_totalDeathCount = 0;
int g_bossDamageCount = 0;
int g_bossDeathCount = 0;
int g_totalStudentsRescued = 0;
int g_gameScore = 0;

// 스테이지 장치 상태
const int WIND_DURATION = 50;      // 약 2초
const int WIND_COOLDOWN = 125;     // 약 5초
bool g_windActive = false;
int g_windDir = 1;
int g_windTick = 0;
int g_windCooldownTick = WIND_COOLDOWN;

const int FALLING_ROCK_MAX = 3;
const int FALLING_ROCK_WARNING_TICK = 10;
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
int g_fallingRockSpawnTick = 40;

const int RECOVERY_ITEM_MAX = 10;
const int RECOVERY_ITEM_MEAT = 0;
const int RECOVERY_ITEM_POTION = 1;
const int RECOVERY_ITEM_BIG_POTION = 2;
const int RECOVERY_ITEM_LIFE = 3;
struct RecoveryItem
{
    bool active;
    bool falling;
    int type;
    int x;
    int y;
    int w;
    int h;
    float vy;
};
RecoveryItem g_recoveryItems[RECOVERY_ITEM_MAX];
int g_bossItemDropTick = 220;

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
    SFX_ATTACK,
    SFX_COUNT
};

bool g_sfxOpened[SFX_COUNT] = { false };

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
int dashSpeed = 8;
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

// 풍선 상태는 최대 2.5초까지만 유지
int balloonTick = 0;
const int BALLOON_DURATION_TICK = 63; // GAME_TIMER_MS 40ms 기준 약 2.5초
const int BALLOON_COOLDOWN_TICK = 8; // GAME_TIMER_MS 40ms 기준 약 0.3초
int balloonCooldownTick = 0;
bool spaceKeyHeld = false; // SPACE를 계속 누르고 있어도 제한 뒤 자동 재시작되지 않게 막음

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

// 백버퍼 해제 함수: 화면을 그릴 때 쓰던 메모리 DC와 비트맵을 정리해 메모리 누수를 막는다.
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

// 백버퍼 준비 함수: 화면 크기에 맞는 임시 그림판을 만들고, 모든 장면을 여기에 먼저 그리게 한다.
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

// 창 크기 조정 함수: 실제 게임 화면 영역이 원하는 크기가 되도록 윈도우 전체 크기를 맞춘다.
void ResizeWindowToClient(HWND hWnd, int clientW, int clientH)
{
    RECT rc = { 0, 0, clientW, clientH };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    SetWindowPos(hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);
}

// 화면 표시 확인 함수: 월드 좌표의 물체가 현재 카메라 화면 안에 보이는지 검사한다.
bool IsVisibleWorld(int x, int y, int w, int h)
{
    if (x + w < cameraX) return false;
    if (x > cameraX + g_backW) return false;
    if (y + h < 0) return false;
    if (y > g_backH) return false;
    return true;
}

// 월드 이미지 출력 함수: 카메라 기준으로 보이는 물체만 실제 화면에 그린다.
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
void StartHammerKirbyTransform();
void StartHammerAttack();
void UpdateHammerAttack();
RECT GetHammerAttackRect();
bool IsHammerAttackDamageFrame();
void CheckHammerAttackHitMonsters();
void StartHammerInvincibleSkill();
void UpdateHammerInvincibleSkill();
void StartSparkKirbyTransform();
void StartSparkAttack();
void UpdateSparkAttack();
RECT GetSparkAttackRect();
bool IsSparkAttackDamageFrame();
void CheckSparkAttackHitMonsters();
void StartSparkSpecialAttack();
void UpdateSparkSpecialAttack();
RECT GetSparkSpecialAttackRect();
bool IsSparkSpecialAttackDamageFrame();
void CheckSparkSpecialAttackHitMonsters();
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
void StartBossBerserkHeal();
void UpdateBossBerserkHeal();
bool IsBossBerserk();
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
void AddGameScore(int score);
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
void ResetRecoveryItems();
void UpdateRecoveryItems();
void DrawRecoveryItems(Graphics& graphics);
void DrawDarkVisionOverlay(Graphics& graphics, int screenW, int screenH);
void StartCameraShake(int power, int duration);
void UpdateCameraShake();
int GetCameraDrawOffsetX();
int GetCameraDrawOffsetY();
void StartCameraPush(int dir, int power, int duration);
void UpdateCameraPush();
int GetCameraPushOffsetX();
void DrawScreenEdgeEffects(Graphics& graphics, int screenW, int screenH);

// 보스 보상 문 변수는 아래쪽 보스전 코드에서 실제로 정의됨.
// CheckDoorTouch가 그보다 위에 있어서 여기서는 미리 알려만 줌.
extern bool g_rewardDoorActive;
extern bool g_rewardDoorOpened;
extern int g_rewardDoorX;
extern int g_rewardDoorY;
extern int g_rewardDoorW;
extern int g_rewardDoorH;

// 파워 투사체 갱신 함수: 커비가 뱉은 별의 이동, 수명, 벽 충돌을 처리한다.
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

// 카메라 갱신 함수: 커비 위치를 따라가도록 카메라 X좌표를 계산하고 맵 밖으로 나가지 않게 제한한다.
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
void StartHammerKirbyTransform();
void StartHammerAttack();
void UpdateHammerAttack();
RECT GetHammerAttackRect();
bool IsHammerAttackDamageFrame();
void CheckHammerAttackHitMonsters();
void StartHammerInvincibleSkill();
void UpdateHammerInvincibleSkill();
void StartSparkKirbyTransform();
void StartSparkAttack();
void UpdateSparkAttack();
RECT GetSparkAttackRect();
bool IsSparkAttackDamageFrame();
void CheckSparkAttackHitMonsters();
void StartSparkSpecialAttack();
void UpdateSparkSpecialAttack();
RECT GetSparkSpecialAttackRect();
bool IsSparkSpecialAttackDamageFrame();
void CheckSparkSpecialAttackHitMonsters();
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
const int BOSS_MAX_HP = 1000; // 보스 체력
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
const int BOSS_BERSERK_HEAL_Y = 160;

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

const int BOSS_PROJECTILE_MAX = 28;
struct BossProjectile
{
    bool active;
    int type; // 0=96 미사일, 1=97 입 폭탄, 2=100 상시 낙하 공격, 3=101 상시 낙하 폭탄, 4/7/8/105=파란 공, 5/6/9/104=빨간 공, 10/11=패턴 낙하
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

// 보스전 연출용 상태
bool g_bossIntro = false;
int g_bossIntroTick = 0;
bool g_bossPhase2Transition = false;
int g_bossPhase2TransitionTick = 0;
bool g_bossDeadEffect = false;
int g_bossDeadEffectTick = 0;
bool g_bossClear = false;
bool g_bossBerserkMode = false;
bool g_bossBerserkHealActive = false;
bool g_bossBerserkHealDone = false;
int g_bossBerserkHealTick = 0;
int g_bossBerserkHealStartHP = 0;
bool g_bossBerserkDropActive = false;
int g_bossBerserkDropTick = 0;
int g_bossTopBombShakeCount = 0;
int g_screenShakeTick = 0;

// 카메라 연출 효과. 월드 그리기에만 적용해서 HUD는 고정되게 함.
int g_cameraShakeTick = 0;
int g_cameraShakePower = 0;
int g_cameraOffsetX = 0;
int g_cameraOffsetY = 0;
int g_cameraPushTick = 0;
int g_cameraPushDuration = 0;
int g_cameraPushDir = 0;
int g_cameraPushPower = 0;
int g_edgeEffectTick = 0;
int g_bossBerserkFogTick = 0;

// 스테이지 분위기 흔들림: 1/2/3스테이지에서 가끔 1/2/3번 흔들림.
int g_stageRandomShakeStage = 0;
int g_stageRandomShakeDone = 0;
int g_stageRandomShakeCooldown = 0;

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

const int BOSS_WARNING_MAX = 10;
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

// 랜덤 범위 함수: 최솟값과 최댓값 사이의 정수를 무작위로 반환한다.
int RandomRange(int minValue, int maxValue)
{
    if (maxValue <= minValue)
        return minValue;

    return minValue + rand() % (maxValue - minValue + 1);
}

// 카메라 흔들림 시작 함수: 피격이나 보스 공격 때 화면이 흔들리도록 세기와 시간을 설정한다.
void StartCameraShake(int power, int duration)
{
    if (power <= 0 || duration <= 0)
        return;

    if (power > g_cameraShakePower || duration > g_cameraShakeTick)
    {
        g_cameraShakePower = power;
        g_cameraShakeTick = duration;
    }
}

// 카메라 흔들림 갱신 함수: 남은 시간 동안 흔들림 값을 줄이고, 끝나면 흔들림을 멈춘다.
void UpdateCameraShake()
{
    if (g_cameraShakeTick > 0)
    {
        int range = g_cameraShakePower * 2 + 1;
        g_cameraOffsetX = rand() % range - g_cameraShakePower;
        g_cameraOffsetY = rand() % range - g_cameraShakePower;
        g_cameraShakeTick--;

        if (g_cameraShakeTick <= 0)
        {
            g_cameraShakePower = 0;
            g_cameraOffsetX = 0;
            g_cameraOffsetY = 0;
        }
    }
    else
    {
        g_cameraOffsetX = 0;
        g_cameraOffsetY = 0;
    }
}

// 카메라 밀림 시작 함수: 강한 공격 연출처럼 화면이 한쪽으로 밀리는 효과를 설정한다.
void StartCameraPush(int dir, int power, int duration)
{
    if (power <= 0 || duration <= 0)
        return;

    if (dir < 0)
        dir = -1;
    else
        dir = 1;

    g_cameraPushDir = dir;
    g_cameraPushPower = power;
    g_cameraPushDuration = duration;
    g_cameraPushTick = duration;
}

// 카메라 밀림 갱신 함수: 설정된 시간 동안 화면 밀림 효과를 유지하다가 점점 종료한다.
void UpdateCameraPush()
{
    if (g_cameraPushTick > 0)
    {
        g_cameraPushTick--;

        if (g_cameraPushTick <= 0)
        {
            g_cameraPushTick = 0;
            g_cameraPushDuration = 0;
            g_cameraPushDir = 0;
            g_cameraPushPower = 0;
        }
    }
}

// 카메라 밀림 값 반환 함수: 현재 화면을 좌우로 얼마나 밀어야 하는지 계산한다.
int GetCameraPushOffsetX()
{
    if (g_cameraPushTick <= 0 || g_cameraPushDuration <= 0)
        return 0;

    return g_cameraPushDir * g_cameraPushPower * g_cameraPushTick / g_cameraPushDuration;
}

// 카메라 X 보정값 함수: 흔들림과 밀림 효과를 합쳐 화면에 적용할 가로 보정값을 반환한다.
int GetCameraDrawOffsetX()
{
    if (g_starTransitionActive || g_isChangingMap)
        return 0;

    return g_cameraOffsetX + GetCameraPushOffsetX();
}

// 카메라 Y 보정값 함수: 흔들림 효과로 화면에 적용할 세로 보정값을 반환한다.
int GetCameraDrawOffsetY()
{
    if (g_starTransitionActive || g_isChangingMap)
        return 0;

    return g_cameraOffsetY;
}

// 스테이지 랜덤 흔들림 함수: 특정 스테이지 분위기에 맞춰 약한 화면 흔들림을 가끔 발생시킨다.
void UpdateStageRandomCameraShake()
{
    if (g_currentStage < 1 || g_currentStage > 3)
    {
        g_stageRandomShakeStage = 0;
        g_stageRandomShakeDone = 0;
        g_stageRandomShakeCooldown = 0;
        return;
    }

    if (isGameOver || g_retryActive || g_isPaused)
        return;

    if (g_stageRandomShakeStage != g_currentStage)
    {
        g_stageRandomShakeStage = g_currentStage;
        g_stageRandomShakeDone = 0;
        g_stageRandomShakeCooldown = RandomRange(90, 180);
    }

    int targetShakeCount = g_currentStage;
    if (g_stageRandomShakeDone >= targetShakeCount)
        return;

    if (g_stageRandomShakeCooldown > 0)
    {
        g_stageRandomShakeCooldown--;
        return;
    }

    StartCameraShake(2 + g_currentStage, 5 + g_currentStage);
    g_stageRandomShakeDone++;
    g_stageRandomShakeCooldown = RandomRange(140, 260);
}

// 보스 충돌 박스 함수: 보스의 현재 위치와 크기로 충돌 판정용 사각형을 만든다.
RECT GetBossRect()
{
    RECT rc;
    rc.left = g_boss.x;
    rc.top = g_boss.y;
    rc.right = g_boss.x + g_boss.w;
    rc.bottom = g_boss.y + g_boss.h;
    return rc;
}

// 보스 투사체 초기화 함수: 보스가 만든 미사일, 빗방울, 탄환 정보를 모두 비활성 상태로 되돌린다.
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

// 보스 경고 표시 초기화 함수: 공격 전에 뜨는 위험 표시 정보를 모두 꺼진 상태로 되돌린다.
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

// 보스 방향 설정 함수: 커비가 보스 왼쪽에 있으면 왼쪽, 오른쪽에 있으면 오른쪽을 보게 한다.
void FaceBossToKirby()
{
    int kirbyCenterX = kirbyX + kirbyW / 2;
    int bossCenterX = g_boss.x + g_boss.w / 2;

    if (kirbyCenterX < bossCenterX)
        g_boss.dir = -1;
    else
        g_boss.dir = 1;
}

// 보스 바닥 위치 반환 함수: 보스가 1페이즈인지 2페이즈인지에 따라 기준 바닥 높이를 반환한다.
int GetBossGroundY()
{
    if (g_boss.phase2)
        return BOSS_PHASE2_GROUND_Y;

    return BOSS_GROUND_Y;
}

// 보스 2페이즈 외형 적용 함수: 보스 크기를 2페이즈 크기로 바꾸고 바닥과 화면 안에 맞춘다.
void ApplyBossPhase2Form()
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
}

// 보스 2페이즈 시작 함수: 보스 체력이 절반 이하가 되면 2페이즈 상태와 전환 연출을 시작한다.
void StartBossPhase2()
{
    if (g_boss.phase2 || g_bossPhase2Transition)
        return;

    ResetBossProjectiles();
    ResetBossWarnings();
    g_bossPhase2Transition = true;
    g_bossPhase2TransitionTick = BOSS_PHASE2_TRANSITION_TOTAL;
    g_screenShakeTick = BOSS_PHASE2_SHAKE_TICKS;
    StartCameraShake(12, 25);
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

    g_boss.state = BOSS_STATE_IDLE;
    g_boss.vx = 0.0f;
    g_boss.vy = 0.0f;
}

// 보스 초기화 함수: 보스 체력, 위치, 패턴, 투사체, 보상 오브젝트를 처음 상태로 준비한다.
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
    g_bossBerserkMode = false;
    g_bossBerserkHealActive = false;
    g_bossBerserkHealDone = false;
    g_bossBerserkHealTick = 0;
    g_bossBerserkHealStartHP = 0;
    g_bossBerserkDropActive = false;
    g_bossBerserkDropTick = 0;
    g_bossBerserkFogTick = 0;
    g_bossTopBombShakeCount = 0;
    g_screenShakeTick = 0;
    g_cameraShakeTick = 0;
    g_cameraShakePower = 0;
    g_cameraOffsetX = 0;
    g_cameraOffsetY = 0;
    g_cameraPushTick = 0;
    g_cameraPushDuration = 0;
    g_cameraPushDir = 0;
    g_cameraPushPower = 0;

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
}

// 보스 투사체 생성 함수: 비어 있는 칸을 찾아 보스 탄환의 종류, 위치, 속도를 저장한다.
void SpawnBossProjectile(int type, int x, int y, int w, int h, float vx, float vy)
{
    if (g_bossBerserkMode && !g_bossBerserkHealActive)
    {
        int activeCount = 0;
        for (int i = 0; i < BOSS_PROJECTILE_MAX; i++)
        {
            if (g_bossProjectiles[i].active)
                activeCount++;
        }

        if (activeCount >= 8)
            return;
    }

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

// 보스 경고 생성 함수: 공격이 나오기 전에 위험 위치와 지속 시간을 저장해 화면에 표시한다.
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

// 보스 경고 갱신 함수: 경고 표시 시간을 줄이고, 시간이 끝나면 실제 공격을 생성한다.
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

// 보스 미사일 생성 함수: 커비 방향을 보고 옆으로 날아가는 미사일 공격을 만든다.
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

// 보스 비 공격 생성 함수: 위에서 아래로 떨어지는 기본 낙하 공격을 만든다.
void SpawnBossRainAttack()
{
    // 바로 떨어지지 않고 바닥에 경고 표시 후 100번 낙하 공격 생성
    int x = RandomRange(30, BG_PART_W - 60);
    SpawnBossWarning(2, x, 545 - 18, 26, 18, 16, 0);
}

// 보스 비 폭탄 생성 함수: 위에서 떨어지며 더 위협적인 폭탄형 공격을 만든다.
void SpawnBossRainBomb()
{
    // 바로 떨어지지 않고 바닥에 경고 표시 후 101번 낙하 폭탄 생성
    int x = RandomRange(30, BG_PART_W - 60);
    SpawnBossWarning(3, x, 545 - 20, 29, 20, 18, 0);
}

// 보스 옆 탄환 생성 함수: 화면 옆에서 가로로 날아오는 탄환을 만든다.
void SpawnBossSideBall()
{
    // 가로 위험 표시는 없애고, 공만 바로 빠르게 지나가게 함
    int ballSize = 34;
    int y = RandomRange(205, 430);
    int dir = RandomRange(0, 1) == 0 ? 1 : -1;
    int x = (dir > 0) ? -ballSize : BG_PART_W;

    SpawnBossProjectile(4, x, y, ballSize, ballSize, 9.0f * dir, 0.0f);
}

// 보스 확산탄 생성 함수: 여러 방향으로 퍼지는 탄환을 한 번에 만든다.
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
    bool bossBerserk = IsBossBerserk();

    SpawnBossProjectile(5, sx, sy, size, size, mainSpeed * dirToKirby, -3.0f);
    SpawnBossProjectile(5, sx, sy, size, size, mainSpeed * dirToKirby, 0.0f);
    SpawnBossProjectile(5, sx, sy, size, size, mainSpeed * dirToKirby, 3.0f);

    if (g_boss.phase2 && !bossBerserk)
    {
        SpawnBossProjectile(5, sx, sy, size, size, subSpeed * dirToKirby, -5.0f);
        SpawnBossProjectile(5, sx, sy, size, size, subSpeed * dirToKirby, 5.0f);
    }
}

// 보스 지면 파동 생성 함수: 바닥을 따라 이동하는 파동 공격을 만든다.
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

// 보스 연속 낙하 생성 함수: 여러 개의 낙하 공격을 짧은 간격으로 만들어 압박한다.
void SpawnBossRainBurst()
{
    // 2페이즈 연속 낙하 패턴. 세로 경고 표시는 유지함.
    int burstCount = IsBossBerserk() ? 2 : 5;
    for (int i = 0; i < burstCount; i++)
    {
        int x = RandomRange(35, BG_PART_W - 70);
        int delay = 12 + i * 7;
        int type = (i % 2 == 0) ? 10 : 11;
        int w = 34;
        int h = 20;
        SpawnBossWarning(type, x, 545 - h, w, h, delay, 0);
    }
}

// 보스 조준탄 생성 함수: 커비 위치를 계산해서 커비 쪽으로 날아가는 탄환을 만든다.
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

// 보스 벽 낙하 생성 함수: 화면 특정 구간에 연속으로 떨어지는 벽 형태 공격을 만든다.
void SpawnBossWallRain()
{
    // 여러 줄이 떨어지지만 커비 근처 한 칸은 안전구역으로 남기는 패턴
    int gapCenter = kirbyX + kirbyW / 2;
    bool bossBerserk = IsBossBerserk();
    int gapW = bossBerserk ? 190 : 145;
    int step = bossBerserk ? 220 : 90;

    for (int x = 45; x < BG_PART_W - 45; x += step)
    {
        if (x > gapCenter - gapW / 2 && x < gapCenter + gapW / 2)
            continue;

        int type = (x / 90) % 2 == 0 ? 10 : 11;
        int w = 34;
        int h = 20;
        SpawnBossWarning(type, x, 545 - h, w, h, 18 + (x % 3) * 3, 0);
    }
}

// 보스 지그재그 탄환 생성 함수: 이동 중 방향이 흔들리는 특수 탄환을 만든다.
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

// 보스 튕김탄 생성 함수: 바닥이나 벽에 닿으면 튕기는 공 형태 공격을 만든다.
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


// 보스 입 폭탄 생성 함수: 보스 입 근처에서 시작하는 폭탄 공격을 만든다.
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

// 보스 데미지 처리 함수: 보스 체력을 줄이고 피격 연출, 2페이즈 전환, 사망 처리를 확인한다.
void DamageBoss(int damage)
{
    if (g_currentStage != 4)
        return;

    if (!g_boss.active)
        return;

    if (g_bossBerserkHealActive)
        return;

    if (damage <= 0)
        return;

    g_boss.hp -= damage;
    g_boss.hitCooldown = 10;
    g_boss.redFlashTick = 6;
    g_boss.dangerTextTick = 10;
    g_screenShakeTick = 0;

    if (g_boss.hp <= BOSS_MAX_HP / 2)
        StartBossPhase2();

    if (g_boss.phase2 && !g_bossBerserkHealDone && g_boss.hp > 0 && g_boss.hp <= BOSS_MAX_HP * 15 / 100)
    {
        StartBossBerserkHeal();
        return;
    }

    if (g_boss.hp <= 0)
    {
        g_boss.hp = 0;
        g_boss.active = false;
        g_bossClear = true;
        AddGameScore(3000);
        g_bossDeadEffect = true;
        g_bossDeadEffectTick = 80;
        g_screenShakeTick = 35;
        StartCameraShake(12, 30);
        ResetBossProjectiles();
        ResetBossWarnings();
    }
}

// 커비 공격과 보스 충돌 검사 함수: 파워별 공격 판정이 보스에게 닿았는지 확인해 데미지를 준다.
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

    if (isHammerAttack && !hammerAttackHitDone && IsHammerAttackDamageFrame())
    {
        RECT hammerRc = GetHammerAttackRect();
        if (IsRectHit(hammerRc, bossRc))
        {
            hammerAttackHitDone = true;
            DamageBoss(30);
            return;
        }
    }

    if (isSparkAttack && !sparkAttackHitDone && IsSparkAttackDamageFrame())
    {
        RECT sparkRc = GetSparkAttackRect();
        if (IsRectHit(sparkRc, bossRc))
        {
            sparkAttackHitDone = true;
            DamageBoss(24);
            return;
        }
    }

    for (int i = 0; i < SPARK_LIGHTNING_MAX; i++)
    {
        if (!g_sparkLightnings[i].active)
            continue;

        RECT sparkRc = GetSparkLightningHitRect(i);
        if (IsRectHit(sparkRc, bossRc))
        {
            g_sparkLightnings[i].active = false;
            DamageBoss(24);
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

// 보스 몸통 충돌 검사 함수: 커비가 보스 몸에 직접 닿았을 때 데미지를 받게 한다.
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

// 보스 투사체 갱신 함수: 보스 탄환을 이동시키고 커비 충돌, 화면 밖 제거, 특수 움직임을 처리한다.
void UpdateBossProjectiles()
{
    RECT kirbyRc = GetKirbyBodyRect();

    for (int i = 0; i < BOSS_PROJECTILE_MAX; i++)
    {
        if (!g_bossProjectiles[i].active)
            continue;


        g_bossProjectiles[i].tick++;


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

        if (!isKirbyHit && kirbyHitCooldownTick <= 0 && IsRectHit(kirbyRc, rc))
        {
            int pType = g_bossProjectiles[i].type;

            if (pType == 4 || pType == 7 || pType == 8 || pType == 11)
                StartKirbySlow(); // 파란 공: 이동속도 감소

            if (pType == 5 || pType == 6 || pType == 9 || pType == 10)
                StartKirbyBurn(); // 빨간 공/폭발: 잠깐 지속피해

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

// 보스 광폭 회복 시작 함수: 보스가 특정 조건에서 회복 연출을 시작하도록 상태를 설정한다.
void StartBossBerserkHeal()
{
    if (g_bossBerserkHealDone || g_bossBerserkHealActive || !g_boss.active || !g_boss.phase2)
        return;

    g_bossBerserkMode = true;
    g_bossBerserkHealActive = true;
    g_bossBerserkHealDone = true;
    g_bossBerserkHealTick = 90;
    g_bossBerserkHealStartHP = g_boss.hp;
    g_bossBerserkDropActive = false;
    g_bossBerserkDropTick = 0;

    ResetBossProjectiles();
    ResetBossWarnings();

    g_boss.state = BOSS_STATE_IDLE;
    g_boss.x = BG_PART_W / 2 - g_boss.w / 2;
    g_boss.y = BOSS_BERSERK_HEAL_Y;
    g_boss.vx = 0.0f;
    g_boss.vy = 0.0f;
    g_boss.dangerTextTick = 40;

    StartCameraShake(8, 18);
}

// 보스 광폭 회복 갱신 함수: 회복 시간 동안 체력을 올리고 연출이 끝나면 다음 패턴으로 돌아간다.
void UpdateBossBerserkHeal()
{
    if (!g_bossBerserkHealActive)
        return;

    g_bossBerserkHealTick--;

    g_boss.x = BG_PART_W / 2 - g_boss.w / 2;
    g_boss.y = BOSS_BERSERK_HEAL_Y;
    g_boss.vx = 0.0f;
    g_boss.vy = 0.0f;
    g_boss.redFlashTick = 0;

    int targetHP = BOSS_MAX_HP * 30 / 100;
    int elapsed = 90 - g_bossBerserkHealTick;
    if (elapsed < 0) elapsed = 0;
    if (elapsed > 90) elapsed = 90;

    if (g_boss.hp < targetHP)
        g_boss.hp = g_bossBerserkHealStartHP + (targetHP - g_bossBerserkHealStartHP) * elapsed / 90;

    if (g_bossBerserkHealTick <= 0)
    {
        g_boss.hp = targetHP;
        g_bossBerserkHealActive = false;
        g_bossBerserkHealTick = 0;
        g_bossBerserkDropActive = true;
        g_bossBerserkDropTick = 0;
        g_boss.vx = 0.0f;
        g_boss.vy = 0.0f;
        ResetBossProjectiles();
        ResetBossWarnings();
    }
}

// 보스 광폭 상태 확인 함수: 현재 보스가 강해진 상태인지 true/false로 알려준다.
bool IsBossBerserk()
{
    return g_currentStage == 4 && g_boss.active && g_boss.phase2 && g_bossBerserkMode;
}

// 보스 전체 갱신 함수: 보스 패턴 선택, 이동, 공격 생성, 투사체, 보상, 사망 연출을 처리한다.
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
            g_screenShakeTick = 0;
        }

        return;
    }

    if (g_bossBerserkHealActive)
    {
        UpdateBossBerserkHeal();
        return;
    }

    if (g_bossBerserkDropActive)
    {
        const int BERSERK_DROP_TOTAL = 30;
        int targetY = BOSS_PHASE2_GROUND_Y;
        g_bossBerserkDropTick++;

        if (g_bossBerserkDropTick > BERSERK_DROP_TOTAL)
            g_bossBerserkDropTick = BERSERK_DROP_TOTAL;

        g_boss.x = BG_PART_W / 2 - g_boss.w / 2;
        g_boss.y = BOSS_BERSERK_HEAL_Y + (targetY - BOSS_BERSERK_HEAL_Y) * g_bossBerserkDropTick / BERSERK_DROP_TOTAL;
        g_boss.vx = 0.0f;
        g_boss.vy = 0.0f;
        FaceBossToKirby();
        ResetBossProjectiles();
        ResetBossWarnings();

        if (g_bossBerserkDropTick >= BERSERK_DROP_TOTAL)
        {
            g_bossBerserkDropActive = false;
            g_bossBerserkDropTick = 0;
            g_boss.y = targetY;
            g_boss.vx = (float)(2 * g_boss.dir);
            g_bossRainAttackCooldown = 75;
            g_bossRainBombCooldown = 95;
            g_bossSideBallCooldown = 130;
            g_bossSpreadShotCooldown = 230;
            g_bossGroundWaveCooldown = 260;
            g_bossRainBurstCooldown = 310;
            g_bossAimedShotCooldown = 185;
            g_bossWallRainCooldown = 390;
            g_bossZigzagCooldown = 205;
            g_bossBounceCooldown = 285;
            g_boss.topBombCooldown = 210;
            g_boss.fastDashCooldown = 170;
            StartCameraShake(5, 10);
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
            ApplyBossPhase2Form();
            g_boss.x = BG_PART_W / 2 - g_boss.w / 2;
            g_boss.y = BOSS_PHASE2_DROP_START_Y;
            g_boss.vx = 0.0f;
            g_boss.vy = 0.0f;
            return;
        }

        if (elapsed < BOSS_PHASE2_DROP_END)
        {
            ApplyBossPhase2Form();
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
            ApplyBossPhase2Form();
            g_boss.y = BOSS_PHASE2_GROUND_Y;
            FaceBossToKirby();
            if (elapsed == BOSS_PHASE2_DROP_END)
            {
                g_screenShakeTick = 10;
                StartCameraShake(10, 18);
            }
            return;
        }

        ApplyBossPhase2Form();
        g_bossPhase2Transition = false;
        g_bossPhase2TransitionTick = 0;
        g_boss.y = BOSS_PHASE2_GROUND_Y;
        FaceBossToKirby();
        g_boss.vx = (float)(2 * g_boss.dir);
        g_boss.vy = 0.0f;
        g_screenShakeTick = 10;
        StartCameraShake(10, 18);
        return;
    }

    UpdateBossWarnings();

    bool bossBerserk = IsBossBerserk();
    if (bossBerserk)
        g_boss.dangerTextTick = 8;

    // 4스테이지에 들어온 순간부터 100번/101번 낙하 공격은 계속 떨어짐
    g_bossRainAttackCooldown--;
    if (g_bossRainAttackCooldown <= 0)
    {
        SpawnBossRainAttack();
        g_bossRainAttackCooldown = bossBerserk ? RandomRange(95, 135) : RandomRange(35, 60); // 숫자가 클수록 하늘 공격 간격 증가
    }

    g_bossRainBombCooldown--;
    if (g_bossRainBombCooldown <= 0)
    {
        SpawnBossRainBomb();
        g_bossRainBombCooldown = bossBerserk ? RandomRange(125, 175) : RandomRange(55, 85); // 숫자가 클수록 폭탄 낙하 간격 증가
    }

    // 2페이즈부터는 세로 낙하 공격 말고 가로로 지나가는 공도 추가
    if (g_boss.phase2)
    {
        g_bossSideBallCooldown--;
        if (g_bossSideBallCooldown <= 0)
        {
            SpawnBossSideBall();
            g_bossSideBallCooldown = bossBerserk ? RandomRange(175, 240) : RandomRange(55, 85); // 숫자가 클수록 가로 공 간격 증가
        }

        g_bossRainBurstCooldown--;
        if (g_bossRainBurstCooldown <= 0)
        {
            SpawnBossRainBurst();
            g_boss.dangerTextTick = 28;
            g_bossRainBurstCooldown = bossBerserk ? RandomRange(260, 360) : RandomRange(180, 260);
        }
    }

    g_bossSpreadShotCooldown--;
    if (g_bossSpreadShotCooldown <= 0)
    {
        SpawnBossSpreadShot();
        g_boss.dangerTextTick = 22;
        g_bossSpreadShotCooldown = bossBerserk ? RandomRange(300, 410) : (g_boss.phase2 ? RandomRange(130, 190) : RandomRange(180, 240));
    }

    g_bossAimedShotCooldown--;
    if (g_bossAimedShotCooldown <= 0)
    {
        SpawnBossAimedShot();
        g_boss.dangerTextTick = 20;
        g_bossAimedShotCooldown = bossBerserk ? RandomRange(230, 320) : (g_boss.phase2 ? RandomRange(95, 140) : RandomRange(150, 210));
    }

    g_bossZigzagCooldown--;
    if (g_bossZigzagCooldown <= 0)
    {
        SpawnBossZigzagShot();
        g_boss.dangerTextTick = 20;
        g_bossZigzagCooldown = bossBerserk ? RandomRange(260, 360) : (g_boss.phase2 ? RandomRange(110, 160) : RandomRange(180, 240));
    }

    if (g_boss.phase2)
    {
        g_bossGroundWaveCooldown--;
        if (g_bossGroundWaveCooldown <= 0)
        {
            SpawnBossGroundWave();
            g_boss.dangerTextTick = 24;
            g_bossGroundWaveCooldown = bossBerserk ? RandomRange(330, 460) : RandomRange(170, 240);
        }

        g_bossWallRainCooldown--;
        if (g_bossWallRainCooldown <= 0)
        {
            SpawnBossWallRain();
            g_boss.dangerTextTick = 26;
            g_bossWallRainCooldown = bossBerserk ? RandomRange(480, 650) : RandomRange(200, 280);
        }

        g_bossBounceCooldown--;
        if (g_bossBounceCooldown <= 0)
        {
            SpawnBossBounceBall();
            g_boss.dangerTextTick = 22;
            g_bossBounceCooldown = bossBerserk ? RandomRange(330, 460) : RandomRange(160, 230);
        }

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
            g_boss.missileCooldown = bossBerserk ? 115 : (g_boss.phase2 ? 100 : 140);
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
            g_boss.dashCooldown = bossBerserk ? 150 : (g_boss.phase2 ? 150 : 210);
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
            g_bossTopBombShakeCount = 0;
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
            g_boss.topBombCooldown = bossBerserk ? RandomRange(180, 260) : RandomRange(190, 260);
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
            g_boss.fastDashCooldown = bossBerserk ? RandomRange(145, 210) : RandomRange(120, 190);
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

        int topBombInterval = bossBerserk ? 20 : 12;
        if (g_boss.actionTick % topBombInterval == 0)
        {
            SpawnBossMouthBomb();
            if (g_bossTopBombShakeCount < 2)
            {
                StartCameraShake(7, 12);
                g_bossTopBombShakeCount++;
            }
        }

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
        StartCameraShake(5, 8);
        StartCameraPush(g_boss.dir, 18, 20);
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
        StartCameraShake(6, 10);
        StartCameraPush(dashDir, 22, 24);
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

// 보스 보상 시작 함수: 보스 처치 후 열쇠와 문이 등장하도록 보상 오브젝트를 초기화한다.
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

// 커비 근처 확인 함수: 커비가 지정한 사각형 주변에 가까이 있는지 검사한다.
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

// 보스 보상 갱신 함수: 열쇠 획득, 문 활성화, 보상 애니메이션 상태를 갱신한다.
void UpdateBossRewardObjects()
{
    if (!g_rewardStarted)
        return;

    if (g_rewardChestActive && !g_rewardChestLanded)
    {
        g_rewardChestY += 6;
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

// 보스 보상 상호작용 함수: 커비가 열쇠나 문 근처에서 상호작용 키를 눌렀는지 확인한다.
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

// 보스 보상 출력 함수: 보스 처치 후 나타나는 열쇠와 문을 화면에 그린다.
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

// 보스 HP바 출력 함수: 보스 체력 비율에 맞춰 상단 체력바를 그린다.
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

// 보스 경고 출력 함수: 곧 공격이 떨어질 위치를 빨간 표시나 경고 이미지로 그린다.
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

// 보스 사망 연출 출력 함수: 보스가 죽을 때 폭발과 사라지는 효과를 그린다.
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

// 보스 그림자 출력 함수: 보스 아래에 그림자를 그려 위치감을 준다.
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

// 보스 레이저 위험 표시 함수: 레이저나 강한 공격이 지나갈 위험 구역을 먼저 보여준다.
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

// 보스 패턴 문구 출력 함수: 현재 보스 패턴이나 경고 문구를 화면에 표시한다.
void DrawBossPatternText(Graphics& graphics)
{
    // 조심/패턴 안내 글자는 출력하지 않음. 빨간 표시만 사용.
    return;
}

// 보스 2페이즈 전환 화면 함수: 2페이즈로 넘어갈 때 화면 전체 연출을 그린다.
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
        RectF dangerRect(0.0f, 165.0f, (REAL)screenW, 58.0f);
        graphics.DrawString(L"DANGER", -1, &dangerFont, dangerRect, &format, &dangerBrush);
    }
}
// 보스 피격 빨간 효과 함수: 보스가 맞았을 때 잠깐 빨갛게 보이도록 덮어 그린다.
void DrawBossHitRedFlash(Graphics& graphics)
{
    if (g_currentStage != 4 || !g_boss.active)
        return;

    if (g_boss.redFlashTick <= 0 || g_bossBerserkHealActive)
        return;

    int alpha = 80 + g_boss.redFlashTick * 22;
    if (alpha > 220) alpha = 220;

    SolidBrush redBrush(Color(alpha, 255, 0, 0));
    graphics.FillEllipse(&redBrush, g_boss.x, g_boss.y, g_boss.w, g_boss.h);
}

// 보스전 파티클 출력 함수: 악몽 느낌의 작은 입자들을 배경에 그려 분위기를 만든다.
void DrawNightmareParticles(Graphics& graphics)
{
    if (g_currentStage != 4)
        return;

    // 간단한 보라색 입자. 리소스 없이도 보스맵 분위기를 살림
    for (int i = 0; i < 5; i++)
    {
        int x = (i * 73 + g_bossIntroTick * 3 + g_boss.hp) % BG_PART_W;
        int y = (i * 47 + g_bossIntroTick * 5 + g_boss.phase2 * 120) % 520;

        int size = g_boss.phase2 ? 4 : 3;
        int alpha = g_boss.phase2 ? 120 : 70;

        SolidBrush pBrush(Color(alpha, 180, 80, 255));
        graphics.FillEllipse(&pBrush, x, y, size, size);
    }
}

// 보스 광폭 회복 연출 함수: 보스가 회복 중일 때 흡수/회복 느낌의 이펙트를 그린다.
void DrawBossBerserkHealEffect(Graphics& graphics)
{
    if (!g_bossBerserkHealActive)
        return;

    int cx = g_boss.x + g_boss.w / 2;
    int cy = g_boss.y + g_boss.h / 2;
    int t = 90 - g_bossBerserkHealTick;
    if (t < 0) t = 0;

    const double PI = 3.14159265358979323846;

    // 1/2 PNG가 주변에서 돌며 들어오는 회복 연출. 공격 판정은 없음.
    for (int i = 0; i < 3; i++)
    {
        Image* absorbFrame = (((t / 5) + i) % 2 == 0) ? g_bossBerserkAbsorbFrame1 : g_bossBerserkAbsorbFrame2;
        double angle = i * PI * 2.0 / 3.0 + t * 0.045;
        float local = (float)((t * 3 + i * 31) % 100) / 100.0f;
        float radius = 500.0f - local * 420.0f;
        float squashY = 0.64f;

        int tailX = cx + (int)(cos(angle) * (radius + 105.0f));
        int tailY = cy + (int)(sin(angle) * (radius + 105.0f) * squashY);
        int headX = cx + (int)(cos(angle) * radius);
        int headY = cy + (int)(sin(angle) * radius * squashY);
        int coreX = cx + (int)(cos(angle) * 38.0f);
        int coreY = cy + (int)(sin(angle) * 24.0f);

        if (absorbFrame == NULL)
            continue;

        int drawW = 112 - (int)(local * 18.0f) + (i % 2) * 8;
        int drawH = drawW;
        int drawX = headX - drawW / 2;
        int drawY = headY - drawH / 2;

        if (headX < cx)
            DrawImageFlipX(graphics, absorbFrame, drawX, drawY, drawW, drawH);
        else
            DrawWorldImage(graphics, absorbFrame, drawX, drawY, drawW, drawH);
    }

    int pulse = (t / 2) % 18;
    SolidBrush coreGlow(Color(70, 135, 30, 230));
    graphics.FillEllipse(&coreGlow, cx - 28 - pulse / 2, cy - 28 - pulse / 2, 56 + pulse, 56 + pulse);

    Pen ringPen(Color(180, 150, 45, 255), (REAL)3);
    graphics.DrawEllipse(&ringPen, cx - 38 - pulse, cy - 38 - pulse, 76 + pulse * 2, 76 + pulse * 2);
}
// 보스 투사체 출력 함수: 현재 활성화된 보스 탄환들을 종류에 맞는 이미지로 그린다.
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

// 보스 전체 출력 함수: 보스 경고, 탄환, 회복 연출, 본체, HP 효과를 정해진 순서로 그린다.
void DrawBossObjects(Graphics& graphics)
{
    if (g_currentStage != 4)
        return;

    DrawNightmareParticles(graphics);
    DrawBossShadow(graphics);
    DrawBossLaserDanger(graphics);
    DrawBossWarnings(graphics);
    DrawBossProjectiles(graphics);
    DrawBossBerserkHealEffect(graphics);
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


// 대시 바람 출력 함수: 커비가 대시할 때 뒤쪽에 바람 효과를 그린다.
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

// 풍선 해제 효과 출력 함수: 커비가 풍선 상태를 끝낼 때 짧은 해제 이펙트를 그린다.
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

// 흡수 앞쪽 효과 출력 함수: 커비가 빨아들이는 방향 앞에 흡입 효과 이미지를 그린다.
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

// 스테이지 이름 반환 함수: 현재 스테이지 번호에 맞는 HUD용 이름 문자열을 반환한다.
const wchar_t* GetStageHudName()
{
    if (g_currentStage == 1) return L"STAGE 1  NIGHTMARE WOODS";
    if (g_currentStage == 2) return L"STAGE 2  MOONLESS HILL";
    if (g_currentStage == 3) return L"STAGE 3  BROKEN DREAM SKY";
    if (g_currentStage == 4) return L"FINAL STAGE  NIGHTMARE CORE";
    if (g_currentStage == 5) return L"";
    return L"KIRBY ADVENTURE";
}

// 구출 수 계산 함수: 현재 스테이지에서 구출한 학생 수와 전체 학생 수를 구한다.
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

    if (g_currentStage == 3)
    {
        *rescued = g_stage3ChildRescued;
        *total = g_stage3ChildTotal;
        return;
    }

    *rescued = 0;
    *total = 0;
}

// 스테이지 전환 효과 시작 함수: 새 스테이지 진입 시 화면 전환 연출 시간을 설정한다.
void StartStageTransitionEffect()
{
    g_stageFadeTick = STAGE_FADE_TICK_MAX;
    g_stageTitleTick = STAGE_TITLE_TICK_MAX;
}

// 별 스테이지 전환 시작 함수: 다음 스테이지로 바로 가지 않고 별 모양 전환 효과를 먼저 시작한다.
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
    balloonCooldownTick = 0;
    spaceKeyHeld = false;
    ResetStageProjectiles();
}

// 별 스테이지 전환 갱신 함수: 별 전환 진행도를 올리고 중간 시점에 실제 스테이지를 바꾼다.
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

// 별 꼭짓점 계산 함수: 별 모양을 그리기 위해 바깥/안쪽 반지름으로 점 좌표를 만든다.
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

// 별 전환 출력 함수: 스테이지 이동 중 화면 위에 별 모양 마스크 효과를 그린다.
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

// 플레이 시간 초기화 함수: 플레이 시간과 클리어 시간 저장 상태를 처음으로 되돌린다.
void ResetPlayTimer()
{
    g_playTimerStarted = false;
    g_playTimeTick = 0;
    g_clearTimeTick = 0;
    g_clearTimeSaved = false;
    g_totalDamageCount = 0;
    g_totalDeathCount = 0;
    g_bossDamageCount = 0;
    g_bossDeathCount = 0;
    g_totalStudentsRescued = 0;
    g_gameScore = 0;
}

// 플레이 시간 시작 함수: 실제 게임 플레이 시간이 증가하도록 타이머 상태를 켠다.
void StartPlayTimer()
{
    if (!g_playTimerStarted)
        g_playTimerStarted = true;
}

// 플레이 시간 갱신 함수: 게임이 진행 중일 때 플레이 시간을 한 틱씩 증가시킨다.
void UpdatePlayTimer()
{
    if (!g_playTimerStarted || g_clearTimeSaved)
        return;

    g_playTimeTick++;
}

bool IsFastClear();
void AddFinalScoreBonus();

// 최종 클리어 시간 저장 함수: 엔딩에 들어갔을 때 마지막 플레이 시간을 기록한다.
void SaveFinalClearTime()
{
    if (g_clearTimeSaved)
        return;

    g_clearTimeTick = g_playTimeTick;
    g_clearTimeSaved = true;
    AddFinalScoreBonus();
}

// 클리어 시간 문자열 함수: 틱으로 저장된 시간을 분/초 형태의 글자로 바꾼다.
void FormatClearTimeText(wchar_t* buffer, int tick)
{
    int totalSeconds = tick * GAME_TIMER_MS / 1000;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    wsprintf(buffer, L"CLEAR TIME : %02d:%02d", minutes, seconds);
}

// 점수 추가 함수: 받은 점수를 현재 점수에 더하고 0 아래로 내려가지 않게 한다.
void AddGameScore(int score)
{
    if (score <= 0)
        return;

    g_gameScore += score;
}

// 전체 구출 대상 수 반환 함수: 모든 스테이지의 학생 수 합계를 반환한다.
int GetTotalStudentCount()
{
    return g_stage1ChildTotal + g_stage2ChildTotal + g_stage3ChildTotal;
}

bool IsFastClear();

// 최종 보너스 점수 함수: 클리어 시간, 구출 수, 노미스 조건에 따라 추가 점수를 준다.
void AddFinalScoreBonus()
{
    int rescuedCount = g_totalStudentsRescued;
    int totalStudents = GetTotalStudentCount();
    if (rescuedCount > totalStudents)
        rescuedCount = totalStudents;

    AddGameScore(g_kirbyLives * 500);
    AddGameScore(rescuedCount * 300);

    if (IsFastClear())
        AddGameScore(1200);
    if (g_bossDamageCount == 0 && g_bossDeathCount == 0)
        AddGameScore(1200);
    if (g_totalDamageCount == 0)
        AddGameScore(1600);
}

// 클리어 점수 반환 함수: 최종 결과 화면에 표시할 점수를 계산해 반환한다.
int GetClearScore()
{
    return g_gameScore;
}

// 업적 개수 계산 함수: 빠른 클리어, 전원 구출, 노미스 같은 달성 조건 개수를 센다.
int GetAchievementCount()
{
    int count = 0;

    if (IsFastClear())
        count++;
    if (g_bossDamageCount == 0 && g_bossDeathCount == 0)
        count++;
    if (g_totalDamageCount == 0)
        count++;

    return count;
}

// 클리어 랭크 문자열 함수: 업적 개수에 따라 결과 화면의 랭크 문구를 반환한다.
const wchar_t* GetClearRankText(int achievementCount)
{
    if (achievementCount >= 3) return L"S";
    if (achievementCount == 2) return L"A";
    if (achievementCount == 1) return L"B";
    return L"C";
}

// 빠른 클리어 확인 함수: 플레이 시간이 기준 시간 이하인지 확인한다.
bool IsFastClear()
{
    int clearSeconds = g_clearTimeTick * GAME_TIMER_MS / 1000;
    return clearSeconds <= 240;
}

// 클리어 결과판 출력 함수: 최종 시간, 점수, 구출 수, 랭크를 결과 화면에 그린다.
void DrawClearResultPanel(Graphics& graphics, int screenW, int screenH)
{
    if (g_currentStage != 5 || !g_clearTimeSaved)
        return;

    int panelW = 390;
    int panelH = 270;
    int panelX = screenW - panelW - 34;
    int panelY = 178;

    SolidBrush panelBrush(Color(190, 12, 12, 28));
    Pen panelPen(Color(230, 255, 230, 90), 2);
    graphics.FillRectangle(&panelBrush, panelX, panelY, panelW, panelH);
    graphics.DrawRectangle(&panelPen, panelX, panelY, panelW, panelH);

    FontFamily fontFamily(L"Arial");
    Font titleFont(&fontFamily, 22, FontStyleBold, UnitPixel);
    Font rankFont(&fontFamily, 54, FontStyleBold, UnitPixel);
    Font lineFont(&fontFamily, 16, FontStyleBold, UnitPixel);
    Font smallFont(&fontFamily, 14, FontStyleBold, UnitPixel);

    SolidBrush titleBrush(Color(255, 255, 245, 200));
    SolidBrush rankBrush(Color(255, 255, 230, 80));
    SolidBrush textBrush(Color(240, 230, 235, 255));
    SolidBrush goodBrush(Color(250, 150, 255, 170));
    SolidBrush offBrush(Color(155, 135, 135, 155));

    StringFormat centerFormat;
    centerFormat.SetAlignment(StringAlignmentCenter);
    centerFormat.SetLineAlignment(StringAlignmentCenter);

    int score = GetClearScore();
    int achievementCount = GetAchievementCount();
    const wchar_t* rankText = GetClearRankText(achievementCount);
    int totalStudents = GetTotalStudentCount();
    int rescuedCount = g_totalStudentsRescued;
    if (rescuedCount > totalStudents)
        rescuedCount = totalStudents;

    RectF titleRect((REAL)panelX, (REAL)(panelY + 12), (REAL)panelW, 30.0f);
    graphics.DrawString(L"CLEAR RESULT", -1, &titleFont, titleRect, &centerFormat, &titleBrush);

    RectF rankRect((REAL)(panelX + 22), (REAL)(panelY + 48), 90.0f, 66.0f);
    graphics.DrawString(rankText, -1, &rankFont, rankRect, &centerFormat, &rankBrush);

    wchar_t line[96];
    wsprintf(line, L"SCORE  %d", score);
    graphics.DrawString(line, -1, &lineFont, PointF((REAL)(panelX + 126), (REAL)(panelY + 55)), &textBrush);

    wsprintf(line, L"LIFE %d   RESCUE %d/%d", g_kirbyLives, rescuedCount, totalStudents);
    graphics.DrawString(line, -1, &lineFont, PointF((REAL)(panelX + 126), (REAL)(panelY + 80)), &textBrush);

    wsprintf(line, L"DEATH %d   DAMAGE %d", g_totalDeathCount, g_totalDamageCount);
    graphics.DrawString(line, -1, &lineFont, PointF((REAL)(panelX + 126), (REAL)(panelY + 105)), &textBrush);

    graphics.DrawString(L"ACHIEVEMENTS", -1, &lineFont, PointF((REAL)(panelX + 24), (REAL)(panelY + 142)), &titleBrush);

    bool achievements[3];
    achievements[0] = IsFastClear();
    achievements[1] = (g_bossDamageCount == 0 && g_bossDeathCount == 0);
    achievements[2] = (g_totalDamageCount == 0);

    const wchar_t* names[3] =
    {
        L"Fast Clear",
        L"Boss No Miss",
        L"No Damage Clear"
    };

    for (int i = 0; i < 3; i++)
    {
        SolidBrush* brush = achievements[i] ? &goodBrush : &offBrush;
        const wchar_t* mark = achievements[i] ? L"[GREAT]" : L"[--]";
        wsprintf(line, L"%s  %s", mark, names[i]);
        graphics.DrawString(line, -1, &smallFont, PointF((REAL)(panelX + 28), (REAL)(panelY + 170 + i * 22)), brush);
    }
}

// 스테이지 클리어 메시지 시작 함수: 문을 열거나 보스를 이겼을 때 안내 문구 시간을 설정한다.
void StartStageClearMessage()
{
    g_stageClearTick = STAGE_CLEAR_TICK_MAX;

    if (g_currentStage == 4)
        SaveFinalClearTime();

    PlayGameSound(SFX_CLEAR);
}

// 구출 효과 시작 함수: 학생을 구출한 위치에 반짝임 효과가 나오도록 좌표와 시간을 저장한다.
void StartRescueEffect(int x, int y)
{
    g_rescueEffectX = x;
    g_rescueEffectY = y;
    g_rescueEffectTick = RESCUE_EFFECT_TICK_MAX;
    PlayGameSound(SFX_RESCUE);
}

// 화면 효과 갱신 함수: 스테이지 전환, 구출 이펙트, 카메라 흔들림, 분위기 효과를 한 번씩 갱신한다.
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
    else
        g_controlGuideForced = false;

    if (g_rescueEffectTick > 0)
        g_rescueEffectTick--;

    g_edgeEffectTick++;

    if (g_currentStage == 4 && IsBossBerserk())
        g_bossBerserkFogTick++;
    else
        g_bossBerserkFogTick = 0;

    UpdateCameraShake();
    UpdateCameraPush();
    UpdateStageRandomCameraShake();

    UpdateStageAtmosphereEffects(g_currentStage);
}

// 반짝 별 출력 함수: 구출이나 결과 화면에서 쓰는 작은 별 모양을 그린다.
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

// 구출 효과 출력 함수: 학생을 구출했을 때 주변에 반짝이는 입자를 그린다.
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

// 커비 데미지 화면 효과 함수: 커비 체력이 낮거나 맞았을 때 화면 가장자리 효과를 그린다.
void DrawKirbyDamageFlash(Graphics& graphics)
{
    if (!isKirbyHit)
        return;

    int alpha = 80 + (kirbyHitTick % 6) * 18;
    if (alpha > 180) alpha = 180;

    SolidBrush flashBrush(Color(alpha, 255, 80, 120));
    graphics.FillEllipse(&flashBrush, kirbyX - 4, kirbyY - 4, kirbyW + 8, kirbyH + 8);
}

// 점수 HUD 출력 함수: 현재 점수와 플레이 시간을 화면 위쪽에 표시한다.
void DrawScoreHUD(Graphics& graphics, int screenW)
{
    if (g_isOpening || g_isStory)
        return;

    int boxW = 185;
    int boxH = 34;
    int boxX = screenW - boxW - 18;
    int boxY = 18;

    SolidBrush boxBrush(Color(145, 18, 18, 35));
    Pen boxPen(Color(210, 255, 230, 100), 2);
    graphics.FillRectangle(&boxBrush, boxX, boxY, boxW, boxH);
    graphics.DrawRectangle(&boxPen, boxX, boxY, boxW, boxH);

    FontFamily fontFamily(L"Arial");
    Font font(&fontFamily, 17, FontStyleBold, UnitPixel);
    SolidBrush textBrush(Color(245, 255, 245, 210));

    wchar_t scoreText[64];
    wsprintf(scoreText, L"SCORE  %d", g_gameScore);
    graphics.DrawString(scoreText, -1, &font, PointF((REAL)(boxX + 14), (REAL)(boxY + 8)), &textBrush);
}

// 게임 HUD 출력 함수: 체력바, 스테이지 이름, 구출 수 같은 기본 UI를 그린다.
void DrawGameHUD(Graphics& graphics)
{
    if (g_currentStage == 5)
        return;

    int panelH = 128;
    SolidBrush panelBrush(Color(150, 15, 20, 35));
    Pen panelPen(Color(220, 255, 235, 160), 2);
    graphics.FillRectangle(&panelBrush, 12, 12, 382, panelH);
    graphics.DrawRectangle(&panelPen, 12, 12, 382, panelH);

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

// 스테이지 메시지 출력 함수: 스테이지 제목이나 클리어 문구를 지정한 투명도로 그린다.
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

// 화면 전환 덮개 함수: 스테이지가 바뀔 때 화면을 어둡게 덮는 효과를 그린다.
void DrawTransitionOverlay(Graphics& graphics, int screenW, int screenH)
{
    if (g_stageFadeTick > 0)
    {
        int alpha = g_stageFadeTick * 255 / STAGE_FADE_TICK_MAX;
        if (alpha > 255) alpha = 255;
        SolidBrush fadeBrush(Color(alpha, 0, 0, 0));
        graphics.FillRectangle(&fadeBrush, 0, 0, screenW, screenH);
    }

    if (g_stageTitleTick > 0 && g_currentStage != 5)
    {
        int alpha = g_stageTitleTick > 18 ? 230 : g_stageTitleTick * 230 / 18;
        DrawStageMessage(graphics, screenW, 145, GetStageHudName(), alpha);

        if (g_currentStage == 1)
        {
            DrawStageMessage(graphics, screenW, 215, L"학생들을 구출해줘!", alpha);
        }
    }

    if (g_stageClearTick > 0)
    {
        int alpha = g_stageClearTick > 15 ? 245 : g_stageClearTick * 245 / 15;
        if (g_currentStage != 5)
            DrawStageMessage(graphics, screenW, 170, L"STAGE CLEAR", alpha);
    }

    if (g_currentStage == 5 && g_clearTimeSaved)
    {
        wchar_t clearTimeText[64];
        FormatClearTimeText(clearTimeText, g_clearTimeTick);
        DrawStageMessage(graphics, screenW, 78, clearTimeText, 235);
    }

    DrawClearResultPanel(graphics, screenW, screenH);
}

// 효과음 파일명 반환 함수: 효과음 번호에 맞는 wav 파일 이름을 반환한다.
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

// 효과음 별명 반환 함수: MCI 재생에 사용할 효과음 고유 이름을 반환한다.
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

// 파일 존재 확인 함수: 지정한 경로에 실제 파일이 있는지 검사한다.
bool IsFileExistsW(const wchar_t* path)
{
    DWORD attr = GetFileAttributesW(path);
    return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

// 실행 폴더 찾기 함수: exe가 있는 폴더 경로를 구해 사운드 파일 경로를 만들 때 사용한다.
void GetExeFolder(wchar_t* folder, size_t folderCount)
{
    if (folder == NULL || folderCount == 0)
        return;

    GetModuleFileNameW(NULL, folder, (DWORD)folderCount);
    folder[folderCount - 1] = 0;

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

// 효과음 경로 생성 함수: 실행 폴더와 sound 폴더, 파일명을 합쳐 실제 wav 경로를 만든다.
bool BuildGameSoundPath(const wchar_t* fileName, wchar_t* outPath, size_t outPathCount)
{
    if (fileName == NULL || outPath == NULL || outPathCount == 0)
        return false;

    wchar_t exeFolder[MAX_PATH];
    GetExeFolder(exeFolder, MAX_PATH);

    if (SUCCEEDED(StringCchPrintfW(outPath, outPathCount, L"sound\\%s", fileName)) && IsFileExistsW(outPath))
        return true;

    if (SUCCEEDED(StringCchPrintfW(outPath, outPathCount, L"..\\..\\sound\\%s", fileName)) && IsFileExistsW(outPath))
        return true;

    if (SUCCEEDED(StringCchPrintfW(outPath, outPathCount, L"%s\\sound\\%s", exeFolder, fileName)) && IsFileExistsW(outPath))
        return true;

    if (SUCCEEDED(StringCchPrintfW(outPath, outPathCount, L"%s\\..\\..\\sound\\%s", exeFolder, fileName)) && IsFileExistsW(outPath))
        return true;

    outPath[0] = 0;
    return false;
}

// 배경음악 모드 반환 함수: 현재 스테이지가 일반, 보스, 엔딩 중 어떤 BGM을 써야 하는지 알려준다.
int GetStageBgmMode()
{
    if (g_currentStage == 4)
        return 1;

    if (g_currentStage == 5)
        return 2;

    return 0;
}

// 파일 BGM 정지 함수: 파일로 재생 중인 배경음악을 멈추고 닫는다.
void StopFileBGM()
{
    mciSendStringW(L"stop bgm_music", NULL, 0, NULL);
    mciSendStringW(L"close bgm_music", NULL, 0, NULL);
}

// 리소스 BGM 재생 함수: exe 리소스에 들어있는 배경음악을 재생한다.
void PlayResourceBGM(int resourceId)
{
    StopFileBGM();
    PlaySound(NULL, NULL, 0);
    PlaySound(MAKEINTRESOURCE(resourceId), g_hInst, SND_RESOURCE | SND_ASYNC | SND_LOOP);
}

// 기본 스테이지 BGM 재생 함수: 일반 스테이지용 배경음악을 시작한다.
void PlayDefaultStageBGM()
{
    PlayResourceBGM(IDR_WAVE1);
}

// 파일 BGM 재생 함수: sound 폴더의 wav 파일을 찾아 반복 재생한다.
void PlayFileBGM(const wchar_t* fileName)
{
    wchar_t path[MAX_PATH];
    if (!BuildGameSoundPath(fileName, path, MAX_PATH))
        return;

    PlaySound(NULL, NULL, 0);
    StopFileBGM();

    wchar_t command[1024];
    if (FAILED(StringCchPrintfW(command, 1024, L"open \"%s\" type waveaudio alias bgm_music", path)))
        return;

    if (mciSendStringW(command, NULL, 0, NULL) != 0)
        return;

    mciSendStringW(L"setaudio bgm_music volume to 650", NULL, 0, NULL);
    mciSendStringW(L"play bgm_music repeat", NULL, 0, NULL);
}

// 스테이지 BGM 동기화 함수: 현재 스테이지에 맞는 음악이 재생 중인지 확인하고 필요하면 바꾼다.
void SyncStageBGM()
{
    int nextMode = GetStageBgmMode();

    if (g_starTransitionActive && nextMode == 2)
        return; // 클리어 음악은 별 전환이 끝나고 춤 화면이 보일 때 시작함

    if (g_currentBgmMode == nextMode)
        return;

    g_currentBgmMode = nextMode;

    if (nextMode == 1)
        PlayResourceBGM(IDR_WAVE_BOSS);
    else if (nextMode == 2)
        PlayResourceBGM(IDR_WAVE_CLEAR);
    else
        PlayDefaultStageBGM();
}

// 효과음 열기 함수: 효과음 파일을 MCI로 열어 재생 준비 상태로 만든다.
bool OpenGameSound(int soundId, const wchar_t* fileName, const wchar_t* alias)
{
    if (soundId < 0 || soundId >= SFX_COUNT)
        return false;

    if (g_sfxOpened[soundId])
        return true;

    wchar_t path[MAX_PATH];
    if (!BuildGameSoundPath(fileName, path, MAX_PATH))
        return false;

    wchar_t command[1024];
    if (FAILED(StringCchPrintfW(command, 1024, L"open \"%s\" type waveaudio alias %s", path, alias)))
        return false;

    if (mciSendStringW(command, NULL, 0, NULL) != 0)
        return false;

    g_sfxOpened[soundId] = true;
    return true;
}

// 효과음 재생 함수: 효과음 번호에 맞는 소리를 처음부터 재생한다.
void PlayGameSound(int soundId)
{
    const wchar_t* fileName = GetGameSoundFileName(soundId);
    const wchar_t* alias = GetGameSoundAlias(soundId);

    if (fileName == NULL || alias == NULL)
        return;

    if (!OpenGameSound(soundId, fileName, alias))
        return;

    wchar_t command[256];
    if (SUCCEEDED(StringCchPrintfW(command, 256, L"stop %s", alias)))
        mciSendStringW(command, NULL, 0, NULL);

    if (SUCCEEDED(StringCchPrintfW(command, 256, L"play %s from 0", alias)))
        mciSendStringW(command, NULL, 0, NULL);
}

// 전체 사운드 정지 함수: 열려 있는 효과음과 배경음악을 모두 닫는다.
void StopAllGameSounds()
{
    wchar_t command[256];

    for (int i = 0; i < SFX_COUNT; i++)
    {
        const wchar_t* alias = GetGameSoundAlias(i);
        if (alias == NULL)
            continue;

        if (SUCCEEDED(StringCchPrintfW(command, 256, L"stop %s", alias)))
            mciSendStringW(command, NULL, 0, NULL);

        if (SUCCEEDED(StringCchPrintfW(command, 256, L"close %s", alias)))
            mciSendStringW(command, NULL, 0, NULL);

        g_sfxOpened[i] = false;
    }
}

// 회복 아이템 삭제 함수: 화면에 있는 회복 아이템 정보를 모두 비활성화한다.
void ClearRecoveryItems()
{
    for (int i = 0; i < RECOVERY_ITEM_MAX; i++)
    {
        g_recoveryItems[i].active = false;
        g_recoveryItems[i].falling = false;
        g_recoveryItems[i].type = RECOVERY_ITEM_MEAT;
        g_recoveryItems[i].x = 0;
        g_recoveryItems[i].y = 0;
        g_recoveryItems[i].w = 34;
        g_recoveryItems[i].h = 34;
        g_recoveryItems[i].vy = 0.0f;
    }
}

// 회복 아이템 생성 함수: 지정한 종류와 위치에 회복 아이템을 하나 만든다.
void SpawnRecoveryItem(int type, int x, int y, bool falling)
{
    if (type < RECOVERY_ITEM_MEAT || type > RECOVERY_ITEM_LIFE)
        return;

    for (int i = 0; i < RECOVERY_ITEM_MAX; i++)
    {
        if (!g_recoveryItems[i].active)
        {
            g_recoveryItems[i].active = true;
            g_recoveryItems[i].falling = falling;
            g_recoveryItems[i].type = type;
            g_recoveryItems[i].x = x;
            g_recoveryItems[i].y = y;
            g_recoveryItems[i].w = (type == RECOVERY_ITEM_LIFE) ? 42 : 34;
            g_recoveryItems[i].h = (type == RECOVERY_ITEM_LIFE) ? 34 : 34;
            g_recoveryItems[i].vy = falling ? 5.0f : 0.0f;
            return;
        }
    }
}

// 회복 아이템 초기화 함수: 모든 회복 아이템을 지우고 생성 대기 시간도 처음으로 돌린다.
void ResetRecoveryItems()
{
    ClearRecoveryItems();
    g_bossItemDropTick = RandomRange(180, 260);

    if (g_currentStage == 1)
        SpawnRecoveryItem(RECOVERY_ITEM_MEAT, 1228, 155, false);
    else if (g_currentStage == 2)
        SpawnRecoveryItem(RECOVERY_ITEM_POTION, 1530, 255, false);
    else if (g_currentStage == 3)
        SpawnRecoveryItem(RECOVERY_ITEM_BIG_POTION, 1130, 518, false);
}

// 회복량 반환 함수: 아이템 종류에 따라 몇 퍼센트 체력을 회복할지 알려준다.
int GetRecoveryItemHealPercent(int type)
{
    if (type == RECOVERY_ITEM_MEAT) return 20;
    if (type == RECOVERY_ITEM_POTION) return 30;
    if (type == RECOVERY_ITEM_BIG_POTION) return 50;
    return 0;
}

// 회복 아이템 적용 함수: 아이템 종류에 맞춰 커비 체력을 회복시킨다.
void ApplyRecoveryItem(int type)
{
    if (type == RECOVERY_ITEM_LIFE) // 1UP 아이템: 최대 목숨보다 적을 때만 목숨을 올림.
    {
        if (g_kirbyLives < KIRBY_MAX_LIVES)
            g_kirbyLives++;
        return;
    }

    int healPercent = GetRecoveryItemHealPercent(type); // 고기/물약 아이템의 회복 비율을 가져옴.
    if (healPercent <= 0)
        return;

    kirbyHP += kirbyMaxHP * healPercent / 100; // 실제 체력을 먼저 회복시킴.
    if (kirbyHP > kirbyMaxHP) // 최대 체력을 넘지 않게 막음.
        kirbyHP = kirbyMaxHP;

    // 여기서는 kirbyDisplayHP를 건드리지 않음. UpdateHPBarAnimation()이 천천히 채움.
}

// 보스전 회복 아이템 랜덤 생성 함수: 보스전 중 일정 조건에서 무작위 회복 아이템을 떨어뜨린다.
void SpawnRandomBossRecoveryItem()
{
    int r = RandomRange(0, 99);
    int type = RECOVERY_ITEM_MEAT;

    if (r < 8)
        type = RECOVERY_ITEM_LIFE;
    else if (r < 28)
        type = RECOVERY_ITEM_BIG_POTION;
    else if (r < 62)
        type = RECOVERY_ITEM_POTION;
    else
        type = RECOVERY_ITEM_MEAT;

    int x = RandomRange(80, BG_PART_W - 120);
    SpawnRecoveryItem(type, x, -45, true);
}

// 회복 아이템 갱신 함수: 아이템 낙하, 충돌, 획득, 수명 감소를 처리한다.
void UpdateRecoveryItems()
{
    if (isGameOver || g_retryActive || g_currentStage == 5)
        return;

    if (g_currentStage == 4 && g_boss.active && !g_bossDeadEffect)
    {
        g_bossItemDropTick--;
        if (g_bossItemDropTick <= 0)
        {
            SpawnRandomBossRecoveryItem();
            g_bossItemDropTick = RandomRange(220, 340);
        }
    }

    RECT kirbyRc = GetKirbyBodyRect();

    for (int i = 0; i < RECOVERY_ITEM_MAX; i++)
    {
        if (!g_recoveryItems[i].active)
            continue;

        if (g_recoveryItems[i].falling)
        {
            g_recoveryItems[i].y += (int)g_recoveryItems[i].vy;
            if (g_recoveryItems[i].vy < 9.0f)
                g_recoveryItems[i].vy += 0.18f;
        }

        RECT rc;
        rc.left = g_recoveryItems[i].x;
        rc.top = g_recoveryItems[i].y;
        rc.right = g_recoveryItems[i].x + g_recoveryItems[i].w;
        rc.bottom = g_recoveryItems[i].y + g_recoveryItems[i].h;

        if (IsRectHit(kirbyRc, rc))
        {
            ApplyRecoveryItem(g_recoveryItems[i].type);
            g_recoveryItems[i].active = false;
            continue;
        }

        if (g_recoveryItems[i].y > WORLD_H + 80)
            g_recoveryItems[i].active = false;
    }
}

// 회복 아이템 출력 함수: 활성화된 회복 아이템을 현재 위치에 그린다.
void DrawRecoveryItems(Graphics& graphics)
{
    for (int i = 0; i < RECOVERY_ITEM_MAX; i++)
    {
        if (!g_recoveryItems[i].active)
            continue;

        int type = g_recoveryItems[i].type;
        if (type < RECOVERY_ITEM_MEAT || type > RECOVERY_ITEM_LIFE)
            continue;

        Image* frame = g_recoveryItemFrames[type];
        if (frame != NULL)
            DrawWorldImage(graphics, frame, g_recoveryItems[i].x, g_recoveryItems[i].y, g_recoveryItems[i].w, g_recoveryItems[i].h);
    }
}

// 스테이지 투사체 초기화 함수: 커비/몬스터/보스 공격 투사체와 능력 상태를 스테이지 시작 상태로 정리한다.
void ResetStageProjectiles()
{
    isPowerProjectileActive = false;
    isFireBallActive = false;
    isFireBreath = false;
    isFireAttackPose = false;
    isBombAttack = false;
    isHammerAttack = false;
    hammerAttackFrameIndex = 0;
    hammerAttackTick = 0;
    hammerAttackHitDone = false;
    isHammerInvincibleSkill = false;
    hammerInvincibleSkillTick = 0;
    hammerInvincibleFrameIndex = 0;
    hammerInvincibleFrameTick = 0;
    hammerInvincibleCooldownTick = 0;
    isSparkAttack = false;
    sparkAttackFrameIndex = 0;
    sparkAttackTick = 0;
    sparkAttackHitDone = false;
    isSparkSpecialAttack = false;
    sparkSpecialAttackFrameIndex = 0;
    sparkSpecialAttackTick = 0;
    sparkSpecialAttackHitDone = false;
    for (int i = 0; i < SPARK_LIGHTNING_MAX; i++)
        g_sparkLightnings[i].active = false;
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

// 스테이지 장치 초기화 함수: 바람, 낙석, 어둠 효과 같은 스테이지별 장치를 처음 상태로 돌린다.
void ResetStageGimmicks()
{
    g_windActive = false;
    g_windDir = 1;
    g_windTick = 0;
    g_windCooldownTick = WIND_COOLDOWN;

    g_fallingRockSpawnTick = 25;
    ResetRecoveryItems();
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

// 바람으로 커비 밀기 함수: 1스테이지 바람이 활성화되면 커비를 바람 방향으로 조금씩 이동시킨다.
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

// 스테이지 바람 갱신 함수: 바람이 켜지고 꺼지는 시간과 커비 밀림을 처리한다.
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

// 낙석 착지 위치 계산 함수: 돌이 떨어질 X좌표에서 어느 높이에 멈춰야 하는지 계산한다.
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

// 낙석 생성 함수: 비어 있는 낙석 칸을 찾아 위에서 떨어지는 돌을 만든다.
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

// 낙석 갱신 함수: 떨어지는 돌의 위치, 바닥 충돌, 커비 피격, 사라지는 시간을 처리한다.
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
        g_fallingRockSpawnTick = RandomRange(38, 58);
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

// 스테이지 장치 전체 갱신 함수: 바람, 낙석, 어둠 효과처럼 스테이지별 특수 장치를 처리한다.
void UpdateStageGimmicks(HWND hWnd)
{
    (void)hWnd;
    UpdateFallingRocks();
    UpdateRecoveryItems();
}

// 낙석 출력 함수: 현재 떨어지거나 바닥에 놓인 돌을 화면에 그린다.
void DrawFallingRocks(Graphics& graphics)
{
    if (g_currentStage != 3)
        return;

    for (int i = 0; i < FALLING_ROCK_MAX; i++)
    {
        if (!g_fallingRocks[i].active || g_fallingRocks[i].warning)
            continue;

        int x = g_fallingRocks[i].x;
        int y = g_fallingRocks[i].y;
        int w = g_fallingRocks[i].w;
        int h = g_fallingRocks[i].h;

        if (g_stage3RockFrame != NULL)
        {
            DrawWorldImage(graphics, g_stage3RockFrame, x, y, w, h);
        }
        else
        {
            SolidBrush rockBrush(Color(245, 95, 95, 110));
            Pen rockPen(Color(230, 45, 45, 60), 2);
            graphics.FillEllipse(&rockBrush, x, y, w, h);
            graphics.DrawEllipse(&rockPen, x, y, w, h);
        }
    }
}

// 스테이지 장치 출력 함수: 낙석, 어둠 덮개 등 스테이지별 특수 효과를 그린다.
void DrawStageGimmicks(Graphics& graphics, int screenW, int screenH)
{
    DrawFallingRocks(graphics);
    DrawRecoveryItems(graphics);
}

// 어둠 시야 효과 출력 함수: 특정 스테이지에서 화면을 어둡게 덮고 커비 주변만 보이게 한다.
void DrawDarkVisionOverlay(Graphics& graphics, int screenW, int screenH)
{
    if (g_currentStage != 2)
        return;

    int centerX = kirbyX - cameraX + kirbyW / 2;
    int centerY = kirbyY + kirbyH / 2;
    int radius = 135;

    // 불 커비는 악몽 어둠을 더 넓게 밝힘.
    if (isFireKirby || kirbyAbilityType == 1)
        radius = 205;
    else if (isSparkKirby || kirbyAbilityType == 4)
        radius = 185;

    GraphicsPath viewPath;
    viewPath.AddEllipse(centerX - radius, centerY - radius, radius * 2, radius * 2);

    Region darkRegion(Rect(0, 0, screenW, screenH));
    darkRegion.Exclude(&viewPath);

    SolidBrush darkBrush(Color(255, 0, 0, 0));
    graphics.FillRegion(&darkBrush, &darkRegion);
}

// 커비 플레이 상태 초기화 함수: 재시작이나 스테이지 이동 때 커비의 이동, 능력, 피격 상태를 정리한다.
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
    balloonCooldownTick = 0;
    spaceKeyHeld = false;
    spaceFrameIndex = 0;
    spaceStartFrameDone = false;
    fireBalloonFrameIndex = 0;
    fireBalloonStartFrameDone = false;
    bombBalloonFrameIndex = 0;
    bombBalloonStartFrameDone = false;
    hammerBalloonFrameIndex = 0;
    hammerBalloonStartFrameDone = false;
    sparkBalloonFrameIndex = 0;
    sparkBalloonStartFrameDone = false;
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

// 커비 스테이지 시작 위치 함수: 현재 스테이지 번호에 맞는 시작 좌표로 커비를 배치한다.
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

// 현재 스테이지 재시작 함수: 생명 감소 후 같은 스테이지를 처음 상태로 다시 시작한다.
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

// 재시도 시작 함수: 게임오버 후 바로 복귀하지 않고 카운트다운 상태로 들어간다.
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
    balloonCooldownTick = 0;
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

    g_totalDeathCount++;
    if (g_currentStage == 4)
        g_bossDeathCount++;

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

// 커비 재등장 함수: 저장된 안전 위치나 스테이지 시작 위치에 커비를 다시 배치한다.
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

// 재시도 카운트 갱신 함수: 카운트다운 시간이 끝나면 커비를 다시 살리거나 최종 게임오버를 처리한다.
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

// 조작 안내 출력 함수: 게임 시작 직후 키 설명을 일정 시간 동안 화면에 보여준다.
void DrawControlGuide(Graphics& graphics, int screenW)
{
    if (g_controlGuideTick <= 0)
        return;

    if (g_currentStage != 1 && !g_controlGuideForced)
        return;

    int boxW = 230;
    int boxH = 194;
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
    graphics.DrawString(L"A / D  이동", -1, &lineFont, PointF((REAL)(boxX + 18), (REAL)(boxY + 36)), &lineBrush);
    graphics.DrawString(L"Space  풍선 날기", -1, &lineFont, PointF((REAL)(boxX + 18), (REAL)(boxY + 58)), &lineBrush);
    graphics.DrawString(L"K  공격", -1, &lineFont, PointF((REAL)(boxX + 18), (REAL)(boxY + 80)), &lineBrush);
    graphics.DrawString(L"I  특별 공격", -1, &lineFont, PointF((REAL)(boxX + 18), (REAL)(boxY + 102)), &lineBrush);
    graphics.DrawString(L"O  변신 해제", -1, &lineFont, PointF((REAL)(boxX + 18), (REAL)(boxY + 124)), &lineBrush);
    graphics.DrawString(L"U  상호작용", -1, &lineFont, PointF((REAL)(boxX + 18), (REAL)(boxY + 146)), &lineBrush);
    graphics.DrawString(L"ESC  일시정지", -1, &lineFont, PointF((REAL)(boxX + 18), (REAL)(boxY + 168)), &lineBrush);
}

// 일시정지 메뉴 출력 함수: Continue, Restart, Exit 같은 메뉴를 화면 중앙에 그린다.
void DrawPauseMenu(Graphics& graphics, int screenW, int screenH)
{
    if (!g_isPaused)
        return;

    SolidBrush darkBrush(Color(170, 0, 0, 0));
    graphics.FillRectangle(&darkBrush, 0, 0, screenW, screenH);

    int boxW = 360;
    int boxH = 310;
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

    const wchar_t* items[PAUSE_MENU_COUNT] = { L"Continue", L"Restart Stage", L"Exit Game", L"Controls 보기" };
    for (int i = 0; i < PAUSE_MENU_COUNT; i++)
    {
        int y = boxY + 88 + i * 48;
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

// 재시도 화면 출력 함수: 죽은 뒤 카운트다운이나 최종 게임오버 안내를 화면에 그린다.
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
// 커비 이동 관련 상태를 갱신하는 함수: 위치 이동, 풍선 제한, 대시 효과, 스페이스 키 해제를 순서대로 처리한다.
void UpdatePlayerMovementState(HWND hWnd)
{
    if (!isDragging && g_currentStage != 5)
    {
        UpdateKirbyPosition(hWnd);
    }

    UpdateBalloonLimit();
    UpdateDashWindFrame();
    UpdateSpaceRelease();
}

// 커비 능력과 공격 상태를 갱신하는 함수: 흡수, 파워, 속성 공격, 능력별 이동을 한곳에서 처리한다.
void UpdatePlayerAbilityState()
{
    UpdateAbsorbFrontEffect();
    UpdatePowerWait();
    UpdatePowerAttack();
    UpdatePowerDigest();
    UpdateFireKirbyStates();
    UpdateHammerAttack();
    UpdateHammerInvincibleSkill();
    UpdateSparkAttack();
    UpdateSparkSpecialAttack();
    UpdatePowerProjectile();
    UpdateAbilityStar();
}

// 커비 피격과 체력 상태를 갱신하는 함수: 맞았을 때 깜빡임, 상태 이상, HP바 애니메이션을 처리한다.
void UpdatePlayerHpState()
{
    UpdateKirbyHitEffect();
    UpdateKirbyStatusEffects();
    UpdateHPBarAnimation();
}

// 안전 위치를 저장하는 함수: 커비가 바닥 위에 있을 때 낙사 복귀용 위치를 기억한다.
void SaveKirbySafePosition()
{
    if (!isGameOver && !g_retryActive && isOnGround && kirbyY < WORLD_H)
    {
        g_lastSafeKirbyX = kirbyX;
        g_lastSafeKirbyY = kirbyY;
    }
}

// 플레이어 전체 갱신 함수: 이동 -> 능력/공격 -> HP -> 안전 위치 저장 순서로 실행된다.
void UpdatePlayer(HWND hWnd)
{
    // 발표용 순서: 이동 처리 -> 능력/공격 처리 -> 피격/HP 처리 -> 안전 위치 저장
    UpdatePlayerMovementState(hWnd);
    UpdatePlayerAbilityState();
    UpdatePlayerHpState();
    SaveKirbySafePosition();
}
// 스테이지 상태 갱신 함수: 구출, 문, 폭탄/불 공격, 보스, 엔딩 스테이지를 처리한다.
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

// 공격 충돌 검사 함수: 커비의 각 능력 공격이 몬스터나 보스에게 맞았는지 확인한다.
void CheckCollision()
{
    CheckPowerProjectileHitMonsters();
    CheckFireAttacksHitMonsters();
    CheckHammerAttackHitMonsters();
    CheckSparkAttackHitMonsters();
    CheckSparkSpecialAttackHitMonsters();
    CheckKirbyAttacksHitBoss();
}

// 몬스터 갱신 함수: 배열에 들어있는 모든 몬스터의 이동과 상태를 갱신한다.
void UpdateMonster()
{
    for (int i = 0; i < MONSTER_COUNT; i++)
    {
        g_monsters[i].Update();
    }
}

// 커비 피격 검사 함수: 몬스터나 적 투사체가 커비에게 닿았는지 확인한다.
void CheckKirbyCollision()
{
    CheckKirbyHitByMonsters();
    CheckEnemyFireBallsHitKirby();
}

// HUD 출력 함수: 체력, 점수, 조작 안내, 보스 HP, 화면 효과를 그린다.
void DrawHUD(Graphics& graphics, int screenW, int screenH)
{
    DrawGameHUD(graphics);
    DrawScoreHUD(graphics, screenW);
    DrawControlGuide(graphics, screenW);
    DrawKirbyStatusUI(graphics);
    DrawBossHpBar(graphics);
    DrawBossPatternText(graphics);
    DrawTransitionOverlay(graphics, screenW, screenH);
}

// 발표용 게임 루프 정리:
// 입력은 WndProc에서 받고, 타이머에서는 상태 갱신 -> 충돌 검사 -> 화면 다시 그리기 순서로 처리함.
// 풍선 애니메이션 초기화 함수: 일반/불/폭탄/해머/스파크 풍선 프레임을 처음 상태로 돌린다.
void ResetAllBalloonAnimationFrames()
{
    spaceFrameIndex = 0;
    spaceStartFrameDone = false;
    fireBalloonFrameIndex = 0;
    fireBalloonStartFrameDone = false;
    bombBalloonFrameIndex = 0;
    bombBalloonStartFrameDone = false;
    hammerBalloonFrameIndex = 0;
    hammerBalloonStartFrameDone = false;
    sparkBalloonFrameIndex = 0;
    sparkBalloonStartFrameDone = false;
}

// 스토리 종료 후 게임 시작 함수: 스토리를 끄고 커비 상태와 카메라, 플레이 타이머를 초기화한다.
void FinishStoryAndStartGame(HWND hWnd)
{
    g_isStory = false;
    g_storyFrameIndex = STORY_FRAME_COUNT - 1;
    StopMove();
    isSpace = false;
    isSpaceRelease = false;
    balloonTick = 0;
    balloonCooldownTick = 0;
    spaceKeyHeld = false;
    ResetAllBalloonAnimationFrames();
    UpdateCamera(hWnd);
    StartStageTransitionEffect();
    StartPlayTimer();
    g_controlGuideTick = CONTROL_GUIDE_TICK_MAX;
}

// 오프닝 타이머 함수: 1번 타이머가 올 때 오프닝 시간을 증가시키고 화면을 다시 그린다.
void UpdateOpeningTimer(HWND hWnd, WPARAM wParam)
{
    if (wParam != 1)
        return;

    g_openingTick++;
    InvalidateRect(hWnd, NULL, FALSE);
}

// 스토리 타이머 함수: 일정 시간마다 스토리 이미지를 넘기고, 마지막이면 게임을 시작한다.
void UpdateStoryTimer(HWND hWnd, WPARAM wParam)
{
    if (wParam != 1)
        return;

    g_storyTick++;

    if (g_storyTick >= STORY_FRAME_DURATION)
    {
        g_storyTick = 0;
        g_storyFrameIndex++;

        if (g_storyFrameIndex >= STORY_FRAME_COUNT)
        {
            FinishStoryAndStartGame(hWnd);
        }

        InvalidateRect(hWnd, NULL, FALSE);
    }
}

// 별 전환 타이머 함수: 스테이지 이동 중 별 전환 효과와 배경음악을 갱신한다.
void UpdateStarTransitionTimer(HWND hWnd, WPARAM wParam)
{
    if (wParam != 1)
        return;

    UpdateStarStageTransition(hWnd);
    SyncStageBGM();
    InvalidateRect(hWnd, NULL, FALSE);
}

// 일시정지/재시도 타이머 함수: 멈춘 상태에서는 필요한 화면 또는 재시도 카운트만 갱신한다.
void UpdatePauseRetryTimer(HWND hWnd, WPARAM wParam)
{
    if (wParam != 1)
        return;

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

// 게임오버 대기 함수: 체력이 0이 된 뒤 잠깐 기다렸다가 재시도 화면으로 넘어간다.
bool UpdateGameOverDelay(HWND hWnd)
{
    if (!isGameOver || g_gameOverHandled)
        return false;

    gameOverTick++;

    if (gameOverTick >= GAME_OVER_DELAY)
    {
        StartRetrySequence();
        InvalidateRect(hWnd, NULL, FALSE);
        return true;
    }

    return false;
}

// 실제 게임 루프 함수: 플레이어, 스테이지, 충돌, 몬스터, 카메라, 화면 갱신을 한 번에 처리한다.
void UpdateGameLoop(HWND hWnd)
{
    UpdatePlayTimer();
    UpdatePlayer(hWnd);
    UpdateStageGimmicks(hWnd);
    UpdateStage(hWnd);
    SyncStageBGM();
    CheckCollision();
    UpdateMonster();
    CheckKirbyCollision();

    if (UpdateGameOverDelay(hWnd))
        return;

    UpdateScreenEffects();
    UpdateCamera(hWnd);

    InvalidateRect(hWnd, NULL, FALSE);
}

// 걷기 애니메이션 초기화 함수: 커비 상태별 걷기 프레임을 0으로 되돌린다.
void ResetWalkAnimationFrames()
{
    walkFrameIndex = 0;
    powerWalkFrameIndex = 0;
    fireWalkFrameIndex = 0;
    bombWalkFrameIndex = 0;
    hammerWalkFrameIndex = 0;
    sparkWalkFrameIndex = 0;
}

// 걷기 애니메이션 타이머 함수: 이동 중인 커비의 상태에 맞춰 걷기 프레임을 넘긴다.
void UpdateWalkAnimationTimer()
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
        else if (isHammerKirby)
        {
            hammerWalkFrameIndex++;

            if (hammerWalkFrameIndex >= HAMMER_WALK_FRAME_COUNT)
            {
                hammerWalkFrameIndex = 0;
                sparkWalkFrameIndex = 0;
            }
        }
        else if (isSparkKirby)
        {
            sparkWalkFrameIndex++;

            if (sparkWalkFrameIndex >= SPARK_WALK_FRAME_COUNT)
            {
                sparkWalkFrameIndex = 0;
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
        ResetWalkAnimationFrames();
    }
}

// 풍선 애니메이션 타이머 함수: 풍선 상태일 때 커비 능력별 풍선 프레임을 넘긴다.
void UpdateBalloonAnimationTimer()
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
        else if (isHammerKirby)
        {
            if (!hammerBalloonStartFrameDone)
            {
                hammerBalloonFrameIndex = 0;
                hammerBalloonStartFrameDone = true;
            }
            else
            {
                hammerBalloonFrameIndex++;

                if (hammerBalloonFrameIndex >= 2)
                    hammerBalloonFrameIndex = 0;
            }
        }
        else if (isSparkKirby)
        {
            if (!sparkBalloonStartFrameDone)
            {
                sparkBalloonFrameIndex = 0;
                sparkBalloonStartFrameDone = true;
            }
            else
            {
                sparkBalloonFrameIndex++;

                if (sparkBalloonFrameIndex >= 2)
                    sparkBalloonFrameIndex = 0;
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
        ResetAllBalloonAnimationFrames();
    }
}

// 흡수 애니메이션 타이머 함수: 흡수 중일 때 입 벌리는 프레임을 마지막 프레임까지 진행한다.
void UpdateAbsorbAnimationTimer()
{
    if (!isAbsorb)
        return;

    if (absorbFrameIndex < absorbFrameCount - 1)
    {
        absorbFrameIndex++;
    }
    else
    {
        absorbFrameIndex = absorbFrameCount - 1;
    }
}

// 몬스터 애니메이션 타이머 함수: 모든 몬스터의 애니메이션 프레임을 한 칸씩 넘긴다.
void UpdateMonsterAnimationTimer()
{
    for (int i = 0; i < MONSTER_COUNT; i++)
    {
        g_monsters[i].NextFrame();
    }
}

// 타이머 분배 함수: 오프닝, 스토리, 전환, 일시정지, 실제 게임 타이머를 상태별로 나눠 호출한다.
void HandleGameTimer(HWND hWnd, WPARAM wParam)
{
    if (g_isOpening)
    {
        UpdateOpeningTimer(hWnd, wParam);
        return;
    }

    if (g_isStory)
    {
        UpdateStoryTimer(hWnd, wParam);
        return;
    }

    if (g_starTransitionActive)
    {
        UpdateStarTransitionTimer(hWnd, wParam);
        return;
    }

    if (g_isPaused || g_retryActive || g_finalGameOver)
    {
        UpdatePauseRetryTimer(hWnd, wParam);
        return;
    }

    if (wParam == 1)
    {
        UpdateGameLoop(hWnd);
    }
    else if (wParam == 2)
    {
        UpdateWalkAnimationTimer();
    }
    else if (wParam == 3)
    {
        UpdateBalloonAnimationTimer();
    }
    else if (wParam == 5)
    {
        UpdateAbsorbAnimationTimer();
    }
    else if (wParam == 7)
    {
        UpdateMonsterAnimationTimer();
    }
}
// 화면 가장자리 박스 출력 함수: 피격, 위험, 보스 광폭 같은 상태를 화면 테두리 색으로 표현한다.
void DrawEdgeBox(Graphics& graphics, int screenW, int screenH, int alpha, int red, int green, int blue, int startInset, int layerCount)
{
    if (alpha <= 0)
        return;

    for (int i = 0; i < layerCount; i++)
    {
        int curAlpha = alpha * (layerCount - i) / layerCount;
        int inset = startInset + i * 5;
        Pen edgePen(Color(curAlpha, red, green, blue), 5);
        graphics.DrawRectangle(&edgePen, inset, inset, screenW - inset * 2 - 1, screenH - inset * 2 - 1);
    }
}

// 화면 가장자리 효과 출력 함수: 체력 낮음, 보스 광폭, 상태 이상에 맞는 테두리 효과를 그린다.
void DrawScreenEdgeEffects(Graphics& graphics, int screenW, int screenH)
{
    bool bossBerserk = (g_currentStage == 4 && IsBossBerserk());

    // 악몽 스테이지에는 2스테이지 시야 마스크와 별개로 어두운 테두리를 씌움.
    if (g_currentStage == 2 || g_currentStage == 3 || g_currentStage == 4)
    {
        int darkLayers = bossBerserk ? 2 : 5;
        int purpleLayers = bossBerserk ? 1 : 4;
        DrawEdgeBox(graphics, screenW, screenH, 60, 0, 0, 0, 0, darkLayers);
        DrawEdgeBox(graphics, screenW, screenH, 40, 35, 0, 65, 10, purpleLayers);
    }

    if (bossBerserk)
    {
        int pulse = (g_edgeEffectTick / 4) % 24;
        if (pulse > 12)
            pulse = 24 - pulse;

        int alpha = 75 + pulse * 4;
        DrawEdgeBox(graphics, screenW, screenH, alpha, 150, 40, 255, 6, 1);

        // 광폭화 시간이 길어질수록 보라 안개가 조금씩 안쪽으로 들어옴.
        int fogLevel = g_bossBerserkFogTick / 700;
        if (fogLevel > 3)
            fogLevel = 3;

        int fogAlpha = 28 + fogLevel * 8;
        int fogInset = 48 - fogLevel * 8;
        if (fogInset < 12)
            fogInset = 12;

        DrawEdgeBox(graphics, screenW, screenH, fogAlpha, 95, 20, 170, fogInset, 1);
    }

    if (!isGameOver && !g_retryActive && kirbyMaxHP > 0 && kirbyHP <= kirbyMaxHP / 5)
    {
        int pulse = (g_edgeEffectTick / 2) % 24;
        if (pulse > 12)
            pulse = 24 - pulse;

        int alpha = 70 + pulse * 6;
        int dangerLayers = bossBerserk ? 2 : 6;
        DrawEdgeBox(graphics, screenW, screenH, alpha, 255, 20, 20, 16, dangerLayers);
    }
}

// 디버그 정보 출력 함수: 개발 중 확인용으로 충돌 박스나 좌표 정보를 화면에 그린다.
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

// 커비 HP바 출력 함수: 현재 체력 비율에 맞춰 HP바 이미지를 채워 그린다.
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

// 오프닝 안내 출력 함수: 시작 화면에서 SPACE 입력 안내 문구를 깜빡이며 보여준다.
void DrawOpeningPressSpace(Graphics& graphics, int screenW, int screenH)
{
    // 오프닝 화면에서만 시작 방법을 알려줌.
    if (!g_isOpening)
        return;

    if ((g_openingTick / 12) % 2 == 1)
        return;

    FontFamily fontFamily(L"Arial");
    Font font(&fontFamily, 32, FontStyleBold, UnitPixel);
    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);

    RectF rect(0.0f, (REAL)(screenH - 96), (REAL)screenW, 48.0f);
    SolidBrush shadowBrush(Color(220, 0, 0, 0));
    SolidBrush textBrush(Color(245, 255, 245, 190));

    RectF shadowRect(2.0f, (REAL)(screenH - 94), (REAL)screenW, 48.0f);
    graphics.DrawString(L"Press Space", -1, &font, shadowRect, &format, &shadowBrush);
    graphics.DrawString(L"Press Space", -1, &font, rect, &format, &textBrush);
}

// 전체 화면 출력 함수: 배경, 스테이지, 몬스터, 커비, 이펙트, UI를 순서대로 그린다.
void DrawScene(HDC hdc, HWND hWnd)
{
    RECT rt;
    GetClientRect(hWnd, &rt);

    if (!PrepareBackBuffer(hdc, rt.right, rt.bottom))
        return;

    HDC memDC = g_backDC;

    // 이전 프레임 잔상이 남지 않게 먼저 전체를 지움
    FillRect(memDC, &rt, (HBRUSH)GetStockObject(BLACK_BRUSH));

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
            FillRect(memDC, &rt, (HBRUSH)GetStockObject(BLACK_BRUSH));

            SetBkMode(memDC, TRANSPARENT);
            SetTextColor(memDC, RGB(255, 255, 255));

        }

        DrawOpeningPressSpace(graphics, rt.right, rt.bottom);

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
            FillRect(memDC, &rt, (HBRUSH)GetStockObject(BLACK_BRUSH));

            SetBkMode(memDC, TRANSPARENT);
            SetTextColor(memDC, RGB(255, 255, 255));

        }

        BitBlt(hdc, 0, 0, rt.right, rt.bottom, memDC, 0, 0, SRCCOPY);

        return;
    }

    // 5스테이지는 138번 클리어 배경을 사용함

    int cameraDrawOffsetX = GetCameraDrawOffsetX();
    int cameraDrawOffsetY = GetCameraDrawOffsetY();

    // 카메라 효과는 월드 그리기에만 적용함.
    int bg1X = -cameraX + cameraDrawOffsetX;
    int bg2X = BG_PART_W - cameraX + cameraDrawOffsetX;

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
        graphics.DrawImage(bg1Image, bg1X, cameraDrawOffsetY, BG_PART_W, BG_PART_H);
    }
    else if (bg1Image == NULL && g_currentStage != 5)
    {
        // 배경 리소스 로드 실패 시 파란 화면으로 보이지 않게 검은색으로 처리
        FillRect(memDC, &rt, (HBRUSH)GetStockObject(BLACK_BRUSH));

        SetBkMode(memDC, TRANSPARENT);
        SetTextColor(memDC, RGB(255, 80, 180));
        if (g_currentStage == 3)
            TextOut(memDC, 20, 20, L"3스테이지 배경 IDB_PNG90 / IDB_PNG91 로드 실패", 42);
        else if (g_currentStage == 4)
            TextOut(memDC, 20, 20, L"4스테이지 배경 IDB_PNG92 로드 실패", 31);
    }

    if (bg2Image != NULL && bg2X + BG_PART_W > 0 && bg2X < rt.right)
    {
        graphics.DrawImage(bg2Image, bg2X, cameraDrawOffsetY, BG_PART_W, BG_PART_H);
    }

    DrawStageAtmosphereEffects(graphics, g_currentStage, cameraX - cameraDrawOffsetX, rt.right, rt.bottom);

    // 커비/몬스터/이펙트는 전부 월드 좌표로 움직이고,
    // 그릴 때만 -cameraX만큼 이동해서 화면에 표시
    GraphicsState worldState = graphics.Save();

    graphics.TranslateTransform((REAL)(-cameraX + cameraDrawOffsetX), (REAL)cameraDrawOffsetY);

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
    DrawSparkLightning(graphics);
    DrawBombObjects(graphics);
    DrawBombExplosions(graphics);
    DrawEnemyFireBalls(graphics);
    DrawBossObjects(graphics);

    bool isKirbyAttackMotion = isFireAttackPose || isBombAttack || isSparkSpecialAttack || isSparkAttack || isHammerAttack || isPowerAttack;
    bool hideKirbyBlink = isKirbyHit && !isKirbyAttackMotion && g_currentStage != 5 && ((kirbyHitTick / 3) % 2 == 1);
    if (!hideKirbyBlink)
    {
    if (g_currentStage == 5)
    {
        DrawDanceKirby(graphics);
    }
    else if (isKirbyHit && !isKirbyAttackMotion)
    {
        Image* hitFrame = g_kirbyHitFrame;

        if (isSparkKirby && g_sparkKirbyHitFrame != NULL)
            hitFrame = g_sparkKirbyHitFrame;
        else if (isHammerKirby && g_hammerKirbyHitFrame != NULL)
            hitFrame = g_hammerKirbyHitFrame;
        else if ((isBombKirby || isBombTransform) && g_bombKirbyHitFrame != NULL)
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
    else if (isSparkSpecialAttack && g_sparkSpecialAttackFrames[0] != NULL)
    {
        DrawKirbyImage(graphics, g_sparkSpecialAttackFrames[0]);
    }
    else if (isSparkAttack && g_sparkAttackFrames[sparkAttackFrameIndex] != NULL)
    {
        Image* sparkAttackFrame = g_sparkAttackFrames[sparkAttackFrameIndex];
        int sparkDrawW = kirbyW;
        int sparkDrawH = kirbyH;

        if (sparkAttackFrameIndex > 0)
        {
            sparkDrawW = SPARK_ATTACK_DRAW_W;
            sparkDrawH = SPARK_ATTACK_DRAW_H;
        }

        int sparkDrawX = kirbyX + kirbyW / 2 - sparkDrawW / 2;
        int sparkDrawY = kirbyY + kirbyH / 2 - sparkDrawH / 2;

        if (kirbyFaceLeft)
            DrawImageFlipX(graphics, sparkAttackFrame, sparkDrawX, sparkDrawY, sparkDrawW, sparkDrawH);
        else
            DrawWorldImage(graphics, sparkAttackFrame, sparkDrawX, sparkDrawY, sparkDrawW, sparkDrawH);
    }
    else if (isHammerAttack && g_hammerAttackFrames[hammerAttackFrameIndex] != NULL)
    {
        DrawKirbyImage(graphics, g_hammerAttackFrames[hammerAttackFrameIndex]);
    }
    else if (isPowerAttack && g_powerAttackFrame != NULL)
    {
        DrawKirbyImage(graphics, g_powerAttackFrame);
    }
    else if (isSpaceRelease)
    {
        if (isSparkKirby && g_sparkBalloonStartFrame != NULL)
        {
            DrawKirbyImage(graphics, g_sparkBalloonStartFrame);
        }
        else if (isHammerKirby && g_hammerBalloonStartFrame != NULL)
        {
            DrawKirbyImage(graphics, g_hammerBalloonStartFrame);
        }
        else if (isBombKirby && g_bombBalloonStartFrame != NULL)
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
        if (isSparkKirby)
        {
            if (!sparkBalloonStartFrameDone && g_sparkBalloonStartFrame != NULL)
            {
                DrawKirbyImage(graphics, g_sparkBalloonStartFrame);
            }
            else if (g_sparkBalloonFrames[sparkBalloonFrameIndex] != NULL)
            {
                DrawKirbyImage(graphics, g_sparkBalloonFrames[sparkBalloonFrameIndex]);
            }
        }
        else if (isFireKirby)
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
        else if (isHammerKirby)
        {
            if (!hammerBalloonStartFrameDone && g_hammerBalloonStartFrame != NULL)
            {
                DrawKirbyImage(graphics, g_hammerBalloonStartFrame);
            }
            else if (g_hammerBalloonFrames[hammerBalloonFrameIndex] != NULL)
            {
                DrawKirbyImage(graphics, g_hammerBalloonFrames[hammerBalloonFrameIndex]);
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

        if (isSparkKirby && g_sparkCrouchFrame != NULL)
            crouchImage = g_sparkCrouchFrame;
        else if (isFireKirby && g_fireCrouchFrame != NULL)
            crouchImage = g_fireCrouchFrame;
        else if (isBombKirby && g_bombCrouchFrame != NULL)
            crouchImage = g_bombCrouchFrame;
        else if (isHammerKirby && g_hammerCrouchFrame != NULL)
            crouchImage = g_hammerCrouchFrame;

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
    else if (isHammerKirby && IsKirbyWalkMoving() && g_hammerWalkFrames[hammerWalkFrameIndex] != NULL)
    {
        DrawKirbyImage(graphics, g_hammerWalkFrames[hammerWalkFrameIndex]);
    }
    else if (isHammerKirby && g_hammerIdleFrame != NULL)
    {
        DrawKirbyImage(graphics, g_hammerIdleFrame);
    }
    else if (isSparkKirby && IsKirbyWalkMoving() && g_sparkWalkFrames[sparkWalkFrameIndex] != NULL)
    {
        DrawKirbyImage(graphics, g_sparkWalkFrames[sparkWalkFrameIndex]);
    }
    else if (isSparkKirby && g_sparkIdleFrame != NULL)
    {
        DrawKirbyImage(graphics, g_sparkIdleFrame);
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
    if (g_invincibleMode || isHammerInvincibleSkill)
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
    DrawScreenEdgeEffects(graphics, rt.right, rt.bottom);
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
        HandleGameTimer(hWnd, wParam);
        return 0;
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
                g_openingTick = 0;
                g_storyFrameIndex = 0;
                g_storyTick = 0;
                StopMove();
                isSpace = false;
                isSpaceRelease = false;
                balloonTick = 0;
                balloonCooldownTick = 0;
                spaceKeyHeld = false;
                spaceFrameIndex = 0;
                spaceStartFrameDone = false;
                fireBalloonFrameIndex = 0;
                fireBalloonStartFrameDone = false;
                bombBalloonFrameIndex = 0;
                bombBalloonStartFrameDone = false;
                hammerBalloonFrameIndex = 0;
                hammerBalloonStartFrameDone = false;
                sparkBalloonFrameIndex = 0;
                sparkBalloonStartFrameDone = false;
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
                else if (g_pauseMenuIndex == 2)
                {
                    DestroyWindow(hWnd);
                }
                else if (g_pauseMenuIndex == 3)
                {
                    g_isPaused = false;
                    g_controlGuideForced = true;
                    g_controlGuideTick = CONTROL_GUIDE_RESHOW_TICK;
                    PlayGameSound(SFX_PAUSE);
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
        if ((lParam & 0x40000000) != 0)
        {
            switch (wParam)
            {
            case 'W':
            case 'A':
            case 'S':
            case 'D':
            case VK_LEFT:
            case VK_RIGHT:
            case VK_SPACE:
            case VK_SHIFT:
            case 'K':
            case 'I':
            case 'O':
            case 'L':
            case 'U':
                return 0;
            }
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

        if (isAbsorb && wParam != 'K' && !isPowerKirby && !isPowerDigest && !isFireKirby && !isBombKirby && !isHammerKirby && !isSparkKirby)
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
            // SPACE를 계속 누르고 있는 중에 시간 제한으로 풍선이 풀렸다면,
            // 키를 한 번 떼고 쿨타임이 끝나기 전까지 다시 풍선이 켜지지 않게 함
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
                balloonCooldownTick = 0;
                spaceKeyHeld = false;
                spaceFrameIndex = 0;
                spaceStartFrameDone = false;
                fireBalloonFrameIndex = 0;
                fireBalloonStartFrameDone = false;
                bombBalloonFrameIndex = 0;
                bombBalloonStartFrameDone = false;
                hammerBalloonFrameIndex = 0;
                hammerBalloonStartFrameDone = false;
                sparkBalloonFrameIndex = 0;
                sparkBalloonStartFrameDone = false;
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

                if (balloonCooldownTick > 0)
                {
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
                    hammerBalloonFrameIndex = 0;
                    hammerBalloonStartFrameDone = false;
                    sparkBalloonFrameIndex = 0;
                    sparkBalloonStartFrameDone = false;
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
            if (isHammerKirby)
            {
                StartHammerAttack();
            }
            else if (isSparkKirby)
            {
                StartSparkAttack();
            }
            else if (isBombKirby)
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
            else if (isHammerKirby)
            {
                StartHammerInvincibleSkill();
            }
            else if (isSparkKirby)
            {
                StartSparkSpecialAttack();
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

        if (isAbsorb && wParam != 'K' && !isPowerKirby && !isPowerDigest && !isFireKirby && !isBombKirby && !isHammerKirby && !isSparkKirby)
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
                hammerBalloonFrameIndex = 0;
                hammerBalloonStartFrameDone = false;
                sparkBalloonFrameIndex = 0;
                sparkBalloonStartFrameDone = false;
                moveUp = false;
                moveDown = false;
                break;
            }

            StopBalloonWithRelease();
            break;

        case 'K':
            if (!isPowerKirby && !isFireKirby && !isBombKirby && !isHammerKirby && !isSparkKirby)
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
        StopAllGameSounds();
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
