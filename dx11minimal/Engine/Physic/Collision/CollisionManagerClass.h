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

#include "../../Lib/isingleton.h"
#include "../../Utils/utils.h"

#include "../../BasicComponents/Transform.h"

#include "../../ECS_Base/entityStorage.h"
#include "../../BasicComponents/collider.h"
#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "SurfaceCollider.h"


/////////////
// STRUCTS //
/////////////

struct RayInfo {
	point3d origin;
	point3d direction;
	CollisionFilter::Group collisionGroup;
	bool touchableOnly;

	RayInfo()
	{
		origin = point3d();
		direction = point3d();
		collisionGroup = CollisionFilter::Group::Projectile;
		touchableOnly = false;
	}

	RayInfo(point3d Origin, point3d Direction, CollisionFilter::Group CollisionGroup, bool TouchableOnly)
		: origin(Origin), direction(Direction), collisionGroup(CollisionGroup), touchableOnly(TouchableOnly)
	{}
};

struct RaycastResult {
	bool hit = false;
	float distance = INFINITY;
	point3d position;
	point3d normal;
	SphereCollider* collider;
	Entity* entity;
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


class CollisionManagerClass : public ISingleton
{
public:
	CollisionManagerClass();
	~CollisionManagerClass();
	CollisionManagerClass(const CollisionManagerClass&);

	void Initialize(EntityStorage& entityStorage);
	void Shutdown();

	static CollisionResult sphere_vs_sphere(
		const Transform t1, const SphereCollider* c1,
		const Transform t2, const SphereCollider* c2);

	static CollisionResult sphere_vs_plane(
		const Transform t1, const SphereCollider* c1,
		const Transform t2, const PlaneCollider* c2);

	RaycastResult Raycast(const RayInfo& ray);
	static std::map<TypePair, CollisionFn> collisionMap;

private:
	EntityStorage* entityStorage = nullptr;
	bool raycast_sphere(const RayInfo&, const Transform, const SphereCollider*, RaycastResult&);
};

#endif