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
	for (int i = 0; i < size; i++) {
		Entity* entity = entities[i];
		if (IsEntityValid(entity)) {
				
			Health* health = entity->GetComponent<Health>();
			if (health != nullptr && health->active) {
				health->hp = health->immortal ? health->maxHp : clamp(health->hp, 0, health->maxHp);
				if (health->hp == 0.0f && health->destroyOnDeath) {
					entity->Destroy();
				}
			}

		}
	}
}