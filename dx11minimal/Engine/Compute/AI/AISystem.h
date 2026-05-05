#ifndef _AI_SYSTEM_H_
#define _AI_SYSTEM_H_

#include<cmath>
#include "../../ECS_Base/system.h"
#include "../../BasicComponents/Transform.h"
#include "../../Physic/Movement/PhysicBody.h"
#include "AIComponent.h"
#include "BossComponent.h"
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

    // ---------- НОВЫЕ методы для босса ----------
    void UpdateBossBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform,
        AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, float deltaTime);

    void UpdateBossPhase1(EntityStorage& entityStorage, Entity* entity, Transform* transform,
        AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, Star* star, float deltaTime);

    void UpdateBossPhase2(EntityStorage& entityStorage, Entity* entity, Transform* transform,
        AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, float deltaTime);

    void UpdateBossPhase3(EntityStorage& entityStorage, Entity* entity, Transform* transform,
        AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, Star* star, float deltaTime);

    // ---------- Специальные атаки босса ----------
    void BossSpecialAttack(EntityStorage& entityStorage, Entity* entity, Transform* transform,
        AIComponent* ai,BossComponent* boss, PhysicBody* physicBody, Star* star);

    //void BossSummonMinions(EntityStorage& entityStorage, Transform* transform, BossComponent* boss);

    void BossAOEAttack(EntityStorage& entityStorage, Transform* transform, BossComponent* boss);

    void CheckBossPhaseTransition(EntityStorage& entityStorage, Entity* entity, Health* health,
        BossComponent* boss, AIComponent* ai);


    Entity* DetectTarget(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai);

    // ---------- Вспомогательная функция: переход к патрулю вокруг последней позиции ----------
    void GoToPatrolAroundLastPosition(AIComponent* ai);


    // ---------- Вспомогательные методы для босса ----------
    Entity* GetNearestPlayer(EntityStorage& entityStorage, Transform* bossTransform);
    float GetDistanceToPlayer(EntityStorage& entityStorage, Transform* bossTransform);
};

#endif