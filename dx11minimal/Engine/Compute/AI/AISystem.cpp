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

        // Получаем необходимые компоненты
        Transform* transform = entity->GetComponent<Transform>();
        AIComponent* ai = entity->GetComponent<AIComponent>();
        PhysicBody* physicBody = entity->GetComponent<PhysicBody>();

        // Если все компоненты есть и ИИ включён – обрабатываем поведение
        if (transform && ai && physicBody && ai->enabled)
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
    }

    ai->stateTimer += deltaTime;   // Увеличиваем таймер текущего состояния
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
        Transform* targetTransform = target->GetComponent<Transform>();

        ai->lastKnownPlayerPosition = targetTransform->position;
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

    Transform* targetTransform = target->GetComponent<Transform>();
    if (!targetTransform) return;

    point3d direction = targetTransform->position - transform->position;
    float distance = direction.magnitude();     // Расстояние до игрока

    if (distance > ai->chaseRange)               // Если игрок слишком далеко
    {
        // Потеряли игрока – запоминаем его последнюю позицию и переходим в поиск
        ai->lastKnownPlayerPosition = targetTransform->position;
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
        Transform* targetTransform = target->GetComponent<Transform>();

        ai->lastKnownPlayerPosition = targetTransform->position;
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

    Transform* targetTransform = target->GetComponent<Transform>();
    if (!targetTransform) return;

    float distance = (targetTransform->position - transform->position).magnitude();

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

        if ((targetTransform->position - transform->position).magnitude() <= ai->detectionRange) {
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