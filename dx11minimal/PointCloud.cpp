#ifndef _POINT_CLOUD_C_
#define _POINT_CLOUD_C_

#include "component.h"
#include "dx11.h"


struct PointCloud : Component
{
	point3d position = point3d();
	point3d scale = point3d(1.0f, 1.0f, 1.0f);
	DirectX::XMMATRIX mRotation = DirectX::XMMatrixIdentity();

	float pointSize = 1.0f;
	float brightness = 1.0f;

	int index = 0;
};

#endif