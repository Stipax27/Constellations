#ifndef _COLLISION_SYSTEM_
#define _COLLISION_SYSTEM_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.cpp"
#include "PhysicBody.cpp"

#include "SphereCollider.cpp"
#include "PlaneCollider.cpp"
#include "SurfaceCollider.cpp"

#include "MethodOfClosest.h"

class CollisionSystem : public System
{
public:
	CollisionSystem()
	{
	}



	void Initialize()
	{
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
			if (entity1->IsActive()) {
				Transform* transform1 = entity1->GetComponent<Transform>();
				PhysicBody* physicBody1 = entity1->GetComponent<PhysicBody>();
				SphereCollider* sphereCollider = entity1->GetComponent<SphereCollider>();
				if (transform1 != nullptr && physicBody1 != nullptr && sphereCollider != nullptr) {

				size_t size = entities.size();
				for (int i = 0; i < size; i++)
				{
					Entity* entity2 = entities[i];
					Transform* transform2 = entity2->GetComponent<Transform>();
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
						if ((playerPos-surfaceCollider->basePos).magnitude() < 64) { // 64.5 – location radius
							float distance;
							point3d closestPoint;

							bool collision = findClosestPointOnSurface(playerPos, *surfaceCollider, closestPoint, distance, 12, 3);

							point3d buoyantForce = point3d(0, 0, 0);

							if (collision && (closestPoint - playerPos).magnitude()<8) {
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
};

#endif