#include "CollisionManagerClass.h"


CollisionManagerClass::CollisionManagerClass(const CollisionManagerClass& other)
{
}


CollisionManagerClass::CollisionManagerClass()
{
}


CollisionManagerClass::~CollisionManagerClass()
{
}


void CollisionManagerClass::Initialize()
{
	//collisionMap[{typeid(SphereCollider), typeid(SphereCollider)}] = sphere_vs_sphere;
}


void CollisionManagerClass::Shutdown()
{
}


CollisionResult CollisionManagerClass::sphere_vs_sphere(
	const Transform t1, const SphereCollider* c1,
	const Transform t2, const SphereCollider* c2)
{
	CollisionResult result = CollisionResult();

	point3d vector = t1.position - t2.position;
	float magnitude = vector.magnitude();

	if (magnitude < c1->radius + c2->radius) {
		result.collided = true;
		result.normal = vector.normalized();
		result.distance = (c1->radius + c2->radius) - magnitude;
	}

	return result;
}