#ifndef _SPRITECLUSTER_C_
#define _SPRITECLUSTER_C_

#include "component.h"
#include "renderCompress.h"
#include "dx11.h"


struct SpriteCluster : Component
{
	int vShader = 0;
	int gShader = 0;
	int pShader = 0;

	InputAssembler::topology topology = InputAssembler::topology::triList;

	int pointsNum = 1;
	int vertexNum = 6;

	RenderCompress compress = RenderCompress::none;
};

#endif