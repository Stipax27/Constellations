#ifndef _PHYSICBODY_C_
#define _PHYSICBODY_C_

#include "component.h"
#include "point3d.h"
#include <DirectXMath.h>

struct PhysicBody : Component
{
	float mass = 1.0f;
	point3d velocity = point3d();
	DirectX::XMVECTOR qAngVelocity = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 0);

	DirectX::XMMATRIX mAngVelocity = DirectX::XMMATRIX{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	bool useGravity = true;
};

#endif