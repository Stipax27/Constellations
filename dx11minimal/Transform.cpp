#ifndef _TRANSFORM_C_
#define _TRANSFORM_C_

#include "component.h"
#include "point3d.h"


struct Transform : Component
{
	point3d position = point3d();
	point3d rotation = point3d();
	point3d scale = point3d(1.0f, 1.0f, 1.0f);

	point3d GetLookVector()
	{
		XMMATRIX pitchMatrix = XMMatrixSet(
			1, 0, 0, 0,
			0, cos(rotation.x), -sin(rotation.x), 0,
			0, sin(rotation.x), cos(rotation.x), 0,
			0, 0, 0, 0
		);

		XMMATRIX yawMatrix = XMMATRIX{
			cos(rotation.y), 0, sin(rotation.y), 0,
			0, 1, 0, 0,
			-sin(rotation.y), 0, cos(rotation.y), 0,
			0, 0, 0, 0
		};

		XMMATRIX rollMatrix = XMMATRIX{
			cos(rotation.z), -sin(rotation.z), 0, 0,
			sin(rotation.z), cos(rotation.z), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 0
		};

		XMMATRIX rotationMatrix = yawMatrix * pitchMatrix * rollMatrix;
	}
};

#endif