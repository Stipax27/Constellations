#ifndef _PHYSICBODY_C_
#define _PHYSICBODY_C_

#include "component.h"
#include "point3d.h"
#include <DirectXMath.h>

struct PhysicBody : Component
{
	float airFriction = 1.0f;
	point3d velocity = point3d();

	DirectX::XMMATRIX mAngVelocity = DirectX::XMMATRIX{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	bool useGravity = true;
};

#endif