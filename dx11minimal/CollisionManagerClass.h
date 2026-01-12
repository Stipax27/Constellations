////////////////////////////////////////////////////////////////////////////////
// Filename: CollisionManagerClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _COLLISIONMANAGERCLASS_H
#define _COLLISIONMANAGERCLASS_H


//////////////
// INCLUDES //
//////////////
#include <map>
#include <unordered_map>
#include <typeindex>
#include <functional>

#include "Transform.cpp"

#include "collider.h"
#include "SphereCollider.cpp"
#include "PlaneCollider.cpp"
#include "SurfaceCollider.cpp"


/////////////
// STRUCTS //
/////////////

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


////////////////////////////////////////////////////////////////////////////////
// Class name: CollisionManagerClass
////////////////////////////////////////////////////////////////////////////////


class CollisionManagerClass
{
public:
	CollisionManagerClass();
	CollisionManagerClass(const CollisionManagerClass&);
	~CollisionManagerClass();

	void Initialize();
	void Shutdown();

private:
	static std::map<TypePair, CollisionFn> collisionMap;
};

#endif