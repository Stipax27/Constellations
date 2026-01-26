#ifndef _SPHERECOLLIDER_H_
#define _SPHERECOLLIDER_H_

#include "collider.h"

struct SphereCollider : Collider
{
	float radius = 1.0f;
};

#endif