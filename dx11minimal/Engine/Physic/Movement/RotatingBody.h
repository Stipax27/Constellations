#ifndef _ROTATING_BODY_H_
#define _ROTATING_BODY_H_

#include "../../ECS_Base/component.h"
#include "../../Types/Point3d.h"
#include <DirectXMath.h>

struct RotatingBody : Component
{
	point3d axis = point3d(0.0f, 1.0f, 0.0f);
	float rotateSpeed = 25.0f;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RotatingBody,
	active,
	axis,
	rotateSpeed)

#endif