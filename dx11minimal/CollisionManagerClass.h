////////////////////////////////////////////////////////////////////////////////
// Filename: CollisionManagerClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _COLLISIONMANAGERCLASS_H_
#define _COLLISIONMANAGERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <map>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <memory>
#include <mutex>

#include "Transform.cpp"

#include "collider.h"
#include "SphereCollider.cpp"
#include "PlaneCollider.cpp"
#include "SurfaceCollider.cpp"


/////////////
// STRUCTS //
/////////////

struct CollisionResult {
	bool collided = false;
	point3d normal;
	float distance;
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


////////////////////////////////////////////////////////////////////////////////
// Class name: CollisionManagerClass
////////////////////////////////////////////////////////////////////////////////


class CollisionManagerClass
{
public:
	CollisionManagerClass();
	~CollisionManagerClass();
	CollisionManagerClass(const CollisionManagerClass&);

	void Initialize();
	void Shutdown();

	static CollisionResult sphere_vs_sphere(
		const Transform t1, const SphereCollider* c1,
		const Transform t2, const SphereCollider* c2);

private:
	static std::map<TypePair, CollisionFn> collisionMap;
};

#endif