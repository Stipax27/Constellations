#ifndef _COLLISION_SYSTEM_
#define _COLLISION_SYSTEM_

//////////////
// INCLUDES //
//////////////
#include <map>
#include <unordered_map>
#include <typeindex>
#include <functional>

#include<cmath>
#include "system.h"
#include "Transform.h"
#include "PhysicBody.h"

#include "collider.h"
#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "SurfaceCollider.h"

#include "Health.h"
#include "SingleDamageGiver.h"
#include "MultiDamageGiver.h"

#include "MethodOfClosest.h"
#include "CollisionManagerClass.h"


class CollisionSystem : public System
{
public:
	CollisionSystem(CollisionManagerClass* CollisionManager)
	{
		collisionManager = CollisionManager;
	}


	void Initialize()
	{
	}


	void Shutdown()
	{
		if (collisionManager) {
			collisionManager = 0;
		}
	}

	float f(float x) {
		return 0.35*pow(max(x*(1-0.125*x),0), 2);
	}

	void Update(vector<Entity*>& entities, float deltaTime)
	{
		size_t size = entities.size();
		for (int i = 0; i < size; i++) {
			Entity* entity1 = entities[i];
			if (IsEntityValid(entity1)) {
				Transform* transform1 = entity1->GetComponentInAncestor<Transform>();
				PhysicBody* physicBody1 = entity1->GetComponentInAncestor<PhysicBody>();
				SphereCollider* collider1 = entity1->GetFirstComponentOfBase<SphereCollider>();
				if (transform1 != nullptr && collider1 != nullptr && collider1->active) {

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

										Health* health = entity1->GetComponent<Health>();
										if (health != nullptr && health->active) {

											pair<Entity*, SingleDamageGiver*> res = entity2->GetAncestorWithComponent<SingleDamageGiver>();
											if (res.first != nullptr && res.second->active && res.second->target == health->fraction && find(res.second->entityFilter.begin(), res.second->entityFilter.end(), entity1->GetId()) == res.second->entityFilter.end()) {
												health->hp -= res.second->damage;

												res.second->entityFilter.push_back(entity1->GetId());
												if (res.second->entityFilter.size() >= res.second->maxHitCount) {
													if (res.second->destroyable) {
														res.first->Destroy();
													}
													res.first->RemoveComponent<SingleDamageGiver>();
												}
											}

											/*MultiDamageGiver* multiDamageGiver = entity2->GetComponent<MultiDamageGiver>();
											if (multiDamageGiver != nullptr) {
												health->hp -= multiDamageGiver->damage;
											}*/

										}
									}
								}

							}


							//PlaneCollider* planeCollider = entity2->GetComponent<PlaneCollider>();
							//if (transform2 != nullptr && planeCollider != nullptr)
							//{
							//	float distance = (transform1->position - transform2->position).dot(planeCollider->normal);
							//	if (distance < sphereCollider->radius)
							//	{
							//		PhysicBody* physicBody1 = entity1->GetComponent<PhysicBody>();
							//		if (physicBody1 != nullptr)
							//		{
							//			//point3d buoyantForce = -(physicBody1->velocity * planeCollider->normal / pow(planeCollider->normal.magnitude(), 2) * planeCollider->normal);
							//			//physicBody1->velocity += buoyantForce;

							//			transform1->position += planeCollider->normal * (sphereCollider->radius - distance);

							//			/*float sideVelocity = (physicBody1->velocity - planeCollider->normal * buoyantForce).magnitude();
							//			if (sideVelocity > 0.0f)
							//			{
							//				physicBody1->velocity = physicBody1->velocity.normalized() * (physicBody1->velocity.magnitude() - ((sideVelocity * sphereCollider->friction + sphereCollider->friction) * deltaTime));
							//			}*/
							//		}

							//		/*PhysicBody* physicBody2 = entity2->GetComponent<PhysicBody>();
							//		if (physicBody2 != nullptr)
							//		{
							//			transform2->position -= planeCollider->normal * (sphereCollider->radius - distance);
							//		}*/
							//	}
							//}

							////Surface
							//SurfaceCollider* surfaceCollider = entity2->GetComponent<SurfaceCollider>();
							//if (surfaceCollider != nullptr) {
							//	point3d playerPos = transform1->position;
							//	if ((playerPos - surfaceCollider->basePos).magnitude() < 64 * transform2->scale.x) { // 64.5 ? location radius
							//		float distance;
							//		point3d closestPoint;

							//		bool collision = findClosestPointOnSurface(playerPos, *surfaceCollider, closestPoint, distance, 12, 3);

							//		point3d buoyantForce = point3d(0, 0, 0);

							//		if (collision && (closestPoint - playerPos).magnitude() < 8) {
							//			buoyantForce = (closestPoint - playerPos).normalized() *
							//				max(0,
							//					f((closestPoint - playerPos).dot(surfaceCollider->getNormal(closestPoint.x, closestPoint.z)))
							//				);
							//		}

							//		PhysicBody* physicBody1 = entity1->GetComponent<PhysicBody>();
							//		physicBody1->velocity += buoyantForce;
							//	}
							//}
						}
					}

				}
			}
		}
	}

private:
	CollisionManagerClass* collisionManager;
};

#endif