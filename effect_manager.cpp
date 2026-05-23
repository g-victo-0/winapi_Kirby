#include "effect_manager.h"
#include <stdlib.h>

using namespace Gdiplus;

const int STAGE_EFFECT_MAX = 64;
const int STAGE_EFFECT_WORLD_W = 2000;

struct StageEffectParticle
{
    bool active;
    int stage;
    int type;
    float x;
    float y;
    float vx;
    float vy;
    int tick;
    int life;
    int size;
};

StageEffectParticle g_stageEffectParticles[STAGE_EFFECT_MAX];
int g_stageEffectTick = 0;

int EffectRandomRange(int minValue, int maxValue)
{
    if (maxValue <= minValue)
        return minValue;

    return minValue + rand() % (maxValue - minValue + 1);
}

void ResetStageAtmosphereEffects()
{
    for (int i = 0; i < STAGE_EFFECT_MAX; i++)
    {
        g_stageEffectParticles[i].active = false;
        g_stageEffectParticles[i].stage = 0;
        g_stageEffectParticles[i].type = 0;
        g_stageEffectParticles[i].x = 0.0f;
        g_stageEffectParticles[i].y = 0.0f;
        g_stageEffectParticles[i].vx = 0.0f;
        g_stageEffectParticles[i].vy = 0.0f;
        g_stageEffectParticles[i].tick = 0;
        g_stageEffectParticles[i].life = 0;
        g_stageEffectParticles[i].size = 0;
    }

    g_stageEffectTick = 0;
}

void SpawnStageParticle(int stage, int type, float x, float y, float vx, float vy, int life, int size)
{
    for (int i = 0; i < STAGE_EFFECT_MAX; i++)
    {
        if (!g_stageEffectParticles[i].active)
        {
            g_stageEffectParticles[i].active = true;
            g_stageEffectParticles[i].stage = stage;
            g_stageEffectParticles[i].type = type;
            g_stageEffectParticles[i].x = x;
            g_stageEffectParticles[i].y = y;
            g_stageEffectParticles[i].vx = vx;
            g_stageEffectParticles[i].vy = vy;
            g_stageEffectParticles[i].tick = 0;
            g_stageEffectParticles[i].life = life;
            g_stageEffectParticles[i].size = size;
            return;
        }
    }
}

void UpdateStageAtmosphereEffects(int currentStage)
{
    g_stageEffectTick++;

    for (int i = 0; i < STAGE_EFFECT_MAX; i++)
    {
        if (!g_stageEffectParticles[i].active)
            continue;

        g_stageEffectParticles[i].tick++;
        g_stageEffectParticles[i].x += g_stageEffectParticles[i].vx;
        g_stageEffectParticles[i].y += g_stageEffectParticles[i].vy;

        if (g_stageEffectParticles[i].tick >= g_stageEffectParticles[i].life)
            g_stageEffectParticles[i].active = false;
    }

    if (currentStage >= 1 && currentStage <= 3)
    {
        if (g_stageEffectTick % 3 == 0)
        {
            SpawnStageParticle(
                currentStage,
                EffectRandomRange(0, 2),
                (float)EffectRandomRange(0, STAGE_EFFECT_WORLD_W),
                (float)EffectRandomRange(-30, 150),
                (float)EffectRandomRange(-5, 5) / 10.0f,
                (float)EffectRandomRange(2, 6) / 10.0f,
                EffectRandomRange(45, 75),
                EffectRandomRange(4, 9)
            );
        }
    }
}

void DrawTinyStar(Graphics& graphics, int x, int y, int size, int alpha)
{
    int inner = size / 2;
    if (inner < 2)
        inner = 2;

    Point points[8] =
    {
        Point(x, y - size),
        Point(x + inner, y - inner),
        Point(x + size, y),
        Point(x + inner, y + inner),
        Point(x, y + size),
        Point(x - inner, y + inner),
        Point(x - size, y),
        Point(x - inner, y - inner)
    };

    SolidBrush brush(Color(alpha, 255, 238, 100));
    graphics.FillPolygon(&brush, points, 8);
}

void DrawEnergyShard(Graphics& graphics, int x, int y, int size, int alpha, bool red)
{
    Point points[4] =
    {
        Point(x, y - size),
        Point(x + size / 2, y),
        Point(x, y + size),
        Point(x - size / 2, y)
    };

    Color color = red ? Color(alpha, 210, 80, 255) : Color(alpha, 150, 80, 255);
    SolidBrush brush(color);
    graphics.FillPolygon(&brush, points, 4);
}

void DrawStageAtmosphereEffects(Graphics& graphics, int currentStage, int cameraX, int screenW, int screenH)
{
    if (currentStage < 1 || currentStage > 3)
        return;

    for (int i = 0; i < STAGE_EFFECT_MAX; i++)
    {
        StageEffectParticle* p = &g_stageEffectParticles[i];

        if (!p->active || p->stage != currentStage)
            continue;

        int sx = (int)p->x - cameraX;
        int sy = (int)p->y;

        if (sx < -30 || sx > screenW + 30 || sy < -30 || sy > screenH + 30)
            continue;

        int alpha = 255 - p->tick * 255 / p->life;
        if (alpha < 0)
            alpha = 0;

        if (p->type == 0)
        {
            DrawEnergyShard(graphics, sx, sy, p->size, alpha * 155 / 255, false);
        }
        else if (p->type == 1)
        {
            SolidBrush purpleBrush(Color(alpha * 90 / 255, 120, 65, 210));
            graphics.FillEllipse(&purpleBrush, sx, sy, p->size + 5, p->size);
        }
        else
        {
            DrawEnergyShard(graphics, sx, sy, p->size, alpha * 135 / 255, true);
        }
    }
}
