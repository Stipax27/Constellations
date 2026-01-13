#ifndef _COMBAT_SYSTEM_
#define _COMBAT_SYSTEM_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.cpp"
#include "Health.cpp"


class CombatSystem : public System
{
public:
	CombatSystem()
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
				
				Health* health = entity->GetComponent<Health>();
				if (health != nullptr) {
					health->hp = clamp(health->hp, 0, health->maxHp);
					if (health->hp == 0.0f) {
						entity->Destroy();
					}
				}

			}
		}
	}
};

#endif