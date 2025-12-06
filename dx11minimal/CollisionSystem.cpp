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
#include "Transform.cpp"
#include "PhysicBody.cpp"

#include "SphereCollider.cpp"
#include "PlaneCollider.cpp"
#include "SurfaceCollider.cpp"

#include "MethodOfClosest.h"


struct CollisionResult {
	bool collided;
	point3d normal;

	CollisionResult(bool Collided = false)
		: collided(Collided)
	{
	}
};

using CollisionFn = CollisionResult(*)(
	const Transform*, const Component*,
	const Transform*, const Component*
	);

struct TypePair {
	std::type_index a, b;
	bool operator<(const TypePair& other) const {
		if (a != other.a) return a < other.a;
		return b < other.b;
	}
};

static std::map<TypePair, CollisionFn> collisionMap;

static CollisionResult sphere_vs_sphere(
	const Transform* t1, const Component* c1,
	const Transform* t2, const Component* c2)
{
	CollisionResult result = CollisionResult();

	const auto* a = static_cast<const SphereCollider*>(c1);
	const auto* b = static_cast<const SphereCollider*>(c2);

	point3d vector = t1->position - t2->position;
	if (vector.magnitude() < a->radius + b->radius) {
		result.collided = true;
		result.normal = vector.normalized();
	}

	return result;
}


class CollisionSystem : public System
{
public:
	CollisionSystem()
	{
	}


	void Initialize()
	{
		collisionMap[{typeid(SphereCollider), typeid(SphereCollider)}] = sphere_vs_sphere;
	}


	void Shutdown()
	{
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
				Transform* transform1 = entity1->GetComponent<Transform>();
				PhysicBody* physicBody1 = entity1->GetComponent<PhysicBody>();
				SphereCollider* sphereCollider = entity1->GetComponent<SphereCollider>();
				if (transform1 != nullptr && physicBody1 != nullptr && sphereCollider != nullptr) {

				size_t size = entities.size();
				for (int i = 0; i < size; i++)
				{
					Entity* entity2 = entities[i];
					if (IsEntityValid(entity1)) {
						Transform* transform2 = entity2->GetComponent<Transform>();


						/*TypePair key{ std::type_index(typeid(colliderA)), std::type_index(typeid(colliderB)) };

						auto it = g_collisionMap.find(key);
						if (it != g_collisionMap.end()) {
							CollisionResult res = it->second(tA, colliderA, tB, colliderB);
							if (res.collided) {
								
							}
						}*/


						PlaneCollider* planeCollider = entity2->GetComponent<PlaneCollider>();
						if (transform2 != nullptr && planeCollider != nullptr)
						{
							float distance = (transform1->position - transform2->position).dot(planeCollider->normal);
							if (distance < sphereCollider->radius)
							{
								PhysicBody* physicBody1 = entity1->GetComponent<PhysicBody>();
								if (physicBody1 != nullptr)
								{
									//point3d buoyantForce = -(physicBody1->velocity * planeCollider->normal / pow(planeCollider->normal.magnitude(), 2) * planeCollider->normal);
									//physicBody1->velocity += buoyantForce;

									transform1->position += planeCollider->normal * (sphereCollider->radius - distance);

									/*float sideVelocity = (physicBody1->velocity - planeCollider->normal * buoyantForce).magnitude();
									if (sideVelocity > 0.0f)
									{
										physicBody1->velocity = physicBody1->velocity.normalized() * (physicBody1->velocity.magnitude() - ((sideVelocity * sphereCollider->friction + sphereCollider->friction) * deltaTime));
									}*/
								}

								/*PhysicBody* physicBody2 = entity2->GetComponent<PhysicBody>();
								if (physicBody2 != nullptr)
								{
									transform2->position -= planeCollider->normal * (sphereCollider->radius - distance);
								}*/
							}
						}

						//Surface
						SurfaceCollider* surfaceCollider = entity2->GetComponent<SurfaceCollider>();
						if (surfaceCollider != nullptr) {
							point3d playerPos = transform1->position;
							if ((playerPos - surfaceCollider->basePos).magnitude() < 64 * transform2->scale.x) { // 64.5 ? location radius
								float distance;
								point3d closestPoint;

								bool collision = findClosestPointOnSurface(playerPos, *surfaceCollider, closestPoint, distance, 12, 3);

								point3d buoyantForce = point3d(0, 0, 0);

								if (collision && (closestPoint - playerPos).magnitude() < 8) {
									buoyantForce = (closestPoint - playerPos).normalized() *
										max(0,
											f((closestPoint - playerPos).dot(surfaceCollider->getNormal(closestPoint.x, closestPoint.z)))
										);
								}

								PhysicBody* physicBody1 = entity1->GetComponent<PhysicBody>();
								physicBody1->velocity += buoyantForce;
							}
						}
					}
				}

				}
			}
		}
	}
};

#endif