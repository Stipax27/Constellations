#ifndef _PHYSICBODY_H_
#define _PHYSICBODY_H_

#include "component.h"
#include "point3d.h"
#include <DirectXMath.h>

struct PhysicBody : Component
{
	float airFriction = 1.0f;
	float mass = 1.0f;

	point3d velocity = point3d();
	point3d acceleration = point3d();

	DirectX::XMMATRIX mAngVelocity = DirectX::XMMatrixIdentity();

	bool useGravity = true;
	bool preciseMovement = false;
};

#endif