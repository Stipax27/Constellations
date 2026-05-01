#include "collisionSystem.h"

using namespace std;



CollisionSystem::CollisionSystem()
{
	collisionManager = Singleton::GetInstance<CollisionManagerClass>();
}


void CollisionSystem::Initialize()
{
}


void CollisionSystem::Shutdown()
{
	if (collisionManager) {
		collisionManager = 0;
	}
}

void CollisionSystem::Update(EntityStorage& entityStorage, float deltaTime)
{
	for (Entity* entity1 : entityStorage.entities)
	{
		if (!IsEntityValid(entity1))
			continue;
		
		Transform* transform1 = entity1->GetUpperComponent<Transform>();
		PhysicBody* physicBody1 = entity1->GetComponentInAncestor<PhysicBody>();
		SphereCollider* collider1 = entity1->GetComponent<SphereCollider>();
		if (transform1 == nullptr || collider1 == nullptr || !collider1->active)
			continue;

		collider1->collisions.clear();
		Transform worldTransform1 = GetWorldTransform(entity1);
		size_t size = entityStorage.entities.size();

		for (Entity* entity2 : entityStorage.entities)
		{
			if (entity2 == entity1 || !IsEntityValid(entity2) || (entity1->GetTimeScale() == 0.0f && entity2->GetTimeScale() == 0.0f))
				continue;

			PlaneCollider* collider2a = entity2->GetComponent<PlaneCollider>();
			if (collider2a == nullptr || !collider2a->active || !CollisionFilter::collisionTable[(int)collider1->collisionGroup][(int)collider2a->collisionGroup])
				continue;

			Transform worldTransform1 = GetWorldTransform(entity1);
			Transform worldTransform2 = GetWorldTransform(entity2);

			CollisionResult result = collisionManager->sphere_vs_plane(worldTransform1, collider1, worldTransform2, collider2a);
			if (result.collided) {
				CollisionInfo info = CollisionInfo();
				info.entityId = entity2->GetId();
				info.normal = result.normal;
				info.position = result.position;
				info.distance = result.distance;
				collider1->collisions.push_back(info);

				PhysicBody* physicBody2 = entity2->GetComponentInAncestor<PhysicBody>();
				if (((physicBody1 != nullptr && physicBody1->active) || (physicBody2 != nullptr && physicBody2->active))) {

					if (collider1->isTouchable && collider2a->isTouchable && physicBody1 != nullptr && physicBody1->active) {
						point3d nVel = physicBody1->velocity.normalized();
						if (collider1->softness == 0 && collider2a->softness == 0) {
							transform1->position += result.normal * result.distance;
							physicBody1->velocity = (nVel + result.normal * result.normal.dot(-nVel)) * physicBody1->velocity.magnitude();
						}
						else {
							float softnessTotal = max(0.01f, collider1->softness + collider2a->softness);
							float penetrationRatio = result.distance / (collider1->radius + collider2a->radius);

							float pushStrength = 5.0f / softnessTotal; // Настройте этот коэффициент
							float velocityPush = pushStrength * penetrationRatio;

							// Ограничиваем максимальное изменение скорости
							//velocityPush = min(velocityPush, 10.0f);

							physicBody1->velocity += result.normal * velocityPush;

							// Дополнительно: небольшое позиционное выталкивание для мягкости
							//float positionPush = 0.3f * penetrationRatio / softnessTotal;
							//transform1->position += result.normal * positionPush;
						}
					}

				}
			}

		}

	}
}