
#ifndef _AI_COMPONENT_
#define _AI_COMPONENT_

//////////////
// INCLUDES //
//////////////
#include <vector>
#include "component.h"
#include "Point3d.h"
#include <DirectXMath.h>
#include "SplinePath.cpp"
#include "entity.h"
#include "Transform.cpp"
#include "Constellation.cpp"
#include "timer.h"

enum class AIBehaviorType
{
    IDLE,
    PATROL,
    CHASE,
    ATTACK,
    FLEE
};

struct AIComponent : Component
{
public:
    AIBehaviorType behaviorType = AIBehaviorType::PATROL;
    //PhysicBody* enemyPhysicBody;
    // Параметры движения
    float movementSpeed = 1.0f;
    float arrivalDistance = 1.0f;

    // Параметры обнаружения
    float detectionRange = 10.0f;
    float chaseRange = 15.0f;
    float attackRange = 3.0f;
    float timeAgr = 1.f;

    // Параметры атаки
    float attackCooldown = 2.0f;
    float attackDamage = 10.0f;

    // Параметры состояний
    float idleDuration = 3.0f;
    float fleeDuration = 5.0f;

    // Патрулирование
    std::vector<point3d> patrolPoints;
    int currentPatrolIndex = 0;

    // Таймеры
    float stateTimer = 0.0f;

};

#endif