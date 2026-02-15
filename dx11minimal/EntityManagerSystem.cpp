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
			if (delayedDestroy != nullptr && delayedDestroy->active) {

				if (delayedDestroy->startTime < 0) {
					delayedDestroy->startTime = entity->localTime;
					continue;
				}

				if (entity->localTime >= delayedDestroy->startTime + delayedDestroy->lifeTime) {
					entity->Destroy();
				}

			}
		}
	}
}