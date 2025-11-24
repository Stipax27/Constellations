#ifndef _PROJECTILE_C_
#define _PROJECTILE_C_

#include "component.h"
#include "point3d.h"
#include <DirectXMath.h>

struct Projectile : Component
{
	float speed = 10.0f;
	point3d velocity = point3d();
};

#endif