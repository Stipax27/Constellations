#include "GravitySystem.h"

using namespace std;



GravitySystem::GravitySystem()
{
}


void GravitySystem::Initialize()
{
}


void GravitySystem::Shutdown()
{
}


void GravitySystem::Update(EntityStorage& entityStorage, float deltaTime)
{
	// First cycle, gravityPoint searching
	const std::vector<Entity*>& entities1 = entityStorage.GetEntitiesWithComponent<GravityPoint>();
	for (Entity* entity1 : entities1) {
		if (!IsEntityValid(entity1))
			continue;

		Transform* transform1 = entity1->GetComponent<Transform>();
		if (transform1 == nullptr)
			continue;

		GravityPoint* gravityPoint = entity1->GetComponent<GravityPoint>();
		if (gravityPoint == nullptr || !gravityPoint->active)
			continue;

		// Second cycle, physicBody searching
		const vector<Entity*>& entities2 = entityStorage.GetEntitiesWithComponent<PhysicBody>();
		for (Entity* entity2 : entities2) {
			if (entity2 == entity1 || !IsEntityValid(entity2))
				continue;

			Transform* transform2 = entity2->GetComponent<Transform>();
			if (transform2 == nullptr)
				continue;

			PhysicBody* physicBody = entity2->GetComponent<PhysicBody>();
			if (physicBody == nullptr || !physicBody->active || !physicBody->useGravity)
				continue;

			// Main processing
			point3d gravityVector = GetWorldTransform(entity1).position - GetWorldTransform(entity2).position;
			float radius = gravityVector.magnitude();

			if (radius <= gravityPoint->radius && radius > 0) {
				float falloff = CalcFalloff(radius, gravityPoint->radius);
				float gravityForce = GRAVITY_CONSTANT * (gravityPoint->mass * physicBody->mass) / max(pow(radius, 1.5f), 1) * falloff;

				physicBody->acceleration += gravityVector.normalized() * gravityForce;

				XMMATRIX newMatrix = TransformMatrixToUpVector(transform2->mRotation, -gravityVector);
				transform2->mRotation = LerpMatrix(transform2->mRotation, newMatrix, 0.05f * falloff);
			}
		}
	}
}


float GravitySystem::CalcFalloff(float radius, float maxRadius) {
	float fadeStart = maxRadius * GRAVITATION_FADE_START;

	if (radius < fadeStart) {
		return 1;
	}
	else {
		float t = (radius - fadeStart) / (maxRadius - fadeStart);
		return 1 - (3 * pow(t, 2) - 2 * pow(t, 3));
	}
}