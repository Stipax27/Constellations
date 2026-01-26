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

#include "utils.h"

#include "Transform.h"

#include "entityStorage.h"
#include "collider.h"
#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "SurfaceCollider.h"


/////////////
// STRUCTS //
/////////////

struct RayInfo {
	point3d origin;
	point3d direction;

	RayInfo()
	{
		origin = point3d();
		direction = point3d();
	}

	RayInfo(point3d Origin, point3d Direction) : origin(Origin), direction(Direction) {}
};

struct RaycastResult {
	bool hit = false;
	float distance = INFINITY;
	point3d position;
	point3d normal;
	SphereCollider* collider;
	Entity* entity;
};

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

	void Initialize(EntityStorage*);
	void Shutdown();

	static CollisionResult sphere_vs_sphere(
		const Transform t1, const SphereCollider* c1,
		const Transform t2, const SphereCollider* c2);

	RaycastResult Raycast(const RayInfo& ray);

private:
	EntityStorage* entityStorage;
	static std::map<TypePair, CollisionFn> collisionMap;

private:
	bool raycast_sphere(const RayInfo&, const Transform, const SphereCollider*, RaycastResult&);
};

#endif