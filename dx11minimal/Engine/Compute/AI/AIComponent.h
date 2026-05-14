#ifndef _AI_COMPONENT_
#define _AI_COMPONENT_

//////////////
// INCLUDES //
//////////////
#include "../../ECS_Base/component.h"
#include "../../Types/Point3d.h"
#include <DirectXMath.h>
#include <vector>

enum class AIBehaviorType
{
    IDLE,
    PATROL,
    CHASE,
    ATTACK,
    FLEE,
    SEARCH,

    BOSS_PHASE_1,
    BOSS_PHASE_2,
    BOSS_PHASE_3,
    BOSS_SUMMON,
    BOSS_SPECIAL_ATTACK,
    BOSS_RAGE
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

    int targetId = -1;
    bool isBoss = false;

    enum class AttackType {
        None,
        Dash,
        StarShot,
        SideDash,   
        AOE
    };

    bool isChargingAttack = false;
    float chargeTimer = 0.0f;
    float chargeDuration = 1.5f;  // Время зарядки перед атакой
    AttackType pendingAttackType = AttackType::None;
    bool isChargeEffectSpawned = false;
    

    // ============ ВИЗУАЛЬНЫЕ ЭФФЕКТЫ ДЛЯ БОССА ============
    struct VisualFeedback
    {
        // Флаги состояния
        bool isAttacking = false;           // идет ли анимация атаки
        bool isCastingSpecial = false;      // зарядка спецатаки
        bool isCastingAOE = false;          // зарядка АОЕ атаки
        bool isSummoning = false;            // призыв миньонов
        bool isTransitioning = false;        // переход между фазами

        // Таймеры визуальных эффектов
        float attackVisualTimer = 0.0f;      // сколько осталось анимации атаки
        float specialCastTimer = 0.0f;       // таймер зарядки спецатаки
        float aoeCastTimer = 0.0f;           // таймер зарядки АОЕ
        float summonTimer = 0.0f;            // таймер призыва
        float transitionTimer = 0.0f;        // таймер перехода фазы

        // Длительности эффектов
        float attackDuration = 0.3f;         // длительность анимации атаки
        float specialCastDuration = 0.5f;    // время зарядки спецатаки
        float aoeCastDuration = 1.0f;        // время зарядки АОЕ (игрок может убежать)
        float summonDuration = 0.8f;         // длительность призыва
        float transitionDuration = 1.5f;     // длительность перехода фазы

        // Оригинальные значения Star компонента (для восстановления)
        float originalRadius = 1.0f;
        point3d originalColor = point3d(1.0f, 1.0f, 1.0f);
        float originalIntensity = 1.0f;

        // Эффекты для разных типов атак
        float attackScale = 2.5f;             // увеличение размера при атаке
        float specialGlow = 2.0f;             // интенсивность свечения при спецатаке
        float aoePulseSpeed = 3.0f;           // скорость пульсации при АОЕ

        // Цвета для разных типов атак
        point3d meleeAttackColor = point3d(1.0f, 0.2f, 0.2f);     // красный
        point3d specialAttackColor = point3d(1.0f, 0.5f, 0.0f);   // оранжевый
        point3d aoeAttackColor = point3d(1.0f, 0.0f, 1.0f);       // фиолетовый
        point3d summonColor = point3d(0.2f, 1.0f, 0.2f);          // зеленый
        point3d phaseTransitionColor = point3d(1.0f, 1.0f, 0.0f);  // желтый

    } visual;
};

#endif