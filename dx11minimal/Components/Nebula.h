#ifndef _NEBULA_H_
#define _NEBULA_H_

#include "../component.h"
#include "../renderCompress.h"
#include "../dx11.h"
#include "../pmode.h"


struct Nebula : Component
{
	int vShader = 0;
	int gShader = 0;

	InputAssembler::topology topology = InputAssembler::topology::triList;

	int count = 1;
	int skipper = 1;
	pMode mode = pMode::point;

	float frustumRadius = 10.0f;

	RenderCompress compress = RenderCompress::none;
};

#endif