#ifndef _PLANECOLLIDER_H_
#define _PLANECOLLIDER_H_

#include "../../BasicComponents/collider.h"
#include "../../Types/point3d.h"

struct PlaneCollider : Collider
{
	point3d normal = point3d(0, 1, 0);
	float radius = 1.0f;
};

#endif