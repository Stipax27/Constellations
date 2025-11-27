#ifndef _PROJECTILE_C_
#define _PROJECTILE_C_

#include "component.h"
#include "point3d.h"
#include <DirectXMath.h>

struct Projectile : Component
{
	float speed = 10.0f;
    float time = 0.0f;

    bool spiralMovement = false;
    float spiralRadius = 0.0f;
    float spiralSpeed = 0.0f;
    point3d startPosition;
    point3d direction = point3d(0, 0, 1);
    point3d spiralVector1 = point3d(1, 0, 0);
    point3d spiralVector2 = point3d(0, 1, 0); 
};

#endif