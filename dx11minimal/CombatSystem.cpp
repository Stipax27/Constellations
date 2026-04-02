#include "combatSystem.h"



CombatSystem::CombatSystem()
{
}


void CombatSystem::Initialize()
{
}


void CombatSystem::Shutdown()
{
}


void CombatSystem::Update(vector<Entity*>& entities, float deltaTime)
{
	size_t size = entities.size();
	for (Entity* entity : entities) {
		if (!IsEntityValid(entity))
			continue;
				
		Health* health = entity->GetComponent<Health>();
		if (health == nullptr || !health->active)
			continue;

		if (!health->immortal) {
			for (DamageUnit damageUnit : health->damageQueue) {
				health->hp -= damageUnit.damage;
			}
		}
		health->damageQueue.clear();

		health->hp = clamp(health->hp, 0.0f, health->maxHp);
		if (health->hp == 0.0f && health->destroyOnDeath) {
			entity->Destroy();
		}
	}
}