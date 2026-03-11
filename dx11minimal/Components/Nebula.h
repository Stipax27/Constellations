#ifndef _NEBULA_H_
#define _NEBULA_H_

#include "../component.h"
#include "../renderCompress.h"
#include "../dx11.h"
#include "../pmode.h"
#include "../Point3d.h"


struct Nebula : Component
{
	int vShader = 0;
	int gShader = 0;

	InputAssembler::topology topology = InputAssembler::topology::triList;

	int count = 1;
	int skipper = 1;
	pMode mode = pMode::point;
	point3d color = point3d(1, 1, 1);
	float scale = 1;

	float frustumRadius = 10.0f;

	RenderCompress compress = RenderCompress::none;
};

#endif