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



	void Initialize() override
	{
		gravityVector = new point3d(0.0f, -GravityAcceleration, 0.0f);
	}


	void Shutdown() override
	{
		if (gravityVector)
		{
			delete gravityVector;
			gravityVector = 0;
		}
	}


	void Update(vector<Entity*>& entities, float deltaTime) override
	{
		size_t size = entities.size();
		for (int i = 0; i < size; i++)
		{
			Entity* entity = entities[i];
			PhysicBody* physicBody = entity->GetComponent<PhysicBody>();
			Transform* transform = entity->GetComponent<Transform>();
			if (physicBody != nullptr && transform != nullptr)
			{

				if (physicBody->useGravity)
				{
					physicBody->velocity += *gravityVector * deltaTime;
				}

				transform->position += physicBody->velocity * deltaTime;
			}
		}
	}

private:
	point3d* gravityVector;
};