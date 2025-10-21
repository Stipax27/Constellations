#ifndef _PHYSIC_SYSTEM_
#define _PHYSIC_SYSTEM_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.cpp"
#include "PhysicBody.cpp"

/////////////
// GLOBALS //
/////////////
const float GravityAcceleration = 9.8f;


class PhysicSystem : public System
{
public:
	PhysicSystem()
	{
		gravityVector = 0;
	}



	void Initialize()
	{
		gravityVector = new point3d(0.0f, -GravityAcceleration, 0.0f);
	}


	void Shutdown()
	{
		if (gravityVector)
		{
			delete gravityVector;
			gravityVector = 0;
		}
	}


	bool Update(vector<Entity*>& entities, float deltaTime)
	{
		size_t size = entities.size();
		for (int i = 0; i < size; i++)
		{
			Entity* entity = entities[i];
			Transform* transform = entity->GetComponent<Transform>();
			PhysicBody* physicBody = entity->GetComponent<PhysicBody>();
			if (transform != nullptr && physicBody != nullptr)
			{
				/*if (physicBody->useGravity)
				{
					physicBody->velocity += *gravityVector * deltaTime;
				}*/

				transform->position += physicBody->velocity * deltaTime;
				//transform->mRotation = XMMatrixMultiply(physicBody->mAngVelocity, transform->mRotation);
				//XMMATRIX x = XMMatrixIdentity();
				//x= XMMatrixMultiply(physicBody->mAngVelocity, transform->mRotation);
				//transform->mRotation = XMMatrixIdentity();
				//transform->mRotation = x;

			}
		}

		return true;
	}

private:
	point3d* gravityVector;
};

#endif