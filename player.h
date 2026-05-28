#pragma once

#include <windows.h>
#include <gdiplus.h>

bool IsMoving();
bool IsKirbyWalkMoving();
void StopMove();
void StartJump();
void UpdateKirbyPosition(HWND hWnd);
bool IsInsideKirby(int mouseX, int mouseY);
void StartKirbyHitEffect();
RECT GetKirbyBodyRect();
void SetKirbyNormalSizeKeepBottom();
void SetKirbyPowerSizeKeepBottom();
void DigestPowerKirby();
void StartPowerProjectile();
void UpdatePowerWait();
void UpdatePowerAttack();
void UpdatePowerDigest();
void EjectAbilityStar();
void UpdateAbilityStar();
void DrawAbilityStar(Gdiplus::Graphics& graphics);
void StartFireKirbyTransform();
void SpawnFireBall();
void StartFireBreath();
void UpdateFireKirbyStates();
void CheckFireAttacksHitMonsters();
void UpdateEnemyFireBalls();
void CheckEnemyFireBallsHitKirby();
void StartBombKirbyTransform();
void StartBombAttack();
void StartBombSpecialAttack();
void UpdateBombAttack();
void StartHammerKirbyTransform();
void StartHammerAttack();
void UpdateHammerAttack();
RECT GetHammerAttackRect();
bool IsHammerAttackDamageFrame();
void CheckHammerAttackHitMonsters();
void UpdateBombObjects();
void DrawBombObjects(Gdiplus::Graphics& graphics);
void DrawBombExplosions(Gdiplus::Graphics& graphics);
void DrawImageFlipX(Gdiplus::Graphics& graphics, Gdiplus::Image* image, int x, int y, int w, int h);
void DrawKirbyImage(Gdiplus::Graphics& graphics, Gdiplus::Image* image);
void DrawPowerProjectile(Gdiplus::Graphics& graphics);
void DrawFireBreath(Gdiplus::Graphics& graphics);
void DrawFireBall(Gdiplus::Graphics& graphics);
void DrawEnemyFireBalls(Gdiplus::Graphics& graphics);
void UpdateKirbyHitEffect();
void UpdateKirbyStatusEffects();
void UpdateHPBarAnimation();
