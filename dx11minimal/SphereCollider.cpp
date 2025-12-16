#ifndef _SPHERECOLLIDER_C_
#define _SPHERECOLLIDER_C_

#include "collider.h"

struct SphereCollider : Collider
{
	float radius = 1.0f;
};

#endif