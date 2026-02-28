#ifndef _AI_SYSTEM_
#define _AI_SYSTEM_

#include<cmath>
#include "system.h"
#include "Transform.h"
#include "Enemy_Patern.h"
#include "PhysicBody.h"
#include "Star.h"

const float AI_UPDATE_INTERVAL = 0.01f;

class AISystem : public System
{
public:
    AISystem()
    {
        playerEntity = nullptr;
        lastUpdateTime = 0.0f;
    }

    void Initialize() override {}
    void Shutdown() override { playerEntity = nullptr; }

    void SetPlayerEntity(Entity* player) { playerEntity = player; }

    void Update(vector<Entity*>& entities, float deltaTime) override
    {
        lastUpdateTime += deltaTime;
        if (lastUpdateTime < AI_UPDATE_INTERVAL)
            return;

        for (Entity* entity : entities)
        {
            if (!entity->IsActive()) continue;

            Transform* transform = entity->GetComponent<Transform>();
            AIComponent* ai = entity->GetComponent<AIComponent>();
            PhysicBody* physicBody = entity->GetComponent<PhysicBody>();

            if (transform && ai && physicBody && ai->enabled)
            {
                ProcessAIBehavior(entity, transform, ai, physicBody, deltaTime);
            }
        }
        lastUpdateTime = 0.0f;
    }

private:
    Entity* playerEntity;
    float lastUpdateTime;

    void ProcessAIBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime)
    {
        // Обновление цвета звезды (опционально)
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
            case AIBehaviorType::SEARCH: star->color1 = point3d(1.0f, 1.0f, 0.0f); break; // жёлтый
            }
        }

        switch (ai->behaviorType)
        {
        case AIBehaviorType::PATROL:
            UpdatePatrolBehavior(entity, transform, ai, physicBody, deltaTime);
            break;
        case AIBehaviorType::CHASE:
            UpdateChaseBehavior(entity, transform, ai, physicBody, deltaTime);
            break;
        case AIBehaviorType::ATTACK:
            UpdateAttackBehavior(entity, transform, ai, physicBody, deltaTime);
            break;
        case AIBehaviorType::FLEE:
            UpdateFleeBehavior(entity, transform, ai, physicBody, deltaTime);
            break;
        case AIBehaviorType::IDLE:
            UpdateIdleBehavior(entity, transform, ai, physicBody, deltaTime);
            break;
        case AIBehaviorType::SEARCH:
            UpdateSearchBehavior(entity, transform, ai, physicBody, deltaTime);
            break;
        }

        ai->stateTimer += deltaTime;
    }

    // ---------- Патрулирование ----------
    void UpdatePatrolBehavior(Entity* entity, Transform* transform, AIComponent* ai,
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
            physicBody->velocity = direction * ai->movementSpeed;
        }

        if (playerEntity && DetectPlayer(transform, ai))
        {
            // Запоминаем позицию, где увидели игрока (текущая позиция игрока)
            Transform* playerTransform = playerEntity->GetComponent<Transform>();
            if (playerTransform)
                ai->lastKnownPlayerPosition = playerTransform->position;
            ai->hasLastKnownPosition = true;
            ai->behaviorType = AIBehaviorType::CHASE;
            ai->stateTimer = 0.0f;
        }
    }

    // ---------- Преследование ----------
    void UpdateChaseBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime)
    {
        if (!playerEntity)
        {
            ai->behaviorType = AIBehaviorType::PATROL;
            return;
        }

        Transform* playerTransform = playerEntity->GetComponent<Transform>();
        if (!playerTransform) return;

        point3d direction = playerTransform->position - transform->position;
        float distance = direction.magnitude();

        if (distance > ai->chaseRange)
        {
            // Потеряли игрока – запоминаем его последнюю позицию и переходим в поиск
            ai->lastKnownPlayerPosition = playerTransform->position;
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
            physicBody->velocity = direction * ai->movementSpeed;
        }
    }

    // ---------- Поиск (новое состояние) ----------
    void UpdateSearchBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime)
    {
        // Если нет последней известной позиции или время поиска истекло – переходим к патрулю
        if (!ai->hasLastKnownPosition || ai->stateTimer >= ai->searchDuration)
        {
            GoToPatrolAroundLastPosition(ai);
            return;
        }

        // Движемся к последней известной позиции
        point3d direction = ai->lastKnownPlayerPosition - transform->position;
        float distance = direction.magnitude();

        if (distance < ai->arrivalDistance)
        {
            // Достигли точки – начинаем патрулировать вокруг неё
            GoToPatrolAroundLastPosition(ai);
            return;
        }

        direction = direction.normalized();
        physicBody->velocity = direction * ai->movementSpeed;

        // Во время движения проверяем, не появился ли игрок снова
        if (playerEntity && DetectPlayer(transform, ai))
        {
            // Снова видим игрока – переходим в преследование
            Transform* playerTransform = playerEntity->GetComponent<Transform>();
            if (playerTransform)
                ai->lastKnownPlayerPosition = playerTransform->position;
            ai->behaviorType = AIBehaviorType::CHASE;
            ai->stateTimer = 0.0f;
        }
    }

    // ---------- Атака ----------
    void UpdateAttackBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime)
    {
        if (!playerEntity)
        {
            ai->behaviorType = AIBehaviorType::PATROL;
            return;
        }

        Transform* playerTransform = playerEntity->GetComponent<Transform>();
        if (!playerTransform) return;

        float distance = (playerTransform->position - transform->position).magnitude();

        if (distance > ai->attackRange)
        {
            ai->behaviorType = AIBehaviorType::CHASE;
            ai->stateTimer = 0.0f;
        }
        else
        {
            physicBody->velocity = point3d();
            if (ai->stateTimer >= ai->attackCooldown)
            {
                // PerformAttack(entity, transform, ai); // ваша реализация атаки
                ai->stateTimer = 0.0f;
            }
        }
    }

    // ---------- Бегство ----------
    void UpdateFleeBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime)
    {
        if (!playerEntity)
        {
            ai->behaviorType = AIBehaviorType::PATROL;
            return;
        }

        Transform* playerTransform = playerEntity->GetComponent<Transform>();
        if (!playerTransform) return;

        point3d direction = transform->position - playerTransform->position;
        direction = direction.normalized();
        physicBody->velocity = direction * ai->movementSpeed;

        if (ai->stateTimer >= ai->fleeDuration)
        {
            ai->behaviorType = AIBehaviorType::PATROL;
            ai->stateTimer = 0.0f;
        }
    }

    // ---------- Бездействие ----------
    void UpdateIdleBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime)
    {
        physicBody->velocity = point3d();

        if (playerEntity && DetectPlayer(transform, ai))
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

    // ---------- Обнаружение игрока ----------
    bool DetectPlayer(Transform* transform, AIComponent* ai)
    {
        if (!playerEntity) return false;
        Transform* playerTransform = playerEntity->GetComponent<Transform>();
        if (!playerTransform) return false;
        float distance = (playerTransform->position - transform->position).magnitude();
        return distance <= ai->detectionRange;
    }

    // ---------- Вспомогательная функция: переход к патрулю вокруг последней позиции ----------
    void GoToPatrolAroundLastPosition(AIComponent* ai)
    {
        if (ai->hasLastKnownPosition)
        {
            // Генерируем четыре точки вокруг последней позиции игрока
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
        ai->hasLastKnownPosition = false; // сбрасываем, чтобы не использовать повторно
    }
};

#endif