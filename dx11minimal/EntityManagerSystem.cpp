#ifndef _ENTITY_MANAGER_SYSTEM_
#define _ENTITY_MANAGER_SYSTEM_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "timer.h"

#include "DelayedDestroy.cpp"


class EntityManagerSystem : public System
{
public:
	EntityManagerSystem()
	{
	}


	void Initialize()
	{
	}


	void Shutdown()
	{
	}


	void Update(vector<Entity*>& entities, float deltaTime)
	{
		size_t size = entities.size();
		for (int i = 0; i < size; i++) {
			Entity* entity = entities[i];
			if (IsEntityValid(entity)) {
				DelayedDestroy* delayedDestroy = entity->GetComponent<DelayedDestroy>();
				if (delayedDestroy != nullptr && timer::currentTime >= delayedDestroy->startTime + delayedDestroy->lifeTime) {
					entity->Destroy();
				}
			}
		}
	}
};

#endif