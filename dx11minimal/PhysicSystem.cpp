#include "physicSystem.h"



PhysicSystem::PhysicSystem()
{
}


void PhysicSystem::Initialize()
{
}


void PhysicSystem::Shutdown()
{
}


void PhysicSystem::Update(vector<Entity*>& entities, float deltaTime)
{
	size_t size = entities.size();
	for (int i = 0; i < size; i++) {
		Entity* entity = entities[i];
		if (IsEntityValid(entity)) {
			Transform* transform = entity->GetComponent<Transform>();
			PhysicBody* physicBody = entity->GetComponent<PhysicBody>();
			if (transform != nullptr && physicBody != nullptr && physicBody->active) {

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





				//// Новое торможение
				//float velMag = physicBody->velocity.magnitude();
				//if (velMag > 0.0f) {
				//	float frictionStrength = SPACE_DENSITY * physicBody->airFriction;
				//	float maxDecel = velMag / dTime;
				//	float actualFriction = min(frictionStrength, maxDecel);
				//	point3d frictionAccel = -physicBody->velocity.normalized() * actualFriction;
				//	physicBody->acceleration += frictionAccel;
				//}

				//physicBody->velocity += physicBody->acceleration * dTime * entity->GetTimeScale();

				//transform->position += physicBody->velocity * dTime * entity->GetTimeScale();
				//XMMATRIX result = physicBody->mAngVelocity * transform->mRotation;
				//transform->mRotation = result;
				//
				//// Старое торможение
				///*float velMagnitude = physicBody->velocity.magnitude();
				//if (velMagnitude > 0) {
				//	float deceleration = min(SPACE_DENSITY / velMagnitude * physicBody->airFriction * dTime, 1);
				//	physicBody->velocity = physicBody->velocity.lerp(point3d(), deceleration);
				//}*/

				//	
				//physicBody->mAngVelocity = XMMatrixIdentity();
				//physicBody->acceleration = point3d();
			}
		}
	}
}