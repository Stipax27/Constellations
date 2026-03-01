
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
#include "Transform.h"
#include "Constellation.h"
#include "timer.h"

enum class AIBehaviorType
{
    IDLE,
    PATROL,
    CHASE,
    ATTACK,
    FLEE,
    SEARCH
};

struct AIComponent : Component
{
public:
    bool enabled = true;
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
    float attackCooldown = 1.0f;
    float attackDamage = 10.0f;

    // Параметры состояний
    float idleDuration = 3.0f;
    float fleeDuration = 5.0f;

    // Патрулирование
    std::vector<point3d> patrolPoints;
    int currentPatrolIndex = 0;

    // Таймеры
    float stateTimer = 0.0f;

    // Поиск
    point3d lastKnownPlayerPosition;   // последняя позиция, где видели игрока
    bool hasLastKnownPosition = false; // флаг, есть ли сохранённая позиция
    float searchDuration = 1.0f;       // сколько времени искать перед возвратом к патрулю
    float searchPatrolRadius = 5.0f;   // радиус для точек патруля вокруг последней позиции

    // Ускорение
    float accelerationStrength = 0.25f;   // коэффициент пропорциональности (чем выше, тем быстрее разгон)
    float maxAcceleration = 10.0f;       // ограничение, чтобы избежать рывков

};

#endif