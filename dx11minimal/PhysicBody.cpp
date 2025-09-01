#ifndef _PHYSICBODY_C_
#define _PHYSICBODY_C_

#include "component.h"
#include "point3d.h"

struct PhysicBody : Component
{
	float mass = 1.0f;
	point3d velocity = point3d();
	point3d angVelocity = point3d();
	bool useGravity = true;
};

#endif