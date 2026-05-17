#include "RayDamageSystem.h"

using namespace std;



RayDamageSystem::RayDamageSystem()
{
}


void RayDamageSystem::Initialize()
{
	collisionManagerClass = Singleton::GetInstance<CollisionManagerClass>();
}


void RayDamageSystem::Shutdown()
{
	if (collisionManagerClass)
		collisionManagerClass = 0;
}


void RayDamageSystem::Update(EntityStorage& entityStorage, float deltaTime)
{
	const std::vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<RayDamager>();
	for (Entity* entity : entities)
	{
		if (!IsEntityValid(entity) || entity->GetTimeScale() == 0.0f)
			continue;

		RayDamager* rayDamager = entity->GetComponent<RayDamager>();
		if (rayDamager == nullptr || !rayDamager->active || entity->localTime - rayDamager->lastDamageTime < rayDamager->interval)
			continue;

		Transform worldTransform = GetWorldTransform(entity);

		RayInfo rayInfo = RayInfo(worldTransform.position, rayDamager->direction, rayDamager->collisionGroup, rayDamager->touchableOnly);
		RaycastResult result = collisionManagerClass->Raycast(rayInfo);
		if (!result.hit)
			continue;

		Entity* target = result.entity;
		pair<Entity*, Health*> hres = target->GetAncestorWithComponent<Health>();
		if (hres.first != nullptr && hres.second->active && rayDamager->target == hres.second->fraction) {

			DamageUnit unit = DamageUnit(rayDamager->damageType, rayDamager->damage);
			hres.second->damageQueue.push_back(unit);

			rayDamager->lastDamageTime = entity->localTime;
			rayDamager->repeatCount++;

			if (rayDamager->repeats >= 0 && rayDamager->repeatCount >= rayDamager->repeats) {
				entity->RemoveComponent<RayDamager>();
			}

		}
	}
}