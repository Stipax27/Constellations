#include "AISystem.h"

using namespace std;

#ifndef PI
#define PI 3.14159265358979323846f
#define BOSS_AGRO_RADIUS 40.0f
#define ARENA_GLOBAL_MIN_X -50.0f
#define ARENA_GLOBAL_MAX_X 50.0f
#define ARENA_GLOBAL_MIN_Z 150.0f
#define ARENA_GLOBAL_MAX_Z 250.0f
#endif

AISystem::AISystem() {}

void AISystem::Initialize() {}

void AISystem::Shutdown() {}


void AISystem::Update(EntityStorage& entityStorage, float deltaTime)
{
    const std::vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<AIComponent>();
    for (Entity* entity : entities)
    {
        if (!IsEntityValid(entity) || entity->GetTimeScale() == 0.0f)
            continue;

        Transform* transform = entity->GetComponent<Transform>();
        AIComponent* ai = entity->GetComponent<AIComponent>();
        PhysicBody* physicBody = entity->GetComponent<PhysicBody>();
        BossComponent* boss = entity->GetComponent<BossComponent>();
        Star* star = entity->GetComponent<Star>();

        if (!transform || !ai || !physicBody || !ai->enabled)
            continue;

        // Обновление визуальных эффектов
        if (star && ai->visual.isAttacking)
        {
            if (ai->visual.attackVisualTimer > 0)
                ai->visual.attackVisualTimer -= deltaTime;
            if (ai->visual.specialCastTimer > 0)
                ai->visual.specialCastTimer -= deltaTime;
            if (ai->visual.aoeCastTimer > 0)
                ai->visual.aoeCastTimer -= deltaTime;
            if (ai->visual.summonTimer > 0)
                ai->visual.summonTimer -= deltaTime;
            if (ai->visual.transitionTimer > 0)
                ai->visual.transitionTimer -= deltaTime;

            if (ai->visual.aoeCastTimer > 0)
            {
                float pulse = sin(ai->visual.aoeCastTimer * ai->visual.aoePulseSpeed * 20.0f) * 0.5f + 0.5f;
                float scaleOffset = (ai->visual.attackScale - 1.0f) * pulse;
                star->radius = ai->visual.originalRadius + scaleOffset;
            }

            bool allEffectsFinished = (ai->visual.attackVisualTimer <= 0 &&
                ai->visual.specialCastTimer <= 0 &&
                ai->visual.aoeCastTimer <= 0 &&
                ai->visual.summonTimer <= 0 &&
                ai->visual.transitionTimer <= 0);

            if (allEffectsFinished)
            {
                star->radius = ai->visual.originalRadius;
                star->color1 = ai->visual.originalColor;
                ai->visual.isAttacking = false;
                ai->visual.isCastingSpecial = false;
                ai->visual.isCastingAOE = false;
                ai->visual.isSummoning = false;
                ai->visual.isTransitioning = false;
            }
        }

        // ===== ВАЖНО: босс использует ТОЛЬКО UpdateBossBehavior =====
        if (boss || ai->isBoss)  // Если есть компонент босса или помечен как босс
        {

            UpdateBossBehavior(entityStorage, entity, transform, ai, boss, physicBody, deltaTime);
            continue;  // Пропускаем старую систему!
        }

        // Старая система для обычных врагов
        ProcessAIBehavior(entityStorage, entity, transform, ai, physicBody, deltaTime);
    }
}

void AISystem::ProcessAIBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
    PhysicBody* physicBody, float deltaTime)
{
    Star* star = entity->GetComponent<Star>();
    if (star)
    {
        switch (ai->behaviorType)
        {
        case AIBehaviorType::PATROL: star->color1 = point3d(0.2f, 0.8f, 0.2f); break;
        case AIBehaviorType::CHASE:  star->color1 = point3d(1.0f, 0.5f, 0.0f); break;
        case AIBehaviorType::ATTACK: star->color1 = point3d(1.0f, 0.0f, 0.0f); break;
        case AIBehaviorType::FLEE:   star->color1 = point3d(0.0f, 0.0f, 1.0f); break;
        case AIBehaviorType::IDLE:   star->color1 = point3d(0.5f, 0.5f, 0.5f); break;
        case AIBehaviorType::SEARCH: star->color1 = point3d(1.0f, 1.0f, 0.0f); break;
        case AIBehaviorType::BOSS_PHASE_1: star->color1 = point3d(0.8f, 0.2f, 0.8f); break;
        case AIBehaviorType::BOSS_PHASE_2: star->color1 = point3d(1.0f, 0.2f, 0.5f); break;
        case AIBehaviorType::BOSS_PHASE_3: star->color1 = point3d(1.0f, 0.0f, 0.3f); break;
        }
    }

    switch (ai->behaviorType)
    {
    case AIBehaviorType::PATROL:
        UpdatePatrolBehavior(entityStorage, entity, transform, ai, physicBody, deltaTime);
        break;
    case AIBehaviorType::CHASE:
        UpdateChaseBehavior(entityStorage, entity, transform, ai, physicBody, deltaTime);
        break;
    case AIBehaviorType::ATTACK:
        UpdateAttackBehavior(entityStorage, entity, transform, ai, physicBody, deltaTime);
        break;
    case AIBehaviorType::FLEE:
        UpdateFleeBehavior(entityStorage, entity, transform, ai, physicBody, deltaTime);
        break;
    case AIBehaviorType::IDLE:
        UpdateIdleBehavior(entityStorage, entity, transform, ai, physicBody, deltaTime);
        break;
    case AIBehaviorType::SEARCH:
        UpdateSearchBehavior(entityStorage, entity, transform, ai, physicBody, deltaTime);
        break;
    }

    ai->stateTimer += deltaTime;
}

// ============ МЕТОДЫ БОССА ============

void AISystem::ExecutePendingAttack(EntityStorage& entityStorage, Entity* entity, Transform* transform,
    AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, Star* star)
{
    switch (ai->pendingAttackType)
    {
    case AIComponent::AttackType::Dash:
        BossDashAttack(entityStorage, entity, transform, ai, boss, physicBody, star);
        boss->lastDashTime = 0.0f;
        break;
    case AIComponent::AttackType::StarShot:
        BossStarShot(entityStorage, entity, transform, ai, boss, physicBody, star);
        boss->lastStarShotTime = 0.0f;
        break;
    case AIComponent::AttackType::SideDash:
        BossSideDash(entityStorage, entity, transform, ai, boss, physicBody);
        boss->lastSideDashTime = 0.0f;
        break;
    case AIComponent::AttackType::AOE:
        BossAOEAttack(entityStorage, transform, boss);
        boss->lastSpecialAttackTime = 0.0f;
        break;
    }

    ai->pendingAttackType = AIComponent::AttackType::None;
}

void AISystem::UpdateBossBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform,
    AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, float deltaTime)
{
    // Проверяем, есть ли игрок рядом с боссом
    Entity* player = GetNearestPlayer(entityStorage, entity);
    float distanceToPlayer = GetDistanceToPlayer(entityStorage, entity);

    // Обновляем флаг
    boss->isPlayerInRange = (distanceToPlayer < 60.0f);

    // Активный цвет
    Star* star = entity->GetComponent<Star>();
    if (star)
    {
        star->color1 = point3d(1.0f, 0.2f, 0.2f);  // Красный
    }

    // Обновляем здоровье и фазы
    Health* health = entity->GetComponent<Health>();
    if (health)
    {
        CheckBossPhaseTransition(entityStorage, entity, health, boss, ai);
    }

    // Обновляем таймеры атак (только если не в режиме зарядки)
    if (!ai->isChargingAttack)
    {
        boss->lastSpecialAttackTime += deltaTime;
        boss->lastDashTime += deltaTime;
        boss->lastSideDashTime += deltaTime;
        boss->lastStarShotTime += deltaTime;
    }

    // Ограничение движения в пределах арены
    point3d pos = transform->position;
    if (pos.x < boss->arenaMinX) pos.x = boss->arenaMinX;
    if (pos.x > boss->arenaMaxX) pos.x = boss->arenaMaxX;
    if (pos.z < boss->arenaMinZ) pos.z = boss->arenaMinZ;
    if (pos.z > boss->arenaMaxZ) pos.z = boss->arenaMaxZ;
    transform->position = pos;

    // Логика в зависимости от фазы
    switch (boss->currentPhase)
    {
    case 1:
        UpdateBossPhase1(entityStorage, entity, transform, ai, boss, physicBody, star, deltaTime);
        break;
    case 2:
        UpdateBossPhase2(entityStorage, entity, transform, ai, boss, physicBody, deltaTime);
        break;
    case 3:
        UpdateBossPhase3(entityStorage, entity, transform, ai, boss, physicBody, star, deltaTime);
        break;
    }

    // Обрабатываем зарядку атаки
    if (ai->isChargingAttack)
    {
        ai->chargeTimer -= deltaTime;

        // Визуальный эффект зарядки (пульсация)
       /* if (star && ai->chargeTimer > 0)
        {
            float pulse = 1.0f + sin(ai->chargeTimer * 15.0f) * 0.3f;
            star->radius = ai->visual.originalRadius * pulse;
        }*/

        // Если зарядка завершена - выполняем атаку
        if (ai->chargeTimer <= 0.0f)
        {
            ExecutePendingAttack(entityStorage, entity, transform, ai, boss, physicBody, star);
            ai->isChargingAttack = false;

            // Восстанавливаем размер звезды
            if (star) star->radius = ai->visual.originalRadius;
        }
        else
        {
            // Во время зарядки босс не двигается
            physicBody->acceleration = point3d();
            physicBody->velocity = physicBody->velocity * 0.9f;
            return;  // Пропускаем движение
        }
    }

    ai->stateTimer += deltaTime;
}

void AISystem::UpdateBossPhase1(EntityStorage& entityStorage, Entity* entity, Transform* transform,
    AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, Star* star, float deltaTime)
{
    Entity* player = GetNearestPlayer(entityStorage, entity);
    if (!player) return;

    float distance = GetDistanceToPlayer(entityStorage, entity);

    // Если игрок далеко (>40м) - пассивный режим
    if (distance > 60.0f)
    {
        if (star) star->color1 = point3d(0.5f, 0.2f, 0.5f);
        physicBody->acceleration = point3d();
        physicBody->velocity = physicBody->velocity * 0.95f;
        return;
    }

    // Если босс уже заряжает атаку - ничего не делаем
    if (ai->isChargingAttack) return;

    // ===== АТАКИ С ЗАРЯДКОЙ =====

    // Боковой рывок
    if (boss->lastSideDashTime >= boss->sideDashCooldown && !boss->isSideDashing)
    {
        if (distance <= 15.0f && distance >= 3.0f && (rand() % 100 < 20))
        {

            if (!ai->isChargingAttack) // Проверяем, что еще не в зарядке
            {
                StartChargeAttack(ai, boss, AIComponent::AttackType::SideDash, boss->sideDashChargeDuration);

                // Создаем эффект только если еще не создавали
                if (!ai->isChargeEffectSpawned)
                {
                    SpawnSideEffect(entityStorage, GetGlobalPosition(entity), point3d(0.3f, 0.6f, 1.0f), 1.5f);
                    ai->isChargeEffectSpawned = true;
                    if (star) star->color1 = point3d(0.3f, 0.6f, 1.0f);
                }
                return;
            }
        }
    }
    else if (boss->lastDashTime >= boss->dashCooldown) // Рывок к игроку
    {
        if (distance <= 20.0f && distance > 3.0f && (rand() % 100 < 60))
        {
            if (!ai->isChargingAttack) // Проверяем, что еще не в зарядке
            {
                StartChargeAttack(ai, boss, AIComponent::AttackType::Dash, boss->dashChargeDuration);

                // Создаем эффект только если еще не создавали
                if (!ai->isChargeEffectSpawned)
                {
                    SpawnAttackEffect(entityStorage, GetGlobalPosition(entity), point3d(1.0f, 0.2f, 0.2f), 1.5f);
                    ai->isChargeEffectSpawned = true;
                    if (star) star->color1 = point3d(1.0f, 0.5f, 0.5f);
                }
                return;
            }

        }
    }
    else if (boss->lastStarShotTime >= boss->starShotCooldown) // Выстрел звездами
    {
        if (distance <= 40.0f && (rand() % 100 < 99))
        {

            if (!ai->isChargingAttack) // Проверяем, что еще не в зарядке
            {
                StartChargeAttack(ai, boss, AIComponent::AttackType::StarShot, boss->starShotChargeDuration);

                // Создаем эффект только если еще не создавали
                if (!ai->isChargeEffectSpawned)
                {
                    SpawnAttackEffect(entityStorage, GetGlobalPosition(entity), point3d(0.8f, 0.2f, 1.0f), 2.0f);
                    ai->isChargeEffectSpawned = true;
                    if (star) star->color1 = point3d(0.8f, 0.2f, 1.0f);
                }
                return;
            }

           
        }
    }

    // Ближняя атака (без зарядки)
    if (distance <= ai->attackRange)
    {
        Health* playerHealth = player->GetComponent<Health>();
        if (playerHealth && ai->stateTimer >= ai->attackCooldown)
        {
            playerHealth->hp -= ai->attackDamage;
            ai->stateTimer = 0.0f;
            //SpawnImpactEffect(entityStorage, GetGlobalPosition(player), point3d(1.0f, 0.2f, 0.2f));
        }
    }

    // Движение к игроку (только если не в зарядке)
    point3d bossGlobalPos = GetGlobalPosition(entity);
    point3d playerGlobalPos = GetGlobalPosition(player);

    point3d direction = playerGlobalPos - bossGlobalPos;
    float horizontalDistance = direction.magnitude();

    if (horizontalDistance > 0.5f)
    {
        direction = direction.normalized();
        float targetSpeed = ai->movementSpeed;

        point3d targetVelocity = direction * targetSpeed;
        point3d desiredAccel = (targetVelocity - physicBody->velocity) * ai->accelerationStrength;

        float maxAccel = ai->maxAcceleration;
        if (desiredAccel.magnitude() > maxAccel)
        {
            desiredAccel = desiredAccel.normalized() * maxAccel;
        }

        physicBody->acceleration = desiredAccel;
    }

    // Анти-застревание
    if (physicBody->velocity.magnitude() < 0.5f && distance > 5.0f)
    {
        point3d dir = (playerGlobalPos - bossGlobalPos).normalized();
        physicBody->velocity = dir * 5.0f;
    }
}

// Вспомогательный метод для начала зарядки атаки
void AISystem::StartChargeAttack(AIComponent* ai, BossComponent* boss,
    AIComponent::AttackType attackType, float chargeDuration)
{
    if (ai->isChargingAttack) return;

    ai->isChargingAttack = true;
    ai->chargeTimer = chargeDuration;
    ai->pendingAttackType = attackType;
    ai->isChargeEffectSpawned = false;
    // Останавливаем босса во время зарядки
    if (boss)
    {
        boss->isChargingDash = (attackType == AIComponent::AttackType::Dash);
        boss->isChargingStarShot = (attackType == AIComponent::AttackType::StarShot);
        boss->isChargingSideDash = (attackType == AIComponent::AttackType::SideDash);
        boss->isChargingAOE = (attackType == AIComponent::AttackType::AOE);
    }
}


void AISystem::UpdateBossPhase2(EntityStorage& entityStorage, Entity* entity, Transform* transform,
    AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, float deltaTime)
{
    Entity* player = GetNearestPlayer(entityStorage,entity );
    if (!player) return;

    float distance = GetDistanceToPlayer(entityStorage, entity);
    float currentSpeed = ai->movementSpeed * 1.5f;

    if (transform->position.x < boss->arenaMinX + 5.0f)
        transform->position.x = boss->arenaMinX + 5.0f;
    if (transform->position.x > boss->arenaMaxX - 5.0f)
        transform->position.x = boss->arenaMaxX - 5.0f;
    if (transform->position.z < boss->arenaMinZ + 5.0f)
        transform->position.z = boss->arenaMinZ + 5.0f;
    if (transform->position.z > boss->arenaMaxZ - 5.0f)
        transform->position.z = boss->arenaMaxZ - 5.0f;
    // Атаки в фазе 2
    if (boss->lastDashTime >= boss->dashCooldown * 0.7f)
    {
        if (distance <= 18.0f && distance > 3.0f && (rand() % 100 < 40))
        {
            BossDashAttack(entityStorage, entity, transform, ai, boss, physicBody, entity->GetComponent<Star>());
            boss->lastDashTime = 0.0f;
            return;
        }
    }

    if (boss->lastStarShotTime >= boss->starShotCooldown * 0.7f)
    {
        if (distance <= 30.0f && (rand() % 100 < 50))
        {
            BossStarShot(entityStorage, entity, transform, ai, boss, physicBody, entity->GetComponent<Star>());
            boss->lastStarShotTime = 0.0f;
            return;
        }
    }

    // Движение к игроку
    point3d direction = (GetWorldTransform(player).position - transform->position).normalized();
    point3d targetVelocity = direction * currentSpeed;
    point3d desiredAccel = (targetVelocity - physicBody->velocity) * ai->accelerationStrength;

    if (desiredAccel.magnitude() > ai->maxAcceleration * 1.3f)
        desiredAccel = desiredAccel.normalized() * ai->maxAcceleration * 1.3f;

    physicBody->acceleration = desiredAccel;

    // Ближняя атака
    if (distance <= ai->attackRange)
    {
        Health* playerHealth = player->GetComponent<Health>();
        if (playerHealth && ai->stateTimer >= ai->attackCooldown * 0.6f)
        {
            playerHealth->hp -= ai->attackDamage * 2;
            ai->stateTimer = 0.0f;
            SpawnImpactEffect(entityStorage, GetWorldTransform(player).position, point3d(1.0f, 0.5f, 0.0f));
        }
        physicBody->acceleration = point3d();
    }
}

void AISystem::UpdateBossPhase3(EntityStorage& entityStorage, Entity* entity, Transform* transform,
    AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, Star* star, float deltaTime)
{
    Entity* player = GetNearestPlayer(entityStorage, entity);
    if (!player) return;

    float distance = GetDistanceToPlayer(entityStorage, entity);
    float currentSpeed = ai->movementSpeed * boss->rageSpeedMultiplier;

    if (transform->position.x < boss->arenaMinX + 5.0f)
        transform->position.x = boss->arenaMinX + 5.0f;
    if (transform->position.x > boss->arenaMaxX - 5.0f)
        transform->position.x = boss->arenaMaxX - 5.0f;
    if (transform->position.z < boss->arenaMinZ + 5.0f)
        transform->position.z = boss->arenaMinZ + 5.0f;
    if (transform->position.z > boss->arenaMaxZ - 5.0f)
        transform->position.z = boss->arenaMaxZ - 5.0f;

    // Атаки в фазе 3
    if (boss->lastDashTime >= boss->dashCooldown * 0.5f)
    {
        if (distance <= 20.0f && (rand() % 100 < 50))
        {
            BossDashAttack(entityStorage, entity, transform, ai, boss, physicBody, star);
            boss->lastDashTime = 0.0f;
            return;
        }
    }

    if (boss->lastStarShotTime >= boss->starShotCooldown * 0.5f)
    {
        if (distance <= 35.0f && (rand() % 100 < 60))
        {
            BossStarShot(entityStorage, entity, transform, ai, boss, physicBody, star);
            boss->lastStarShotTime = 0.0f;
            return;
        }
    }

    // АОЕ атака
    if (boss->lastSpecialAttackTime >= boss->specialAttackCooldown * 0.5f)
    {
        if (distance <= boss->aoeAttackRange && (rand() % 100 < 30))
        {
            BossAOEAttack(entityStorage, transform, boss);
            boss->lastSpecialAttackTime = 0.0f;
            return;
        }
    }

    // Движение к игроку
    point3d direction = (GetWorldTransform(player).position - transform->position).normalized();
    point3d targetVelocity = direction * currentSpeed;
    point3d desiredAccel = (targetVelocity - physicBody->velocity) * ai->accelerationStrength;

    if (desiredAccel.magnitude() > ai->maxAcceleration * 1.5f)
        desiredAccel = desiredAccel.normalized() * ai->maxAcceleration * 1.5f;

    physicBody->acceleration = desiredAccel;

    // Ближняя атака
    if (distance <= ai->attackRange)
    {
        Health* playerHealth = player->GetComponent<Health>();
        if (playerHealth && ai->stateTimer >= ai->attackCooldown * 0.4f)
        {
            playerHealth->hp -= ai->attackDamage * 3;
            ai->stateTimer = 0.0f;
            SpawnImpactEffect(entityStorage, GetWorldTransform(player).position, point3d(1.0f, 0.2f, 0.0f));
        }
        physicBody->acceleration = point3d();
    }

    // Аура урона
    if (distance < 5.0f)
    {
        Health* playerHealth = player->GetComponent<Health>();
        if (playerHealth)
        {
            playerHealth->hp -= 30.0f * deltaTime;
        }
    }
}

// ============ АТАКИ БОССА ============

void AISystem::BossSideDash(EntityStorage& entityStorage, Entity* entity, Transform* transform,
    AIComponent* ai, BossComponent* boss, PhysicBody* physicBody)
{
    Entity* player = GetNearestPlayer(entityStorage, entity);
    if (!player) return;

    point3d bossGlobalPos = GetGlobalPosition(entity);
    point3d playerGlobalPos = GetGlobalPosition(player);

    point3d toBoss = bossGlobalPos - playerGlobalPos;
    toBoss = toBoss.normalized();

    point3d sideDirection = point3d(-toBoss.z, 0, toBoss.x);

    if (rand() % 2 == 0)
    {
        sideDirection = point3d(toBoss.z, 0, -toBoss.x);
    }

    //SpawnAttackEffect(entityStorage, bossGlobalPos, point3d(1.0f, 0.2f, 0.2f), 1.2f);
    //SpawnSlashEffect(entityStorage, bossGlobalPos, sideDirection, point3d(0.3f, 0.6f, 1.0f));

    physicBody->velocity = (sideDirection*5.f)*boss->sideDashSpeed;
    physicBody->acceleration = point3d();

    boss->isSideDashing = true;
    boss->sideDashTimer = 0.5f;
    boss->sideDashDirection = sideDirection;

    // ... остальной код эффектов ...
}

void AISystem::BossDashAttack(EntityStorage& entityStorage, Entity* entity, Transform* transform,
    AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, Star* star)
{
    Entity* player = GetNearestPlayer(entityStorage, entity);
    if (!player) return;

    // Получаем глобальные позиции
    point3d bossGlobalPos = GetGlobalPosition(entity);
    point3d playerGlobalPos = GetGlobalPosition(player);

    // Визуальный эффект на глобальной позиции
    //SpawnAttackEffect(entityStorage, bossGlobalPos, point3d(1.0f, 0.2f, 0.2f), 1.5f);
    //SpawnAuraEffect(entity, point3d(1.0f, 0.2f, 0.2f), 0.3f);

    if (star && !ai->visual.isAttacking && ai->visual.attackVisualTimer <= 0)
    {
        ai->visual.originalRadius = star->radius;
        ai->visual.originalColor = star->color1;
    }

    ai->visual.isAttacking = true;
    ai->visual.attackVisualTimer = ai->visual.attackDuration;

    if (star)
    {
        star->radius = ai->visual.originalRadius * ai->visual.attackScale;
    }

    // Направление рывка (глобальное)
    point3d dashDirection = (playerGlobalPos - bossGlobalPos).normalized();
    physicBody->velocity = dashDirection * boss->dashSpeed;
    physicBody->acceleration = point3d();

    // Эффект следа
    Entity* trail = entityStorage.CreateEntity("DashTrail", entity);
    ParticleEmitter* trailParticles = trail->AddComponent<ParticleEmitter>();
    trailParticles->rate = 300;
    trailParticles->lifetime = 200;
    trailParticles->color = point3d(1.0f, 0.2f, 0.5f);
    trailParticles->size = { 0.1f, 0.4f };
    trailParticles->opacity = { 0.8f, 0.0f };
    trailParticles->emitDirection = EmitDirection::Back;
    trailParticles->spread = { 0.5f, 0.5f };
    trailParticles->speed = { 5.0f, 2.0f };
    trailParticles->useWorldSpace = false;

    DelayedDestroy* delayed = trail->AddComponent<DelayedDestroy>();
    delayed->lifeTime = 300;

    // Урон игроку
    Health* playerHealth = player->GetComponent<Health>();
    if (playerHealth)
    {
        playerHealth->hp -= boss->dashDamage;
        SpawnImpactEffect(entityStorage, playerGlobalPos, point3d(1.0f, 0.2f, 0.2f));
    }
}

void AISystem::BossStarShot(EntityStorage& entityStorage, Entity* entity, Transform* transform,
    AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, Star* star)
{
    Entity* player = GetNearestPlayer(entityStorage, entity);
    if (!player) return;

    point3d bossGlobalPos = GetGlobalPosition(entity);
    point3d playerGlobalPos = GetGlobalPosition(player);

    // Эффект на ГЛОБАЛЬНОЙ позиции
    //SpawnAttackEffect(entityStorage, bossGlobalPos, point3d(0.8f, 0.2f, 1.0f), 4.2f);

    for (int i = 0; i < boss->starShotCount; i++)
    {
        Entity* starEntity = entityStorage.CreateEntity("BossStar", nullptr);  // nullptr, не entity!

        Transform* starTransform = starEntity->AddComponent<Transform>();
        starTransform->position = bossGlobalPos;  // Глобальная позиция!

        // Направление к игроку
        point3d direction = (playerGlobalPos - bossGlobalPos).normalized();
        direction.x += ((rand() % 100) - 50) / 100.0f;
        direction.y += ((rand() % 100) - 50) / 100.0f;
        direction.z += ((rand() % 100) - 50) / 100.0f;
        direction = direction.normalized();

        PhysicBody* starPhysic = starEntity->AddComponent<PhysicBody>();
        starPhysic->velocity = direction * (boss->starShotSpeed + (rand() % 50) / 10.0f);
        starPhysic->airFriction = 0.0f;

        Star* starComp = starEntity->AddComponent<Star>();
        starComp->radius = 0.5f;
        starComp->crownRadius = 0.8f;
        starComp->color1 = point3d(1.0f, 0.3f, 0.8f);
        starComp->color2 = point3d(0.8f, 0.1f, 0.5f);
        starComp->crownColor = point3d(1.0f, 0.5f, 1.0f);

        SphereCollider* collider = starEntity->AddComponent<SphereCollider>();
        collider->radius = 0.6f;
        collider->collisionGroup = CollisionFilter::Group::Enemy;

        SingleDamager* damager = starEntity->AddComponent<SingleDamager>();
        damager->damage = boss->starShotDamage;
        damager->target = Fraction::Player;
        damager->destroyable = true;

        DelayedDestroy* destroy = starEntity->AddComponent<DelayedDestroy>();
        destroy->lifeTime = 3000;
    }
}

void AISystem::BossSpecialAttack(EntityStorage& entityStorage, Entity* entity, Transform* transform,
    AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, Star* star)
{
    BossDashAttack(entityStorage, entity, transform, ai, boss, physicBody, star);
}

void AISystem::BossAOEAttack(EntityStorage& entityStorage, Transform* transform, BossComponent* boss)
{
    if (!transform) return;

    SpawnAOEEffect(entityStorage, transform->position, boss->aoeAttackRange, point3d(1.0f, 0.5f, 0.0f));

    Entity* chargeEffect = entityStorage.CreateEntity("AOECharge", nullptr);
    Transform* chargeTransform = chargeEffect->AddComponent<Transform>();
    chargeTransform->position = transform->position;

    Star* chargeStar = chargeEffect->AddComponent<Star>();
    chargeStar->radius = 1.0f;
    chargeStar->crownRadius = boss->aoeAttackRange;
    chargeStar->color1 = point3d(1.0f, 0.3f, 0.0f);
    chargeStar->color2 = point3d(0.8f, 0.2f, 0.0f);

    DelayedDestroy* delayed = chargeEffect->AddComponent<DelayedDestroy>();
    delayed->lifeTime = 500;

    // Поиск игроков через entityStorage напрямую
    const std::vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<Health>();

    for (Entity* target : entities)
    {
        Transform* targetTransform = target->GetComponent<Transform>();
        Health* health = target->GetComponent<Health>();

        if (health && health->fraction == Fraction::Player && targetTransform)
        {
            float distance = (targetTransform->position - transform->position).magnitude();

            if (distance <= boss->aoeAttackRange)
            {
                SpawnImpactEffect(entityStorage, targetTransform->position, point3d(1.0f, 0.5f, 0.0f));
                float damageMultiplier = 1.0f - (distance / boss->aoeAttackRange);
                float damage = boss->aoeDamage * damageMultiplier;
                health->hp -= damage;
            }
        }
    }
}

void AISystem::CheckBossPhaseTransition(EntityStorage& entityStorage, Entity* entity, Health* health,
    BossComponent* boss, AIComponent* ai)
{
    if (!health || boss->isTransitioning) return;

    float healthPercentage = health->hp / health->maxHp;

    if (healthPercentage <= 0.7f && boss->currentPhase == 1)
    {
        boss->currentPhase = 2;
        boss->rageSpeedMultiplier = 1.5f;
        boss->isTransitioning = true;

        Transform* transform = entity->GetComponent<Transform>();
        if (transform)
        {
            //SpawnAuraEffect(entity, point3d(1.0f, 0.2f, 0.5f), 2.0f);
            //SpawnAttackEffect(entityStorage, transform->position, point3d(1.0f, 0.3f, 0.8f), 2.5f);
        }

        boss->isTransitioning = false;
    }
    else if (healthPercentage <= 0.3f && boss->currentPhase == 2)
    {
        boss->currentPhase = 3;
        boss->rageSpeedMultiplier = 2.0f;
        boss->isTransitioning = true;

        Transform* transform = entity->GetComponent<Transform>();
        if (transform)
        {
            SpawnAuraEffect(entity, point3d(1.0f, 0.0f, 0.3f), 3.0f);
            SpawnAOEEffect(entityStorage, transform->position, 15.0f, point3d(1.0f, 0.2f, 0.3f));
        }

        boss->isTransitioning = false;
    }
}

// ============ ВИЗУАЛЬНЫЕ ЭФФЕКТЫ ============

void AISystem::SpawnAttackEffect(EntityStorage& entityStorage, const point3d& position, const point3d& color, float size)
{
    Entity* effect = entityStorage.CreateEntity("BossAttackEffect", nullptr);

    Transform* transform = effect->AddComponent<Transform>();
    transform->position = position;  // Это уже глобальная позиция

    /*Star* star = effect->AddComponent<Star>();
    star->radius = size;
    star->crownRadius = size * 1.5f;
    star->color1 = color;
    star->color2 = color * 0.5f;
    star->crownColor = color * 1.5f;*/

    ParticleEmitter* particles = effect->AddComponent<ParticleEmitter>();

    particles->rate = 350;
    particles->lifetime = 2000;
    particles->color = color;
    particles->size = { 0.0f, 5.0f };
    particles->opacity = { 1.0f, 0.0f };
    particles->emitDirection = EmitDirection::Up;
    particles->speed = { 10.0f, 0.0f };
    particles->spread = { PI, PI };
    particles->isReverse = true;
    particles->useWorldSpace = true;
    

    DelayedDestroy* delayed = effect->AddComponent<DelayedDestroy>();
    delayed->lifeTime = 2000;
}

void AISystem::SpawnSideEffect(EntityStorage& entityStorage, const point3d& position, const point3d& color, float size)
{
    Entity* effect = entityStorage.CreateEntity("BossAttackEffect", nullptr);

    Transform* transform = effect->AddComponent<Transform>();
    transform->position = position;  // Это уже глобальная позиция

    /*Star* star = effect->AddComponent<Star>();
    star->radius = size;
    star->crownRadius = size * 1.5f;
    star->color1 = color;
    star->color2 = color * 0.5f;
    star->crownColor = color * 1.5f;*/

    ParticleEmitter* particles = effect->AddComponent<ParticleEmitter>();

    particles->rate = 50;
    particles->lifetime = 1500;
    particles->color = color;
    particles->size = { 10.0f, 10.0f };
    particles->opacity = { 1.0f, 0.0f };
    particles->emitDirection = EmitDirection::Up;
    particles->speed = { 10.0f, 0.0f };
    particles->spread = { PI, PI };
    particles->isReverse = true;
    particles->useWorldSpace = true;


    DelayedDestroy* delayed = effect->AddComponent<DelayedDestroy>();
    delayed->lifeTime = 1500;
}

void AISystem::SpawnSlashEffect(EntityStorage& entityStorage, const point3d& position, const point3d& direction, const point3d& color)
{
    Entity* slash = entityStorage.CreateEntity("BossSlashEffect", nullptr);

    Transform* transform = slash->AddComponent<Transform>();
    transform->position = position;

    point3d lookDir = direction;
    if (lookDir.magnitude() < 0.01f) lookDir = point3d(1, 0, 0);
    lookDir = lookDir.normalized();

    float yaw = atan2(lookDir.x, lookDir.z);
    float pitch = asin(lookDir.y);
    transform->mRotation = XMMatrixRotationRollPitchYaw(pitch, yaw, 0);

    Beam* beam = slash->AddComponent<Beam>();
    beam->point1 = point3d(0, 0, -1.5f);
    beam->point2 = point3d(0, 0, 1.5f);
    beam->size1 = 0.5f;
    beam->size2 = 0.5f;
    beam->color1 = color;
    beam->color2 = color;
    beam->opacity1 = 1.0f;
    beam->opacity2 = 0.0f;
    beam->pShader = 22;

    ParticleEmitter* particles = slash->AddComponent<ParticleEmitter>();
    particles->rate = 300;
    particles->lifetime = 200;
    particles->color = color;
    particles->size = { 0.1f, 0.5f };
    particles->opacity = { 1.0f, 0.0f };
    particles->emitDirection = EmitDirection::Right;
    particles->spread = { 1.57f, 1.57f };
    particles->speed = { 15.0f, 8.0f };
    particles->useWorldSpace = false;

    DelayedDestroy* delayed = slash->AddComponent<DelayedDestroy>();
    delayed->lifeTime = 300;
}

void AISystem::SpawnImpactEffect(EntityStorage& entityStorage, const point3d& position, const point3d& color)
{
    Entity* impact = entityStorage.CreateEntity("BossImpactEffect", nullptr);

    Transform* transform = impact->AddComponent<Transform>();
    transform->position = position;

    ParticleEmitter* particles = impact->AddComponent<ParticleEmitter>();
    particles->rate = 400;
    particles->lifetime = 400;
    particles->color = color;
    particles->size = { 0.1f, 0.8f };
    particles->opacity = { 1.0f, 0.0f };
    particles->emitDirection = EmitDirection::Up;
    particles->spread = { 3.14f, 3.14f };
    particles->speed = { 15.0f, 10.0f };
    particles->useWorldSpace = true;

    Star* star = impact->AddComponent<Star>();
    star->radius = 0.3f;
    star->crownRadius = 0.6f;
    star->color1 = color;
    star->color2 = color * 0.5f;
    star->crownColor = point3d(1, 1, 1);

    DelayedDestroy* delayed = impact->AddComponent<DelayedDestroy>();
    delayed->lifeTime = 400;
}

void AISystem::SpawnAuraEffect(Entity* bossEntity, const point3d& color, float duration)
{
    if (!bossEntity) return;

    EntityStorage* storage = bossEntity->GetOwnerStorage();
    if (!storage) return;

    Entity* aura = storage->CreateEntity("BossAura", bossEntity);

    Transform* transform = aura->AddComponent<Transform>();
    transform->position = point3d(0, 0, 0);

    Star* star = aura->AddComponent<Star>();
    star->radius = 5.0f;
    star->crownRadius = 7.0f;
    star->color1 = color;
    star->color2 = color * 0.3f;
    star->crownColor = color * 1.2f;

    ParticleEmitter* particles = aura->AddComponent<ParticleEmitter>();
    particles->rate = 50;
    particles->lifetime = 1000;
    particles->color = color;
    particles->size = { 0.1f, 0.3f };
    particles->opacity = { 0.8f, 0.0f };
    particles->emitDirection = EmitDirection::Up;
    particles->spread = { 3.14f, 3.14f };
    particles->speed = { 3.0f, 1.0f };
    particles->useWorldSpace = true;

    DelayedDestroy* delayed = aura->AddComponent<DelayedDestroy>();
    delayed->lifeTime = duration * 1000;
}

void AISystem::SpawnAOEEffect(EntityStorage& entityStorage, const point3d& position, float radius, const point3d& color)
{
    Entity* ring = entityStorage.CreateEntity("AOERing", nullptr);

    Transform* transform = ring->AddComponent<Transform>();
    transform->position = position;

    Star* star = ring->AddComponent<Star>();
    star->radius = 0.5f;
    star->crownRadius = radius;
    star->color1 = color;
    star->color2 = color * 0.3f;
    star->crownColor = color * 1.5f;

    Entity* explosion = entityStorage.CreateEntity("AOEExplosion", nullptr);
    transform = explosion->AddComponent<Transform>();
    transform->position = position;

    ParticleEmitter* particles = explosion->AddComponent<ParticleEmitter>();
    particles->rate = 500;
    particles->lifetime = 500;
    particles->color = color;
    particles->size = { 0.2f, 1.0f };
    particles->opacity = { 1.0f, 0.0f };
    particles->emitDirection = EmitDirection::Up;
    particles->spread = { 3.14f, 3.14f };
    particles->speed = { 20.0f, 10.0f };
    particles->useWorldSpace = true;

    DelayedDestroy* delayed = ring->AddComponent<DelayedDestroy>();
    delayed->lifeTime = 300;

    delayed = explosion->AddComponent<DelayedDestroy>();
    delayed->lifeTime = 500;
}

// ============ ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ============

Entity* AISystem::GetNearestPlayer(EntityStorage& entityStorage, Entity* bossEntity)
{
    if (!bossEntity) return nullptr;

    const std::vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<Health>();
    Entity* nearestPlayer = nullptr;
    float minDistance = FLT_MAX;

    point3d bossGlobalPos = GetGlobalPosition(bossEntity);

    for (Entity* target : entities)
    {
        if (!IsEntityValid(target)) continue;

        Health* health = target->GetComponent<Health>();
        if (health && health->fraction == Fraction::Player)
        {
            point3d playerGlobalPos = GetGlobalPosition(target);

            float distance = (playerGlobalPos - bossGlobalPos).magnitude();
            if (distance < minDistance)
            {
                minDistance = distance;
                nearestPlayer = target;
            }
        }
    }

    if (nearestPlayer)
    {
        AIComponent* ai = nearestPlayer->GetComponent<AIComponent>();
        if (ai) ai->targetId = nearestPlayer->GetId();
    }

    return nearestPlayer;
}

float AISystem::GetDistanceToPlayer(EntityStorage& entityStorage, Entity* bossEntity)
{
    if (!bossEntity) return FLT_MAX;

    Entity* player = GetNearestPlayer(entityStorage, bossEntity);
    if (!player) return FLT_MAX;

    point3d bossGlobalPos = GetGlobalPosition(bossEntity);
    point3d playerGlobalPos = GetGlobalPosition(player);

    return (playerGlobalPos - bossGlobalPos).magnitude();
}

// ============ ОСТАЛЬНЫЕ МЕТОДЫ (ПАТРУЛЬ, ПРЕСЛЕДОВАНИЕ И Т.Д.) ============

void AISystem::UpdatePatrolBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
    PhysicBody* physicBody, float deltaTime)
{
    if (ai->patrolPoints.empty()) return;

    point3d& targetPoint = ai->patrolPoints[ai->currentPatrolIndex];
    point3d direction = targetPoint - transform->position;
    float distance = direction.magnitude();

    if (distance < ai->arrivalDistance)
    {
        ai->currentPatrolIndex = (ai->currentPatrolIndex + 1) % ai->patrolPoints.size();
        ai->stateTimer = 0.0f;
    }
    else
    {
        direction = direction.normalized();
        point3d targetVelocity = direction * ai->movementSpeed;
        point3d desiredAccel = (targetVelocity - physicBody->velocity) * ai->accelerationStrength;

        float accelMag = desiredAccel.magnitude();
        if (accelMag > ai->maxAcceleration)
            desiredAccel = desiredAccel.normalized() * ai->maxAcceleration;
        physicBody->acceleration = desiredAccel;
    }

    Entity* target = DetectTarget(entityStorage, entity, transform, ai);
    if (target)
    {
        ai->lastKnownPlayerPosition = GetWorldTransform(target).position;
        ai->hasLastKnownPosition = true;
        ai->behaviorType = AIBehaviorType::CHASE;
        ai->stateTimer = 0.0f;
    }
}

void AISystem::UpdateChaseBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
    PhysicBody* physicBody, float deltaTime)
{
    Entity* target = entityStorage.GetEntityById(ai->targetId);
    if (!IsEntityValid(target))
    {
        ai->behaviorType = AIBehaviorType::PATROL;
        return;
    }

    point3d targetWorldPos = GetWorldTransform(target).position;
    point3d direction = targetWorldPos - GetWorldTransform(entity).position;
    float distance = direction.magnitude();

    if (distance > ai->chaseRange)
    {
        ai->lastKnownPlayerPosition = targetWorldPos;
        ai->hasLastKnownPosition = true;
        ai->behaviorType = AIBehaviorType::SEARCH;
        ai->stateTimer = 0.0f;
    }
    else if (distance <= ai->attackRange)
    {
        ai->behaviorType = AIBehaviorType::ATTACK;
        ai->stateTimer = 0.0f;
    }
    else
    {
        direction = direction.normalized();
        point3d targetVelocity = direction * ai->movementSpeed;
        point3d desiredAccel = targetVelocity * ai->accelerationStrength;
        float accelMag = desiredAccel.magnitude();
        if (accelMag > ai->maxAcceleration)
            desiredAccel = desiredAccel.normalized() * ai->maxAcceleration;
        physicBody->acceleration = desiredAccel;
    }
}

void AISystem::UpdateSearchBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
    PhysicBody* physicBody, float deltaTime)
{
    if (!ai->hasLastKnownPosition || ai->stateTimer >= ai->searchDuration)
    {
        GoToPatrolAroundLastPosition(ai);
        return;
    }

    point3d direction = ai->lastKnownPlayerPosition - transform->position;
    float distance = direction.magnitude();

    if (distance < ai->arrivalDistance)
    {
        GoToPatrolAroundLastPosition(ai);
        return;
    }

    direction = direction.normalized();
    point3d targetVelocity = direction * ai->movementSpeed;
    point3d desiredAccel = targetVelocity * ai->accelerationStrength;
    float accelMag = desiredAccel.magnitude();
    if (accelMag > ai->maxAcceleration)
        desiredAccel = desiredAccel.normalized() * ai->maxAcceleration;
    physicBody->acceleration = desiredAccel;

    Entity* target = DetectTarget(entityStorage, entity, transform, ai);
    if (target)
    {
        ai->lastKnownPlayerPosition = GetWorldTransform(target).position;
        ai->behaviorType = AIBehaviorType::CHASE;
        ai->stateTimer = 0.0f;
    }
}

void AISystem::UpdateAttackBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
    PhysicBody* physicBody, float deltaTime)
{
    Entity* target = entityStorage.GetEntityById(ai->targetId);
    if (!IsEntityValid(target))
    {
        ai->behaviorType = AIBehaviorType::PATROL;
        return;
    }

    float distance = (GetWorldTransform(target).position - GetWorldTransform(entity).position).magnitude();

    if (distance > ai->attackRange)
    {
        ai->behaviorType = AIBehaviorType::CHASE;
        ai->stateTimer = 0.0f;
    }
    else
    {
        physicBody->velocity = point3d();
        Health* playerHealth = target->GetComponent<Health>();
        if (playerHealth)
        {
            playerHealth->hp -= ai->attackDamage;
            if (playerHealth->hp < 0) playerHealth->hp = 0;
        }
        ai->stateTimer = 0.0f;
    }
}

void AISystem::UpdateFleeBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
    PhysicBody* physicBody, float deltaTime)
{
    Entity* target = entityStorage.GetEntityById(ai->targetId);
    if (!IsEntityValid(target))
    {
        ai->behaviorType = AIBehaviorType::PATROL;
        return;
    }

    Transform* targetTransform = target->GetComponent<Transform>();
    if (!targetTransform) return;

    point3d direction = transform->position - targetTransform->position;
    direction = direction.normalized();
    point3d targetVelocity = direction * ai->movementSpeed;
    point3d desiredAccel = targetVelocity * ai->accelerationStrength;
    float accelMag = desiredAccel.magnitude();
    if (accelMag > ai->maxAcceleration)
        desiredAccel = desiredAccel.normalized() * ai->maxAcceleration;
    physicBody->acceleration = desiredAccel;

    if (ai->stateTimer >= ai->fleeDuration)
    {
        ai->behaviorType = AIBehaviorType::PATROL;
        ai->stateTimer = 0.0f;
    }
}

void AISystem::UpdateIdleBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
    PhysicBody* physicBody, float deltaTime)
{
    physicBody->velocity = point3d();

    if (DetectTarget(entityStorage, entity, transform, ai))
    {
        ai->behaviorType = AIBehaviorType::CHASE;
        ai->stateTimer = 0.0f;
    }

    if (ai->stateTimer >= ai->idleDuration)
    {
        ai->behaviorType = AIBehaviorType::PATROL;
        ai->stateTimer = 0.0f;
    }
}

Entity* AISystem::DetectTarget(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai)
{
    const std::vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<Health>();

    for (Entity* target : entities)
    {
        if (target == entity || !IsEntityValid(target))
            continue;

        Transform* targetTransform = target->GetComponent<Transform>();
        if (!targetTransform)
            continue;

        Health* health = target->GetComponent<Health>();
        if (!health || health->fraction != Fraction::Player)
            continue;

        if ((GetWorldTransform(target).position - GetWorldTransform(entity).position).magnitude() <= ai->detectionRange) {
            ai->targetId = target->GetId();
            return target;
        }
    }

    return nullptr;
}

void AISystem::GoToPatrolAroundLastPosition(AIComponent* ai)
{
    if (ai->hasLastKnownPosition)
    {
        point3d center = ai->lastKnownPlayerPosition;
        float r = ai->searchPatrolRadius;
        ai->patrolPoints = {
            center + point3d(r, 0,  0),
            center + point3d(0, 0,  r),
            center + point3d(-r, 0,  0),
            center + point3d(0, 0, -r)
        };
        ai->currentPatrolIndex = 0;
    }
    ai->behaviorType = AIBehaviorType::PATROL;
    ai->stateTimer = 0.0f;
    ai->hasLastKnownPosition = false;
}

void AISystem::UpdateBossCircleMovement(EntityStorage& entityStorage, Entity* entity, Transform* transform,
    AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, float deltaTime)
{
    // Этот метод пока не используется, оставлен для совместимости
}
point3d AISystem::GetGlobalPosition(Entity* entity)
{
    if (!entity) return point3d(0, 0, 0);

    point3d globalPos = point3d(0, 0, 0);
    Entity* current = entity;

    while (current)
    {
        Transform* t = current->GetComponent<Transform>();
        if (t)
        {
            globalPos = globalPos + t->position;
        }
        current = current->GetParent();
    }

    return globalPos;
}