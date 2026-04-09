#include "physicSystem.h"

using namespace std;



PhysicSystem::PhysicSystem()
{
}


void PhysicSystem::Initialize()
{
}


void PhysicSystem::Shutdown()
{
}


void PhysicSystem::Update(EntityStorage& entityStorage, float deltaTime)
{
	const std::vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<PhysicBody>();

	for (Entity* entity : entities)
	{
		if (!IsEntityValid(entity) || entity->GetTimeScale() == 0.0f)
			continue;

		Transform* transform = entity->GetComponent<Transform>();
		PhysicBody* physicBody = entity->GetComponent<PhysicBody>();
		if (transform == nullptr || physicBody == nullptr || !physicBody->active)
			continue;

		float dTime = physicBody->preciseMovement ? 0.01f : timer::deltaTime * 0.001f;

		physicBody->velocity += physicBody->acceleration * dTime * entity->GetTimeScale();

		float velMag = physicBody->velocity.magnitude();
		if (velMag > 0.0f) {
			// Сила сопротивления пропорциональна скорости (вязкое трение)
			// Или квадрату скорости (турбулентное трение) - раскомментируйте нужное
					
			//float frictionForce = SPACE_DENSITY * physicBody->airFriction * velMag; // линейное
			//float frictionForce = SPACE_DENSITY * velMag * physicBody->airFriction;

			float frictionForce = SPACE_DENSITY * physicBody->airFriction * pow(velMag, 1.5f); // квадратичное

			float deltaV = frictionForce * dTime;
			if (deltaV >= velMag) {
				physicBody->velocity = point3d(0, 0, 0);
			}
			else {
				point3d frictionVel = -physicBody->velocity.normalized() * deltaV;
				physicBody->velocity += frictionVel;
			}
		}

		transform->position += physicBody->velocity * dTime * entity->GetTimeScale();
		XMMATRIX result = physicBody->mAngVelocity * transform->mRotation;
		transform->mRotation = result;

		physicBody->mAngVelocity = XMMatrixIdentity();
		physicBody->acceleration = point3d();
	}
}