#ifndef _BOSS_COMPONENT_H_
#define _BOSS_COMPONENT_H_

#include "../../ECS_Base/component.h"

struct BossComponent : Component
{

    int currentPhase = 1;
    float phaseHealthThresholds[3] = { 0.7f, 0.3f, 0.0f };

    float specialAttackCooldown = 10.0f;
    float lastSpecialAttackTime = 0.0f;
    int summonCount = 3;
    float rageSpeedMultiplier = 1.5f;

    float aoeAttackRange = 8.0f;
    float aoeDamage =10.0f;
    float dashAttackSpeed = 15.0f;

    float phaseTransitionTime = 2.0f;
    bool isTransitioning = false;
};

#endif
