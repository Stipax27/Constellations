#ifndef _POINT_CLOUD_H_
#define _POINT_CLOUD_H_

#include "component.h"
#include "renderCompress.h"
#include "dx11.h"


struct PointCloud : Component
{
	point3d position = point3d();
	point3d scale = point3d(1.0f, 1.0f, 1.0f);
	DirectX::XMMATRIX mRotation = DirectX::XMMatrixIdentity();

	int vShader = 17;
	int gShader = 17;
	int pShader = 17;

	float pointSize = 1.0f;
	float brightness = 1.0f;
	point3d color = point3d(1.0f, 1.0f, 1.0f);

	int index = 0;
	int instances = 1;

	float frustumRadius = 10.0f;

	RenderCompress compress = RenderCompress::none;
};

#endif