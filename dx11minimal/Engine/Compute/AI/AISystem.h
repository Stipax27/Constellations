#ifndef _AI_SYSTEM_H_
#define _AI_SYSTEM_H_

#include<cmath>
#include "../../ECS_Base/system.h"
#include "../../BasicComponents/Transform.h"
#include "../../Physic/Movement/PhysicBody.h"
#include "AIComponent.h"
#include "BossComponent.h"
#include "../../UI/Text/TextLabel.h"
#include "../../Render/ParticleEmitter.h"
#include "../../Render/Beam.h"
#include "../../Compute/DelayedDestroy/DelayedDestroy.h"
#include "../../Compute/Combat/SingleDamager.h"
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
    void ExecutePendingAttack(EntityStorage& entityStorage, Entity* entity, Transform* transform,
        AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, Star* star);

    void StartChargeAttack(AIComponent* ai, BossComponent* boss,
        AIComponent::AttackType attackType, float chargeDuration);
       
    void UpdateBossBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform,
        AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, float deltaTime);

    void UpdateBossPhase1(EntityStorage& entityStorage, Entity* entity, Transform* transform,
        AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, Star* star, float deltaTime);

    void UpdateBossPhase2(EntityStorage& entityStorage, Entity* entity, Transform* transform,
        AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, Star* star, float deltaTime);

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
    Entity* GetNearestPlayer(EntityStorage& entityStorage, Entity* bossEntity);
    float GetDistanceToPlayer(EntityStorage& entityStorage, Entity* bossEntity);

    // Визуальные эффекты для атак босса
    void SpawnAttackEffect(EntityStorage& entityStorage, Entity* entity,  point3d& position, const point3d& color, float size, float timeCharge);
    void SpawnDashEffect(EntityStorage& entityStorage, Entity* entity,    point3d& position, const point3d& color, float size, float timeCharge);
    void SpawnSideEffect(EntityStorage& entityStorage, Entity* entity,    point3d& position, const point3d& color, float size, float timeCharge);
    void SpawnSlashEffect(EntityStorage& entityStorage,  const point3d& position, const point3d& direction, const point3d& color);
    void SpawnImpactEffect(EntityStorage& entityStorage, const point3d& position, const point3d& color);
    void SpawnAuraEffect(Entity* bossEntity, const point3d& color, float duration);

    // Эффекты для разных типов атак
    void SpawnMeleeAttackEffect(EntityStorage& entityStorage, const point3d& position, const point3d& direction);
    void SpawnRangedAttackEffect(EntityStorage& entityStorage, const point3d& position, const point3d& direction);
    void SpawnAOEEffect(EntityStorage& entityStorage, const point3d& position, float radius, const point3d& color);

    void BossSideDash(EntityStorage& entityStorage, Entity* entity, Transform* transform,
        AIComponent* ai, BossComponent* boss, PhysicBody* physicBody);

    void BossDashAttack(EntityStorage& entityStorage, Entity* entity, Transform* transform,
        AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, Star* star);

    void BossStarShot(EntityStorage& entityStorage, Entity* entity, Transform* transform,
        AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, Star* star);

    void UpdateBossCircleMovement(EntityStorage& entityStorage, Entity* entity, Transform* transform,
        AIComponent* ai, BossComponent* boss, PhysicBody* physicBody, float deltaTime);

    point3d GetGlobalPosition(Entity* entity);

};

#endif