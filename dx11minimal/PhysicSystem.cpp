#ifndef _PHYSIC_SYSTEM_
#define _PHYSIC_SYSTEM_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.cpp"
#include "PhysicBody.cpp"
#include "Projectile.cpp"


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

				Projectile* projectile = entity->GetComponent<Projectile>();
				if (transform != nullptr && projectile != nullptr)
				{
					projectile->time += deltaTime;

					float distanceTraveled = projectile->speed * projectile->time;

					if (projectile->spiralMovement)
					{
						float angleTraveled = projectile->time * projectile->spiralSpeed;
						float localX = cos(angleTraveled) * projectile->spiralRadius;
						float localY = sin(angleTraveled) * projectile->spiralRadius;

						transform->position = projectile->startPosition +
							projectile->direction * distanceTraveled +
							(projectile->spiralVector1 * localX +
								projectile->spiralVector2 * localY);
					}
					else
					{
						transform->position += projectile->direction * projectile->speed * deltaTime;
					}
				}
			}
		}

		return true;
	}
};

#endif