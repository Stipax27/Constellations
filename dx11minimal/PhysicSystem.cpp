#ifndef _PHYSIC_SYSTEM_
#define _PHYSIC_SYSTEM_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.cpp"
#include "PhysicBody.cpp"


class PhysicSystem : public System
{
public:
	PhysicSystem()
	{
	}



	void Initialize()
	{
	}


	void Shutdown()
	{
	}


	bool Update(vector<Entity*>& entities, float deltaTime)
	{
		size_t size = entities.size();
		for (int i = 0; i < size; i++) {
			Entity* entity = entities[i];
			if (entity->IsActive()) {
				Transform* transform = entity->GetComponent<Transform>();
				PhysicBody* physicBody = entity->GetComponent<PhysicBody>();
				if (transform != nullptr && physicBody != nullptr) {

					transform->position += physicBody->velocity * deltaTime * entity->timeScale;
					XMMATRIX result = physicBody->mAngVelocity * transform->mRotation;
					transform->mRotation = result;
					
					float velMagnitude = physicBody->velocity.magnitude();
					if (velMagnitude > 0) {
						float deceleration = min(SPACE_DENSITY / velMagnitude * deltaTime, 1);
						physicBody->velocity = physicBody->velocity.lerp(point3d(), deceleration);
					}
					
					physicBody->mAngVelocity = XMMatrixIdentity();
				}
			}
		}

		return true;
	}
};

#endif