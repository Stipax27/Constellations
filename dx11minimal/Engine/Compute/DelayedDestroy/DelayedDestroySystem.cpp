#include "DelayedDestroySystem.h"

using namespace std;



DelayedDestroySystem::DelayedDestroySystem()
{
}


void DelayedDestroySystem::Initialize()
{
}


void DelayedDestroySystem::Shutdown()
{
}


void DelayedDestroySystem::Update(EntityStorage& entityStorage, float deltaTime)
{
	const std::vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<DelayedDestroy>();
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