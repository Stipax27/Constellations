#ifndef _PHYSICBODY_C_
#define _PHYSICBODY_C_

#include "component.h"
#include "point3d.h"

struct PhysicBody : Component
{
	float mass = 1.0f;
	point3d velocity = point3d();
	XMVECTOR qAngVelocity = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 0);
	bool useGravity = true;
};

#endif