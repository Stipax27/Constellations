#ifndef _BOSS_COMPONENT_H_
#define _BOSS_COMPONENT_H_

#include "../../ECS_Base/component.h"

struct BossComponent : Component
{
    int currentPhase = 1;
    float phaseHealthThresholds[3] = { 0.7f, 0.3f, 0.0f };

    // Таймеры атак (инициализируем в CreateZenithLocation)
    float specialAttackCooldown = 6.0f;
    float lastSpecialAttackTime = 0.0f;

    float dashCooldown = 2.0f;
    float lastDashTime = 0.0f;

    float starShotCooldown = 1.5f;
    float lastStarShotTime = 0.0f;

    float sideDashCooldown = 2.5f;
    float lastSideDashTime = 0.0f;
    float sideDashSpeed = 10.f;
    // Параметры атак
    float dashSpeed = 30.0f;
    float dashDamage = 25.0f;

    float starShotCount = 12;
    float starShotSpeed = 15.0f;
    float starShotDamage = 10.0f;

    float aoeAttackRange = 12.0f;
    float aoeDamage = 35.0f;

    // Арена 100x100x100
    float arenaMinX = -50.0f;
    float arenaMaxX = 50.0f;
    float arenaMinY = -50.0f;
    float arenaMaxY = 50.0f;
    float arenaMinZ = -50.0f;
    float arenaMaxZ = 50.0f;

    // Состояния
    float rageSpeedMultiplier = 1.0f;
    bool isTransitioning = false;

    bool isSideDashing = false;
    float sideDashTimer = 0.0f;
    point3d sideDashDirection;

    bool isPlayerInRange = false;

    float executionTimer = 5.0f;
    float executionTimeLimit = 5.0f;
    bool isInExecutionPhase = false;
};

#endif