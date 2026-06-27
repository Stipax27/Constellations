////////////////////////////////////////////////////////////////////////////////
// Filename: collider.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _COLLIDER_H_
#define _COLLIDER_H_

//////////////
// INCLUDES //
//////////////
#include <vector>
#include "../ECS_Base/component.h"
#include "../Physic/Collision/CollisionGroup.h"
#include "../Types/Point3d.h"

/////////////
// STRUCTS //
/////////////

struct CollisionResult {
	point3d position = point3d();
	point3d normal = point3d(0, 1, 0);
	float distance = 0;
	bool collided = false;
};

struct CollisionInfo {
	point3d position = point3d();
	point3d normal = point3d(0, 1, 0);
	float distance = 0;
	int entityId = -1;
};

////////////////////////////////////////////////////////////////////////////////
// Struct name: Collider
////////////////////////////////////////////////////////////////////////////////
struct Collider : Component
{
	float friction = 1.0f;
	float softness = 0.0f;
	bool isTouchable = true;
	bool anti = false;

	CollisionFilter::Group collisionGroup = CollisionFilter::Group::Projectile;

	std::vector<CollisionInfo> collisions;
};

#endif