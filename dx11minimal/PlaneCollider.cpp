#ifndef _PLANECOLLIDER_C_
#define _PLANECOLLIDER_C_

#include "component.h"
#include "point3d.h"

struct PlaneCollider : Component
{
	float friction = 1.0f;
	point3d normal = point3d();
	float radius = 1.0f;

	bool isTouchable = true;
};

#endif