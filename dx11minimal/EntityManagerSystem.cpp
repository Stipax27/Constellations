#include "entityManagerSystem.h"



EntityManagerSystem::EntityManagerSystem()
{
}


void EntityManagerSystem::Initialize()
{
}


void EntityManagerSystem::Shutdown()
{
}


void EntityManagerSystem::Update(vector<Entity*>& entities, float deltaTime)
{
	size_t size = entities.size();
	for (int i = 0; i < size; i++) {
		Entity* entity = entities[i];
		if (IsEntityValid(entity)) {
			DelayedDestroy* delayedDestroy = entity->GetComponent<DelayedDestroy>();
			if (delayedDestroy != nullptr && delayedDestroy->active && timer::currentTime >= delayedDestroy->startTime + delayedDestroy->lifeTime / entity->timeScale) {
				entity->Destroy();
			}
		}
	}
}