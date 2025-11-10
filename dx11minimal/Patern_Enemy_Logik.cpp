
#ifndef _AI_SYSTEM_
#define _AI_SYSTEM_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.cpp"
#include "Enemy_Patern.h"
#include "PhysicBody.cpp"


/////////////
// GLOBALS //
/////////////
const float AI_UPDATE_INTERVAL = 0.1f; // Частота обновления ИИ в секундах

class AISystem : public System
{
public:
    AISystem()
    {
        playerEntity = nullptr;
        lastUpdateTime = 0.0f;
    }

    void Initialize()
    {
        //enemyPhysicBody;
    }

    void Shutdown()
    {
        playerEntity = nullptr;
    }

    // Установка цели (игрока) для врагов
    void SetPlayerEntity(Entity* player)
    {
        playerEntity = player;
    }

    bool Update(vector<Entity*>& entities, float deltaTime)
    {
        lastUpdateTime += deltaTime;

        // Обновляем ИИ с определенным интервалом для оптимизации
        if (lastUpdateTime < AI_UPDATE_INTERVAL)
            return true;

        size_t size = entities.size();
        for (int i = 0; i < size; i++)
        {
            Entity* entity = entities[i];
            if (entity->active)
            {
                Transform* transform = entity->GetComponent<Transform>();
                AIComponent* aiComponent = entity->GetComponent<AIComponent>();
                PhysicBody* physicBody = entity->GetComponent<PhysicBody>();

                if (transform != nullptr && aiComponent != nullptr && physicBody != nullptr)
                {
                    ProcessAIBehavior(entity, transform, aiComponent, physicBody, deltaTime);
                }
            }
        }

        lastUpdateTime = 0.0f;
        return true;
    }

private:
    Entity* playerEntity;
    float lastUpdateTime;

    void ProcessAIBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime)
    {
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
        }

        // Обновление таймеров состояний
        ai->stateTimer += deltaTime;
    }

    void UpdatePatrolBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime)
    {
        if (ai->patrolPoints.empty()) return;

        // Получаем текущую точку патрулирования
        point3d& targetPoint = ai->patrolPoints[ai->currentPatrolIndex];

        // Движемся к точке
        point3d direction = (targetPoint - transform->position);
        float distance = direction.magnitude();

        //XMMATRIX additionalRotation = XMMatrixRotationRollPitchYaw(XMConvertToRadians(mousePos.y), XMConvertToRadians(mousePos.x), 0);

        //enemyPhysicBody->mAngVelocity = enemyPhysicBody->mAngVelocity * additionalRotation;

        if (distance < ai->arrivalDistance)
        {
            // Достигли точки, переходим к следующей
            ai->currentPatrolIndex = (ai->currentPatrolIndex + 1) % ai->patrolPoints.size();
            ai->stateTimer = 0.0f;
        }
        else
        {
            // Движение к точке
            direction.normalized();
            physicBody->velocity = direction * ai->movementSpeed;

            // Проверка на обнаружение игрока
            if (playerEntity && DetectPlayer(transform, ai))
            {
                ai->behaviorType = AIBehaviorType::CHASE;
                ai->stateTimer = 0.0f;
            }
        }
    }

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
            // Игрок слишком далеко, возвращаемся к патрулированию
            ai->behaviorType = AIBehaviorType::PATROL;
            ai->stateTimer = 0.0f;
        }
        else if (distance <= ai->attackRange)
        {
            // Достаточно близко для атаки
            ai->behaviorType = AIBehaviorType::ATTACK;
            ai->stateTimer = 0.0f;
        }
        else
        {
            // Продолжаем преследование
            direction.normalized();
            physicBody->velocity = direction * ai->movementSpeed;
        }
    }

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

        point3d direction = playerTransform->position - transform->position;
        float distance = direction.magnitude();

        if (distance > ai->attackRange)
        {
            // Игрок ушел из радиуса атаки
            ai->behaviorType = AIBehaviorType::CHASE;
            ai->stateTimer = 0.0f;
        }
        else
        {
            // Атакуем игрока
            physicBody->velocity = point3d(); // Останавливаемся для атаки

            // Реализация логики атаки
            if (ai->stateTimer >= ai->attackCooldown)
            {
                PerformAttack(entity, transform, ai);
                ai->stateTimer = 0.0f;
            }
        }
    }

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
        direction.normalized();

        physicBody->velocity = direction * ai->movementSpeed;

        // Проверяем, можно ли прекратить бегство
        if (ai->stateTimer >= ai->fleeDuration)
        {
            ai->behaviorType = AIBehaviorType::PATROL;
            ai->stateTimer = 0.0f;
        }
    }

    void UpdateIdleBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime)
    {
        physicBody->velocity = point3d(); // Стоим на месте

        // Проверка на обнаружение игрока
        if (playerEntity && DetectPlayer(transform, ai))
        {
            ai->behaviorType = AIBehaviorType::CHASE;
            ai->stateTimer = 0.0f;
        }

        // Через некоторое время возвращаемся к патрулированию
        if (ai->stateTimer >= ai->idleDuration)
        {
            ai->behaviorType = AIBehaviorType::PATROL;
            ai->stateTimer = 0.0f;
        }
    }

    bool DetectPlayer(Transform* transform, AIComponent* ai)
    {
        if (!playerEntity) return false;

        Transform* playerTransform = playerEntity->GetComponent<Transform>();
        if (!playerTransform) return false;

        point3d direction = playerTransform->position - transform->position;
        float distance = direction.magnitude();

        return distance <= ai->detectionRange;
    }
};

#endif