////////////////////////////////////////////////////////////////////////////////
// Filename: collider.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _COLLIDER_H_
#define _COLLIDER_H_

//////////////
// INCLUDES //
//////////////
#include <vector>
#include "component.h"
#include "CollisionGroup.h"
#include "Point3d.h"

/////////////
// STRUCTS //
/////////////

struct CollisionResult {
	bool collided = false;
	point3d normal = point3d(0, 1, 0);
	point3d position = point3d();
	float distance = 0;
};

struct CollisionInfo {
	int entityId;
	point3d normal;
	point3d position;
	float distance;
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

	vector<CollisionInfo> collisions;
};

#endif