#ifndef _MESH_H_
#define _MESH_H_

#include "component.h"
#include "dx11.h"


struct Mesh : Component
{
	Rasterizer::cullmode cullMode = Rasterizer::cullmode::front;
	int index = 0;
};

#endif