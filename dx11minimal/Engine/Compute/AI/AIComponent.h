#ifndef _AI_COMPONENT_H_
#define _AI_COMPONENT_H_

#include "../../ECS_Base/component.h"
#include <vector>
#include <string>

enum class AIBehaviorType {
    IDLE,
    PATROL,
    CHASE,
    ATTACK,
    FLEE,
    SEARCH,
    BOSS_PHASE_1,
    BOSS_PHASE_2,
    BOSS_PHASE_3
};

struct AIComponent : public Component {
    bool enabled = true;

    // Основные параметры
    float detectionRange = 15.0f;
    float attackRange = 2.5f;
    float chaseRange = 30.0f;
    float movementSpeed = 8.0f;
    float accelerationStrength = 8.0f;
    float maxAcceleration = 50.0f;
    float attackDamage = 15.0f;
    float attackCooldown = 1.5f;
    float arrivalDistance = 1.0f;

    // Состояния
    AIBehaviorType behaviorType = AIBehaviorType::IDLE;
    float stateTimer = 0.0f;
    int targetId = -1;

    // Патруль
    std::vector<point3d> patrolPoints;
    int currentPatrolIndex = 0;

    // Поиск
    bool hasLastKnownPosition = false;
    point3d lastKnownPlayerPosition;
    float searchDuration = 5.0f;
    float searchPatrolRadius = 5.0f;
    float idleDuration = 3.0f;
    float fleeDuration = 3.0f;

    // Визуальные эффекты атаки
    struct AttackVisual {
        bool isAttacking = false;
        float attackVisualTimer = 0.0f;
        float attackDuration = 0.3f;
        float attackScale = 1.3f;
        float originalRadius = 1.0f;
        point3d originalColor = point3d(1.0f, 1.0f, 1.0f);

        bool isCastingSpecial = false;
        float specialCastTimer = 0.0f;

        bool isCastingAOE = false;
        float aoeCastTimer = 0.0f;
        float aoePulseSpeed = 3.0f;

        bool isSummoning = false;
        float summonTimer = 0.0f;

        bool isTransitioning = false;
        float transitionTimer = 0.0f;
    } visual;

    // Для босса
    bool isBoss = false;
    bool isChargingAttack = false;
    bool isChargeEffectSpawned = false;
    float chargeTimer = 0.0f;

    enum class AttackType {
        None,
        Dash,
        StarShot,
        SideDash,
        AOE
    };

    AttackType pendingAttackType = AttackType::None;

    // ===== ПАРАМЕТРЫ ДЛЯ МИНЬОНОВ (обычных врагов) =====
    bool isMinion = false;              // Флаг миньона
    float minionAggroRadius = 20.0f;    // Радиус привлечения внимания
    float minionDeaggroRadius = 40.0f;  // Радиус потери интереса
    bool isAggroed = false;             // Находится ли в боевом режиме

    // Таймеры для фаз атаки миньона
    float minionWindupTimer = 0.0f;     // Таймер замаха
    float minionWindupDuration = 0.4f;  // Длительность замаха (сек)
    float minionLungeTimer = 0.0f;      // Таймер рывка
    float minionLungeDuration = 0.3f;   // Длительность рывка (сек)
    float minionRecoveryTimer = 0.0f;   // Таймер восстановления
    float minionRecoveryDuration = 1.0f;// Пауза после атаки (сек)

    // Параметры рывка миньона
    float minionLungeSpeed = 20.0f;     // Скорость рывка при атаке
    float minionPushForce = 10.0f;      // Сила отталкивания игрока
    point3d minionLungeDirection;       // Направление рывка
    bool minionHasDealtDamage = false;  // Нанесен ли урон в текущем рывке

    // Фазы атаки миньона (используются внутри ATTACK)
    enum class MinionAttackPhase {
        WINDUP,     // Замах
        LUNGE,      // Рывок
        RECOVERY    // Восстановление
    };
    MinionAttackPhase minionAttackPhase = MinionAttackPhase::WINDUP;

    AIComponent() = default;
};

#endif