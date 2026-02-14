#ifndef _MESH_H_
#define _MESH_H_

#include "component.h"
#include "dx11.h"


struct Mesh : Component
{
	point3d position = point3d();
	point3d scale = point3d(1.0f, 1.0f, 1.0f);
	DirectX::XMMATRIX mRotation = DirectX::XMMatrixIdentity();

	Rasterizer::cullmode cullMode = Rasterizer::cullmode::front;
	int index = 0;

	int vShader = 15;
	int pShader = 15;
};

#endif