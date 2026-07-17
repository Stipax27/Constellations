#ifndef _SPHERECOLLIDER_H_
#define _SPHERECOLLIDER_H_

#include "../../BasicComponents/collider.h"

struct SphereCollider : Collider
{
	float radius = 1.0f;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SphereCollider,
	active,
	friction,
	softness,
	isTouchable,
	anti,
	collisionGroup,
	collisions,
	radius)

#endif