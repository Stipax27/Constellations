#ifndef _AI_SYSTEM_H_
#define _AI_SYSTEM_H_

#include<cmath>
#include "../../ECS_Base/system.h"
#include "../../BasicComponents/Transform.h"
#include "../../Physic/Movement/PhysicBody.h"
#include "AIComponent.h"

#include "../../Render/Star.h"
#include "../../Compute/Combat/Health.h"



class AISystem : public System
{
public:
    AISystem();

    void Initialize() override;
    void Shutdown() override;

    void Update(EntityStorage& entityStorage, float deltaTime) override;

private:
    void ProcessAIBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);

    // ---------- Патрулирование ----------
    void UpdatePatrolBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);

    // ---------- Преследование ----------
    void UpdateChaseBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);

    // ---------- Поиск (после потери игрока) ----------
    void UpdateSearchBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);

    // ---------- Атака ----------
    void UpdateAttackBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);

    // ---------- Бегство ----------
    void UpdateFleeBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);

    // ---------- Бездействие ----------
    void UpdateIdleBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);


    Entity* DetectTarget(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai);

    // ---------- Вспомогательная функция: переход к патрулю вокруг последней позиции ----------
    void GoToPatrolAroundLastPosition(AIComponent* ai);
};

#endif