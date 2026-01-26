#ifndef _PHYSIC_SYSTEM_
#define _PHYSIC_SYSTEM_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.h"
#include "PhysicBody.h"


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


	void Update(vector<Entity*>& entities, float deltaTime)
	{
		size_t size = entities.size();
		for (int i = 0; i < size; i++) {
			Entity* entity = entities[i];
			if (IsEntityValid(entity)) {
				Transform* transform = entity->GetComponent<Transform>();
				PhysicBody* physicBody = entity->GetComponent<PhysicBody>();
				if (transform != nullptr && physicBody != nullptr && physicBody->active) {

					float dTime = physicBody->preciseMovement ? 0.01f : deltaTime;

					transform->position += physicBody->velocity * dTime * entity->timeScale;
					XMMATRIX result = physicBody->mAngVelocity * transform->mRotation;
					transform->mRotation = result;
					
					float velMagnitude = physicBody->velocity.magnitude();
					if (velMagnitude > 0) {
						float deceleration = min(SPACE_DENSITY / velMagnitude * physicBody->airFriction * dTime, 1);
						physicBody->velocity = physicBody->velocity.lerp(point3d(), deceleration);
					}
					
					physicBody->mAngVelocity = XMMatrixIdentity();
				}
			}
		}
	}
};

#endif