#ifndef _PLANECOLLIDER_C_
#define _PLANECOLLIDER_C_

#include "collider.h"
#include "point3d.h"

struct PlaneCollider : Collider
{
	point3d normal = point3d(0, 1, 0);
	float radius = 1.0f;

	float gravitySpeed = 9.8f;
	float gravityDistance = 50.0f;
};

#endif