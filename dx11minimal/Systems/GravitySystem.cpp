#include "GravitySystem.h"



GravitySystem::GravitySystem()
{
}


void GravitySystem::Initialize()
{
}


void GravitySystem::Shutdown()
{
}


void GravitySystem::Update(vector<Entity*>& entities, float deltaTime)
{
	// First cycle, gravityPoint searching
	for (Entity* entity1 : entities) {
		if (!IsEntityValid(entity1))
			continue;

		Transform* transform1 = entity1->GetComponent<Transform>();
		if (transform1 == nullptr)
			continue;

		GravityPoint* gravityPoint = entity1->GetComponent<GravityPoint>();
		if (gravityPoint == nullptr || !gravityPoint->active)
			continue;

		// Second cycle, physicBody searching
		for (Entity* entity2 : entities) {
			if (entity2 == entity1 || !IsEntityValid(entity2))
				continue;

			Transform* transform2 = entity2->GetComponent<Transform>();
			if (transform2 == nullptr)
				continue;

			PhysicBody* physicBody = entity2->GetComponent<PhysicBody>();
			if (physicBody == nullptr || !physicBody->active || !physicBody->useGravity)
				continue;

			// Main processing
			point3d gravityVector = GetWorldTransform(entity1).position - GetWorldTransform(entity2).position);
			float radius = (GetWorldTransform(entity1).position - GetWorldTransform(entity2).position).magnitude();
			float gravityForce = GRAVITY_CONSTANT * (gravityPoint->mass * physicBody->mass) / pow(radius, 2);

		}
	}
}