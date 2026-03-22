#include "collisionSystem.h"



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

void CollisionSystem::Update(vector<Entity*>& entities, float deltaTime)
{
	size_t size = entities.size();
	for (int i = 0; i < size; i++) {
		Entity* entity1 = entities[i];
		if (IsEntityValid(entity1)) {
			Transform* transform1 = entity1->GetComponentInAncestor<Transform>();
			PhysicBody* physicBody1 = entity1->GetComponentInAncestor<PhysicBody>();
			SphereCollider* collider1 = entity1->GetFirstComponentOfBase<SphereCollider>();
			if (transform1 != nullptr && collider1 != nullptr && collider1->active) {

				collider1->collisions.clear();
				Transform worldTransform1 = GetWorldTransform(entity1);
				size_t size = entities.size();

				for (int i = 0; i < size; i++)
				{
					Entity* entity2 = entities[i];
					if (entity2 != entity1 && IsEntityValid(entity1)) {
						SphereCollider* collider2 = entity2->GetFirstComponentOfBase<SphereCollider>();
						if (collider2 != nullptr && collider2->active && CollisionFilter::collisionTable[(int)collider1->collisionGroup][(int)collider2->collisionGroup]) {

							PhysicBody* physicBody2 = entity2->GetComponentInAncestor<PhysicBody>();
							if (((physicBody1 != nullptr && physicBody1->active) || (physicBody2 != nullptr && physicBody2->active))) {
								/*TypePair key{ std::type_index(typeid(collider1)), std::type_index(typeid(collider2)) };

								auto it = collisionMap.find(key);
								if (it != collisionMap.end()) {
									CollisionResult res = it->second(transform1, collider1, transform2, collider2);
									if (res.collided) {

									}
								}*/

								Transform worldTransform1 = GetWorldTransform(entity1);
								Transform worldTransform2 = GetWorldTransform(entity2);

								CollisionResult result = collisionManager->sphere_vs_sphere(worldTransform1, collider1, worldTransform2, collider2);
								if (result.collided) {
									CollisionInfo info = CollisionInfo();
									info.entityId = entity2->GetId();
									info.normal = result.normal;
									info.position = result.position;
									info.distance = result.distance;
									collider1->collisions.push_back(info);

									//transform1->position += planeCollider->normal * (sphereCollider->radius - distance);

									if (collider1->isTouchable && collider2->isTouchable && physicBody1 != nullptr && physicBody1->active) {
										point3d nVel = physicBody1->velocity.normalized();
										if (collider1->softness == 0 && collider2->softness == 0) {
											transform1->position += result.normal * result.distance;
											physicBody1->velocity = (nVel + result.normal * result.normal.dot(-nVel)) * physicBody1->velocity.magnitude();
										}
										else {
											physicBody1->velocity += result.normal * pow(SPACE_DENSITY, 2) * (result.distance / (collider1->radius + collider2->radius)) * timer::deltaTime / 1000;
										}
									}

									/*float sideVelocity = (physicBody1->velocity - res.normal * res.distance).magnitude();
									if (sideVelocity > 0.0f)
									{
										physicBody1->velocity = (physicBody1->velocity.normalized() + res.normal) * (physicBody1->velocity.magnitude() - ((sideVelocity * collider1->friction + collider1->friction) * deltaTime));
									}*/

									/*PhysicBody* physicBody2 = entity2->GetComponent<PhysicBody>();
									if (physicBody2 != nullptr)
									{
										transform2->position -= planeCollider->normal * (sphereCollider->radius - distance);
									}*/

									pair<Entity*, Health*> hres = entity1->GetAncestorWithComponent<Health>();
									if (hres.first != nullptr && hres.second->active) {

										pair<Entity*, SingleDamager*> sdres = entity2->GetAncestorWithComponent<SingleDamager>();
										if (sdres.first != nullptr && sdres.second->active && sdres.second->target == hres.second->fraction && find(sdres.second->entityFilter.begin(), sdres.second->entityFilter.end(), hres.first->GetId()) == sdres.second->entityFilter.end()) {
											hres.second->hp -= sdres.second->damage;

											sdres.second->entityFilter.push_back(hres.first->GetId());
											if (sdres.second->entityFilter.size() >= sdres.second->maxHitCount) {
												if (sdres.second->destroyable) {
													sdres.first->Destroy();
												}
												sdres.first->RemoveComponent<SingleDamager>();
											}
										}

										pair<Entity*, MultiDamager*> mdres = entity2->GetAncestorWithComponent<MultiDamager>();
										if (mdres.first != nullptr && mdres.second->active && mdres.second->target == hres.second->fraction && timer::currentTime - mdres.second->lastDamageTime >= mdres.second->inverval) {
											hres.second->hp -= mdres.second->damage;

											mdres.second->lastDamageTime = timer::currentTime;
											mdres.second->repeatCount++;

											if (mdres.second->repeats >= 0 && mdres.second->repeatCount >= mdres.second->repeats) {
												if (mdres.second->destroyable) {
													mdres.first->Destroy();
												}
												mdres.first->RemoveComponent<MultiDamager>();
											}
										}

										/*MultiDamager* multiDamager = entity2->GetComponent<MultiDamager>();
										if (multiDamager != nullptr) {
											health->hp -= multiDamager->damage;
										}*/

									}
								}
							}

						}
					}
				}

			}
		}
	}
}