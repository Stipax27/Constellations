#include "AISystem.h"

using namespace std;


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
            // Обновляем таймеры
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

            // Пульсация для АОЕ
            if (ai->visual.aoeCastTimer > 0)
            {
                float pulse = sin(ai->visual.aoeCastTimer * ai->visual.aoePulseSpeed * 20.0f) * 0.5f + 0.5f;
                float scaleOffset = (ai->visual.attackScale - 1.0f) * pulse;
                star->radius = ai->visual.originalRadius + scaleOffset;
            }

            // Восстановление после эффектов
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

        // Выбор поведения
        if (boss)
        {
            UpdateBossBehavior(entityStorage, entity, transform, ai, boss, physicBody, deltaTime);
        }
        else
        {
            ProcessAIBehavior(entityStorage, entity, transform, ai, physicBody, deltaTime);
        }
    }
}

// Метод, который в зависимости от текущего состояния ИИ вызывает соответствующую функцию
void AISystem::ProcessAIBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
    PhysicBody* physicBody, float deltaTime)
{
    // Для отладки меняем цвет звезды в зависимости от состояния (необязательно)
    Star* star = entity->GetComponent<Star>();
    if (star)
    {
        switch (ai->behaviorType)
        {
        case AIBehaviorType::PATROL: star->color1 = point3d(0.2f, 0.8f, 0.2f); break; // зелёный
        case AIBehaviorType::CHASE:  star->color1 = point3d(1.0f, 0.5f, 0.0f); break; // оранжевый
        case AIBehaviorType::ATTACK: star->color1 = point3d(1.0f, 0.0f, 0.0f); break; // красный
        case AIBehaviorType::FLEE:   star->color1 = point3d(0.0f, 0.0f, 1.0f); break; // синий
        case AIBehaviorType::IDLE:   star->color1 = point3d(0.5f, 0.5f, 0.5f); break; // серый
        case AIBehaviorType::SEARCH: star->color1 = point3d(1.0f, 1.0f, 0.0f); break; // жёлтый

        // Цвета для Босса
        case AIBehaviorType::BOSS_PHASE_1: star->color1 = point3d(0.8f, 0.2f, 0.8f); break; // фиолетовый
        case AIBehaviorType::BOSS_PHASE_2: star->color1 = point3d(1.0f, 0.2f, 0.5f); break; // розовый
        case AIBehaviorType::BOSS_PHASE_3: star->color1 = point3d(1.0f, 0.0f, 0.3f); break; // ярко-красный
        }


    }

    // Выбор подходящего обработчика в зависимости от текущего состояния
    switch (ai->behaviorType)          // Проверяем текущее состояние (тип поведения)
    {
    case AIBehaviorType::PATROL:       // Если патрулирование
        UpdatePatrolBehavior(entityStorage, entity, transform, ai, physicBody, deltaTime); // Вызываем метод патрулирования
        break;

    case AIBehaviorType::CHASE:        // Если преследование
        UpdateChaseBehavior(entityStorage, entity, transform, ai, physicBody, deltaTime); // Вызываем метод преследования
        break;

    case AIBehaviorType::ATTACK:       // Если атака
        UpdateAttackBehavior(entityStorage, entity, transform, ai, physicBody, deltaTime); // Вызываем метод атаки
        break;

    case AIBehaviorType::FLEE:         // Если бегство
        UpdateFleeBehavior(entityStorage, entity, transform, ai, physicBody, deltaTime); // Вызываем метод бегства
        break;

    case AIBehaviorType::IDLE:         // Если бездействие
        UpdateIdleBehavior(entityStorage, entity, transform, ai, physicBody, deltaTime); // Вызываем метод бездействия
        break;

    case AIBehaviorType::SEARCH:       // Если поиск (после потери игрока)
        UpdateSearchBehavior(entityStorage, entity, transform, ai, physicBody, deltaTime); // Вызываем метод поиска
        break;

    case AIBehaviorType::BOSS_PHASE_1:
    case AIBehaviorType::BOSS_PHASE_2:
    case AIBehaviorType::BOSS_PHASE_3:
    case AIBehaviorType::BOSS_SUMMON:
    case AIBehaviorType::BOSS_SPECIAL_ATTACK:
    case AIBehaviorType::BOSS_RAGE:
       
        break;
    }

    ai->stateTimer += deltaTime;   // Увеличиваем таймер текущего состояния
}


// Эффект удара (вспышка с частицами)
void AISystem::SpawnAttackEffect(EntityStorage& entityStorage, const point3d& position, const point3d& color, float size)
{
    Entity* effect = entityStorage.CreateEntity("BossAttackEffect", nullptr);

    Transform* transform = effect->AddComponent<Transform>();
    transform->position = position;

    Star* star = effect->AddComponent<Star>();
    star->radius = size;
    star->crownRadius = size * 1.5f;
    star->color1 = color;
    star->color2 = color * 0.5f;
    star->crownColor = color * 1.5f;

    // Добавляем партиклы для эффекта
    ParticleEmitter* particles = effect->AddComponent<ParticleEmitter>();
    particles->rate = 200;
    particles->lifetime = 300;
    particles->color = color;
    particles->size = { 0.2f, 1.0f };
    particles->opacity = { 1.0f, 0.0f };
    particles->emitDirection = EmitDirection::Up;
    particles->spread = { 3.14f, 3.14f }; // PI, PI
    particles->speed = { 10.0f, 5.0f };
    particles->useWorldSpace = true;

    // Автоматическое уничтожение через 0.5 секунды
    DelayedDestroy* delayed = effect->AddComponent<DelayedDestroy>();
    delayed->lifeTime = 500;
}

// Эффект рубящего удара (как мечом)
void AISystem::SpawnSlashEffect(EntityStorage& entityStorage, const point3d& position, const point3d& direction, const point3d& color)
{
    Entity* slash = entityStorage.CreateEntity("BossSlashEffect", nullptr);

    Transform* transform = slash->AddComponent<Transform>();
    transform->position = position;

    // Направляем взгляд в сторону удара
    point3d lookDir = direction;
    if (lookDir.magnitude() < 0.01f) lookDir = point3d(1, 0, 0);
    lookDir = lookDir.normalized();

    // Вычисляем углы для матрицы поворота
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

    // Партиклы вдоль разреза
    ParticleEmitter* particles = slash->AddComponent<ParticleEmitter>();
    particles->rate = 300;
    particles->lifetime = 200;
    particles->color = color;
    particles->size = { 0.1f, 0.5f };
    particles->opacity = { 1.0f, 0.0f };
    particles->emitDirection = EmitDirection::Right;
    particles->spread = { 1.57f, 1.57f }; // PI/2, PI/2
    particles->speed = { 15.0f, 8.0f };
    particles->useWorldSpace = false;

    DelayedDestroy* delayed = slash->AddComponent<DelayedDestroy>();
    delayed->lifeTime = 300;
}

// Эффект ударной волны
void AISystem::SpawnImpactEffect(EntityStorage& entityStorage, const point3d& position, const point3d& color)
{
    Entity* impact = entityStorage.CreateEntity("BossImpactEffect", nullptr);

    Transform* transform = impact->AddComponent<Transform>();
    transform->position = position;

    // Кольцевая волна частиц
    ParticleEmitter* particles = impact->AddComponent<ParticleEmitter>();
    particles->rate = 400;
    particles->lifetime = 400;
    particles->color = color;
    particles->size = { 0.1f, 0.8f };
    particles->opacity = { 1.0f, 0.0f };
    particles->emitDirection = EmitDirection::Up;
    particles->spread = { 3.14f, 3.14f }; // PI, PI - все направления
    particles->speed = { 15.0f, 10.0f };
    particles->useWorldSpace = true;

    // Звезда в центре удара
    Star* star = impact->AddComponent<Star>();
    star->radius = 0.3f;
    star->crownRadius = 0.6f;
    star->color1 = color;
    star->color2 = color * 0.5f;
    star->crownColor = point3d(1, 1, 1);

    DelayedDestroy* delayed = impact->AddComponent<DelayedDestroy>();
    delayed->lifeTime = 400;
}

// Aura эффект для фаз босса
void AISystem::SpawnAuraEffect(Entity* bossEntity, const point3d& color, float duration)
{
    if (!bossEntity) return;

    Entity* aura = bossEntity->GetOwnerStorage()->CreateEntity("BossAura", bossEntity);

    Transform* transform = aura->AddComponent<Transform>();
    transform->position = point3d(0, 0, 0);

    // Пульсирующая звезда вокруг босса
    Star* star = aura->AddComponent<Star>();
    star->radius = 5.0f;
    star->crownRadius = 7.0f;
    star->color1 = color;
    star->color2 = color * 0.3f;
    star->crownColor = color * 1.2f;
   

    // Постоянные партиклы вокруг босса
    ParticleEmitter* particles = aura->AddComponent<ParticleEmitter>();
    particles->rate = 50;
    particles->lifetime = 1000;
    particles->color = color;
    particles->size = { 0.1f, 0.3f };
    particles->opacity = { 0.8f, 0.0f };
    particles->emitDirection = EmitDirection::Up;
    particles->spread = { 3.14f, 3.14f }; // PI, PI - все направления
    particles->speed = { 3.0f, 1.0f };
    particles->useWorldSpace = true;

    DelayedDestroy* delayed = aura->AddComponent<DelayedDestroy>();
    delayed->lifeTime = duration * 1000;
}

// Эффект ближней атаки
void AISystem::SpawnMeleeAttackEffect(EntityStorage& entityStorage, const point3d& position, const point3d& direction)
{
    SpawnSlashEffect(entityStorage, position, direction, point3d(1.0f, 0.2f, 0.2f));
    SpawnAttackEffect(entityStorage, position + direction * 2.0f, point3d(1.0f, 0.5f, 0.0f), 0.8f);
}

// Эффект дальней атаки
void AISystem::SpawnRangedAttackEffect(EntityStorage& entityStorage, const point3d& position, const point3d& direction)
{
    // Эффект заряда перед выстрелом
    SpawnAttackEffect(entityStorage, position, point3d(0.8f, 0.2f, 1.0f), 0.5f);

    // Линия полета снаряда
    Entity* beam = entityStorage.CreateEntity("AttackBeam", nullptr);
    Transform* transform = beam->AddComponent<Transform>();
    transform->position = position;

    Beam* beamComp = beam->AddComponent<Beam>();
    beamComp->point1 = point3d(0, 0, 0);
    beamComp->point2 = direction * 10.0f;
    beamComp->size1 = 0.2f;
    beamComp->size2 = 0.2f;
    beamComp->color1 = point3d(0.8f, 0.2f, 1.0f);
    beamComp->color2 = point3d(0.8f, 0.2f, 1.0f);
    beamComp->opacity1 = 1.0f;
    beamComp->opacity2 = 0.0f;

    DelayedDestroy* delayed = beam->AddComponent<DelayedDestroy>();
    delayed->lifeTime = 200;
}

// Эффект АОЕ атаки
void AISystem::SpawnAOEEffect(EntityStorage& entityStorage, const point3d& position, float radius, const point3d& color)
{
    // Расширяющееся кольцо
    Entity* ring = entityStorage.CreateEntity("AOERing", nullptr);

    Transform* transform = ring->AddComponent<Transform>();
    transform->position = position;

    Star* star = ring->AddComponent<Star>();
    star->radius = 0.5f;
    star->crownRadius = radius;
    star->color1 = color;
    star->color2 = color * 0.3f;
    star->crownColor = color * 1.5f;
    

    // Взрывные частицы
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
    particles->spread = { 3.14f, 3.14f }; // PI, PI - все направления
    particles->speed = { 20.0f, 10.0f };
    particles->useWorldSpace = true;

    DelayedDestroy* delayed = ring->AddComponent<DelayedDestroy>();
    delayed->lifeTime = 300;

    delayed = explosion->AddComponent<DelayedDestroy>();
    delayed->lifeTime = 500;
}


void AISystem::UpdateBossBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform,
    AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, float deltaTime)
{
    // Проверяем смену фазы
    Health* health = entity->GetComponent<Health>();
    if (health)
    {
        CheckBossPhaseTransition(entityStorage, entity, health, boss, ai);
    }

    // Анимируем цвет звезды для визуальной обратной связи
    Star* star = entity->GetComponent<Star>();
    if (star)
    {
        switch (boss->currentPhase)
        {
        case 1: star->color1 = point3d(0.8f, 0.2f, 0.8f); break;
        case 2: star->color1 = point3d(1.0f, 0.2f, 0.5f); break;
        case 3: star->color1 = point3d(1.0f, 0.0f, 0.3f); break;
        }
    }

    // Обновляем таймер спецатаки
    boss->lastSpecialAttackTime += deltaTime;

    // Логика в зависимости от фазы
    switch (boss->currentPhase)
    {
    case 1:
        UpdateBossPhase1(entityStorage, entity, transform, ai, boss, physicBody,star, deltaTime);
        break;
    case 2:
        UpdateBossPhase2(entityStorage, entity, transform, ai, boss, physicBody, deltaTime);
        break;
    case 3:
        UpdateBossPhase3(entityStorage, entity, transform, ai, boss, physicBody, star ,deltaTime);
        break;
    }

    ai->stateTimer += deltaTime;
}


void AISystem::UpdateBossPhase1(EntityStorage& entityStorage, Entity* entity, Transform* transform,
    AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, Star* star, float deltaTime)
{
    // ПОИСК ИГРОКА
    Entity* player = GetNearestPlayer(entityStorage, transform);
    if (!player)
    {
        // Нет игрока - стоим на месте
        physicBody->acceleration = point3d();
        physicBody->velocity = physicBody->velocity * 0.95f;
        return;
    }

    float distance = GetDistanceToPlayer(entityStorage, transform);

    // Специальная атака
    if (boss->lastSpecialAttackTime >= boss->specialAttackCooldown)
    {
        if (rand() % 100 < 20)
        {
            BossSpecialAttack(entityStorage, entity, transform, ai, boss, physicBody, star);
            boss->lastSpecialAttackTime = 0.0f;  // ИСПРАВЛЕНО: только это поле есть
            return;
        }
    }

    // Если в радиусе атаки
    if (distance <= ai->attackRange)
    {
        // Останавливаемся
        physicBody->acceleration = point3d();
        physicBody->velocity = physicBody->velocity * 0.9f;

        // Атакуем
        Health* playerHealth = player->GetComponent<Health>();
        if (playerHealth && ai->stateTimer >= ai->attackCooldown)
        {
            playerHealth->hp -= ai->attackDamage;
            ai->stateTimer = 0.0f;

            // Отбрасываем игрока
            PhysicBody* playerPhysic = player->GetComponent<PhysicBody>();
            if (playerPhysic)
            {
                point3d pushDirection = (player->GetComponent<Transform>()->position - transform->position).normalized();
                playerPhysic->velocity = pushDirection * 8.0f;
            }
        }
    }
    else
    {
        // ДВИГАЕМСЯ К ИГРОКУ
        point3d playerPos = GetWorldTransform(player).position;
        point3d bossPos = transform->position;
        point3d direction = (playerPos - bossPos).normalized();

        // Желаемая скорость
        point3d targetVelocity = direction * ai->movementSpeed;

        // Ускорение для достижения желаемой скорости
        point3d desiredAccel = (targetVelocity - physicBody->velocity) * ai->accelerationStrength;

        // Ограничиваем ускорение
        if (desiredAccel.magnitude() > ai->maxAcceleration)
        {
            desiredAccel = desiredAccel.normalized() * ai->maxAcceleration;
        }

        physicBody->acceleration = desiredAccel;

        // Ограничиваем максимальную скорость
        float maxSpeed = 15.0f;
        if (physicBody->velocity.magnitude() > maxSpeed)
        {
            physicBody->velocity = physicBody->velocity.normalized() * maxSpeed;
        }
    }

    // Обновляем таймеры
    ai->stateTimer += deltaTime;
    boss->lastSpecialAttackTime += deltaTime;  // ИСПРАВЛЕНО: увеличиваем таймер спецатаки
}

void AISystem::UpdateBossPhase2(EntityStorage& entityStorage, Entity* entity, Transform* transform,
    AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, float deltaTime)
{
    Entity* player = GetNearestPlayer(entityStorage, transform);
    if (!player) return;

    float distance = GetDistanceToPlayer(entityStorage, transform);

    // Призыв миньонов в фазе 2 (чаще, чем спецатаки)
    if (boss->lastSpecialAttackTime >= boss->specialAttackCooldown * 0.6f)
    {
        //BossSummonMinions(entityStorage, transform, boss);
        boss->lastSpecialAttackTime = 0.0f;
        return;
    }

    // Увеличенная скорость в фазе 2
    float currentSpeed = ai->movementSpeed * 1.3f;

    if (distance <= ai->attackRange)
    {
        // Удвоенный урон в фазе 2 и более быстрая атака
        Health* playerHealth = player->GetComponent<Health>();
        if (playerHealth && ai->stateTimer >= ai->attackCooldown * 0.7f)
        {
            playerHealth->hp -= ai->attackDamage * 2;
            ai->stateTimer = 0.0f;
        }
        physicBody->acceleration = point3d();
    }
    else
    {
        point3d direction = (GetWorldTransform(player).position - transform->position).normalized();
        point3d targetVelocity = direction * currentSpeed;
        point3d desiredAccel = targetVelocity * ai->accelerationStrength;

        if (desiredAccel.magnitude() > ai->maxAcceleration * 1.2f)
            desiredAccel = desiredAccel.normalized() * ai->maxAcceleration * 1.2f;

        physicBody->acceleration = desiredAccel;
    }
}

void AISystem::UpdateBossPhase3(EntityStorage& entityStorage, Entity* entity, Transform* transform,
    AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, Star* star,float deltaTime)
{
    Entity* player = GetNearestPlayer(entityStorage, transform);
    if (!player) return;

    float distance = GetDistanceToPlayer(entityStorage, transform);

    // Частые спецатаки в фазе 3
    if (boss->lastSpecialAttackTime >= boss->specialAttackCooldown * 0.4f)
    {
        // Чередуем AOE и обычные спецатаки
        if (rand() % 100 < 50)
        {
            BossAOEAttack(entityStorage, transform, boss);
        }
        else
        {
            BossSpecialAttack(entityStorage, entity, transform,ai, boss, physicBody,star);
        }
        boss->lastSpecialAttackTime = 0.0f;
        return;
    }

    // Ещё большая скорость в фазе 3
    float currentSpeed = ai->movementSpeed * boss->rageSpeedMultiplier;

    if (distance <= ai->attackRange)
    {
        // Тройной урон и очень быстрые атаки
        Health* playerHealth = player->GetComponent<Health>();
        if (playerHealth && ai->stateTimer >= ai->attackCooldown * 0.4f)
        {
            playerHealth->hp -= ai->attackDamage * 3;
            ai->stateTimer = 0.0f;
        }
        physicBody->acceleration = point3d();
    }
    else
    {
        point3d direction = (GetWorldTransform(player).position - transform->position).normalized();
        point3d targetVelocity = direction * currentSpeed;
        point3d desiredAccel = targetVelocity * ai->accelerationStrength;

        if (desiredAccel.magnitude() > ai->maxAcceleration * 1.5f)
            desiredAccel = desiredAccel.normalized() * ai->maxAcceleration * 1.5f;

        physicBody->acceleration = desiredAccel;
    }
}

// ============ СПЕЦИАЛЬНЫЕ АТАКИ БОССА ============

void AISystem::BossSpecialAttack(EntityStorage& entityStorage, Entity* entity, Transform* transform,
    AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, Star* star)
{
    Entity* player = GetNearestPlayer(entityStorage, transform);
    if (!player) return;

    // ВИЗУАЛЬНЫЙ ЭФФЕКТ ПОДГОТОВКИ АТАКИ
    SpawnAuraEffect(entity, point3d(0.8f, 0.2f, 0.8f), 0.5f);
    SpawnAttackEffect(entityStorage, transform->position, point3d(0.8f, 0.2f, 0.8f), 1.5f);

    if (!ai->visual.isAttacking && ai->visual.attackVisualTimer <= 0)
    {
        ai->visual.originalRadius = star->radius;
        ai->visual.originalColor = star->color1;
    }

    ai->visual.isAttacking = true;
    ai->visual.isCastingSpecial = true;
    ai->visual.attackVisualTimer = ai->visual.attackDuration;
    ai->visual.specialCastTimer = ai->visual.specialCastDuration;

    star->radius = ai->visual.originalRadius * ai->visual.attackScale;
    star->color1 = ai->visual.specialAttackColor;

    Transform* playerTransform = player->GetComponent<Transform>();
    if (playerTransform)
    {
        point3d dashDirection = (playerTransform->position - transform->position).normalized();

        // ЭФФЕКТ ПЕРЕД РЫВКОМ
        SpawnSlashEffect(entityStorage, transform->position, dashDirection, point3d(1.0f, 0.0f, 0.5f));

        physicBody->velocity = dashDirection * boss->dashAttackSpeed;
        physicBody->acceleration = point3d();

        // ЭФФЕКТ СЛЕДА ВО ВРЕМЯ РЫВКА
        Entity* trail = entityStorage.CreateEntity("DashTrail", entity);
        ParticleEmitter* trailParticles = trail->AddComponent<ParticleEmitter>();
        trailParticles->rate = 300;
        trailParticles->lifetime = 200;
        trailParticles->color = point3d(1.0f, 0.3f, 0.8f);
        trailParticles->size = { 0.1f, 0.4f };
        trailParticles->opacity = { 0.8f, 0.0f };
        trailParticles->emitDirection = EmitDirection::Back;
        trailParticles->spread = { 0.5f, 0.5f };
        trailParticles->speed = { 5.0f, 2.0f };
        trailParticles->useWorldSpace = false;

        DelayedDestroy* delayed = trail->AddComponent<DelayedDestroy>();
        delayed->lifeTime = 300;

        Health* playerHealth = player->GetComponent<Health>();
        if (playerHealth)
        {
            playerHealth->hp -= ai->attackDamage * 1.5f;

            // ЭФФЕКТ ПОПАДАНИЯ В ИГРОКА
            SpawnImpactEffect(entityStorage, playerTransform->position, point3d(1.0f, 0.2f, 0.2f));

            PhysicBody* playerPhysic = player->GetComponent<PhysicBody>();
            if (playerPhysic)
            {
                point3d pushDirection = dashDirection * 8.0f;
                playerPhysic->velocity = pushDirection;
            }
        }
    }
}

void AISystem::BossAOEAttack(EntityStorage& entityStorage, Transform* transform, BossComponent* boss)
{
    if (!transform) return;

    // ВИЗУАЛЬНЫЙ ЭФФЕКТ ПОДГОТОВКИ АОЕ
    SpawnAOEEffect(entityStorage, transform->position, boss->aoeAttackRange, point3d(1.0f, 0.5f, 0.0f));

    // Эффект заряда
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

    // Поиск игроков в радиусе АОЕ
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
                // ЭФФЕКТ ПОПАДАНИЯ ПО КАЖДОМУ ИГРОКУ
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

    if (healthPercentage <= 0.3f && boss->currentPhase < 3)
    {
        boss->currentPhase = 3;
        boss->isTransitioning = true;

        // ВИЗУАЛЬНЫЙ ЭФФЕКТ ПЕРЕХОДА В ФАЗУ 3
        Transform* transform = entity->GetComponent<Transform>();
        if (transform)
        {
            // Большой взрыв при переходе
            SpawnAOEEffect(entityStorage, transform->position, 15.0f, point3d(1.0f, 0.2f, 0.3f));
            SpawnAuraEffect(entity, point3d(1.0f, 0.0f, 0.3f), 3.0f);

            // Множественные эффекты вокруг
            for (int i = 0; i < 8; i++)
            {
                float angle = (i * PI * 2 / 8);
                point3d offset = point3d(cos(angle), 0, sin(angle)) * 3.0f;
                SpawnAttackEffect(entityStorage, transform->position + offset, point3d(1.0f, 0.2f, 0.5f), 0.8f);
            }
        }

        boss->isTransitioning = false;
    }
    else if (healthPercentage <= 0.7f && boss->currentPhase < 2)
    {
        boss->currentPhase = 2;
        boss->isTransitioning = true;

        // ВИЗУАЛЬНЫЙ ЭФФЕКТ ПЕРЕХОДА В ФАЗУ 2
        Transform* transform = entity->GetComponent<Transform>();
        if (transform)
        {
            SpawnAuraEffect(entity, point3d(1.0f, 0.2f, 0.5f), 3.0f);
            SpawnAttackEffect(entityStorage, transform->position, point3d(0.8f, 0.2f, 0.8f), 2.0f);
        }

        boss->isTransitioning = false;
    }
}
// ============ ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ============

Entity* AISystem::GetNearestPlayer(EntityStorage& entityStorage, Transform* bossTransform)
{
    const std::vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<Health>();
    Entity* nearestPlayer = nullptr;
    float minDistance = FLT_MAX;

    for (Entity* target : entities)
    {
        if (!IsEntityValid(target)) continue;

        Health* health = target->GetComponent<Health>();
        if (health && health->fraction == Fraction::Player)
        {
            Transform* targetTransform = target->GetComponent<Transform>();
            if (targetTransform)
            {
                float distance = (targetTransform->position - bossTransform->position).magnitude();
                if (distance < minDistance)
                {
                    minDistance = distance;
                    nearestPlayer = target;
                }
            }
        }
    }

    // Если нашли игрока - сохраняем его ID
    if (nearestPlayer)
    {
        AIComponent* ai = nearestPlayer->GetComponent<AIComponent>();
        if (ai) ai->targetId = nearestPlayer->GetId();
    }

    return nearestPlayer;
}

float AISystem::GetDistanceToPlayer(EntityStorage& entityStorage, Transform* bossTransform)
{
    Entity* player = GetNearestPlayer(entityStorage, bossTransform);
    if (!player) return FLT_MAX;

    Transform* playerTransform = player->GetComponent<Transform>();
    if (!playerTransform) return FLT_MAX;

    return (playerTransform->position - bossTransform->position).magnitude();
}

// ---------- Патрулирование ----------
void AISystem::UpdatePatrolBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
    PhysicBody* physicBody, float deltaTime)
{
    if (ai->patrolPoints.empty()) return;   // Если нет точек патруля – ничего не делаем

    point3d& targetPoint = ai->patrolPoints[ai->currentPatrolIndex]; // Текущая целевая точка
    point3d direction = targetPoint - transform->position;            // Вектор к цели
    float distance = direction.magnitude();                           // Расстояние до цели

    if (distance < ai->arrivalDistance)   // Если почти дошли
    {
        // Переходим к следующей точке (зацикленно)
        ai->currentPatrolIndex = (ai->currentPatrolIndex + 1) % ai->patrolPoints.size();
        ai->stateTimer = 0.0f;            // Сбрасываем таймер
    }
    else
    {
        direction = direction.normalized();        // Нормализуем направлениеs
        //physicBody->velocity = direction * ai->movementSpeed; // Двигаемся к цели

        point3d targetVelocity = direction * ai->movementSpeed;
        //point3d desiredAccel = (targetVelocity - physicBody->velocity) * ai->accelerationStrength;
        point3d desiredAccel = targetVelocity * ai->accelerationStrength;
        float accelMag = desiredAccel.magnitude();
        if (accelMag > ai->maxAcceleration)
            desiredAccel = desiredAccel.normalized() * ai->maxAcceleration;
        physicBody->acceleration = desiredAccel;
    }

    // Если заметили игрока во время патруля – переключаемся на преследование
    Entity* target = DetectTarget(entityStorage, entity, transform, ai);
    if (target)
    {
        ai->lastKnownPlayerPosition = GetWorldTransform(target).position;
        ai->hasLastKnownPosition = true;
        ai->behaviorType = AIBehaviorType::CHASE;
        ai->stateTimer = 0.0f;
    }
}

// ---------- Преследование ----------
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
    float distance = direction.magnitude();     // Расстояние до игрока

    if (distance > ai->chaseRange)               // Если игрок слишком далеко
    {
        // Потеряли игрока – запоминаем его последнюю позицию и переходим в поиск
        ai->lastKnownPlayerPosition = targetWorldPos;
        ai->hasLastKnownPosition = true;
        ai->behaviorType = AIBehaviorType::SEARCH;
        ai->stateTimer = 0.0f;
    }
    else if (distance <= ai->attackRange)        // Если подошли на расстояние атаки
    {
        ai->behaviorType = AIBehaviorType::ATTACK;
        ai->stateTimer = 0.0f;
    }
    else                                           // Иначе продолжаем двигаться к игроку
    {
        direction = direction.normalized();
        /*physicBody->velocity = direction * ai->movementSpeed;*/
        point3d targetVelocity = direction * ai->movementSpeed;
        //point3d desiredAccel = (targetVelocity - physicBody->velocity) * ai->accelerationStrength;
        point3d desiredAccel = targetVelocity * ai->accelerationStrength;
        float accelMag = desiredAccel.magnitude();
        if (accelMag > ai->maxAcceleration)
            desiredAccel = desiredAccel.normalized() * ai->maxAcceleration;
        physicBody->acceleration = desiredAccel;
    }
}

// ---------- Поиск (после потери игрока) ----------
void AISystem::UpdateSearchBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
    PhysicBody* physicBody, float deltaTime)
{
    // Если нет сохранённой позиции или истекло время поиска – переходим к патрулю вокруг последней позиции
    if (!ai->hasLastKnownPosition || ai->stateTimer >= ai->searchDuration)
    {
        GoToPatrolAroundLastPosition(ai);
        return;
    }

    // Движемся к последней известной позиции игрока
    point3d direction = ai->lastKnownPlayerPosition - transform->position;
    float distance = direction.magnitude();

    if (distance < ai->arrivalDistance)          // Если достигли точки
    {
        GoToPatrolAroundLastPosition(ai);        // Начинаем патрулировать вокруг неё
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

// ---------- Атака ----------
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

    if (distance > ai->attackRange)               // Если игрок вышел из радиуса атаки
    {
        ai->behaviorType = AIBehaviorType::CHASE; // Возвращаемся к преследованию
        ai->stateTimer = 0.0f;
    }
    else
    {
        // Останавливаемся для атаки
        physicBody->velocity = point3d();

        // Проверяем, прошло ли достаточно времени для следующей атаки
        //if (ai->stateTimer >= ai->attackCooldown)
        //{
            // Получаем компонент здоровья игрока
            Health* playerHealth = target->GetComponent<Health>();
            if (playerHealth)
            {
                playerHealth->hp -= ai->attackDamage; // Наносим урон
                if (playerHealth->hp < 0) playerHealth->hp = 0; // Не даём уйти в минус
            }
            ai->stateTimer = 0.0f; // Сбрасываем таймер атаки
        //}
    }
}

// ---------- Бегство ----------
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

    point3d direction = transform->position - targetTransform->position; // Вектор от игрока
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

// ---------- Бездействие ----------
void AISystem::UpdateIdleBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
    PhysicBody* physicBody, float deltaTime)
{
    physicBody->velocity = point3d();            // Стоим на месте

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


// ---------- Вспомогательная функция: переход к патрулю вокруг последней позиции ----------
void AISystem::GoToPatrolAroundLastPosition(AIComponent* ai)
{
    if (ai->hasLastKnownPosition)                  // Если есть сохранённая позиция
    {
        // Генерируем четыре точки патруля вокруг последней позиции игрока
        point3d center = ai->lastKnownPlayerPosition;
        float r = ai->searchPatrolRadius;
        ai->patrolPoints = {
            center + point3d(r, 0,  0),
            center + point3d(0, 0,  r),
            center + point3d(-r, 0,  0),
            center + point3d(0, 0, -r)
        };
        ai->currentPatrolIndex = 0;                 // Начинаем с первой точки
    }
    ai->behaviorType = AIBehaviorType::PATROL;      // Переводим в режим патруля
    ai->stateTimer = 0.0f;                          // Сбрасываем таймер
    ai->hasLastKnownPosition = false;                // Забываем сохранённую позицию
}