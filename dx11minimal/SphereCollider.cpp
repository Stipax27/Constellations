#ifndef _SPHERECOLLIDER_C_
#define _SPHERECOLLIDER_C_

#include "component.h"

struct SphereCollider : Component
{
	float friction = 1.0f;
	float radius = 1.0f;

	bool isTouchable = true;
};

#endif