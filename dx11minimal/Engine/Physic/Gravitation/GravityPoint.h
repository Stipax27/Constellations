#ifndef _GRAVITY_POINT_H_
#define _GRAVITY_POINT_H_

#include "../../ECS_Base/component.h"

struct GravityPoint : Component
{
	float radius = 10.0f;
	float mass = 1.0f;
};

#endif