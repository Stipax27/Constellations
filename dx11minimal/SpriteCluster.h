#ifndef _SPRITECLUSTER_H_
#define _SPRITECLUSTER_H_

#include "Engine/ECS_Base/component.h"
#include "Engine/Render/renderCompress.h"
#include "Engine/dx11.h"


struct SpriteCluster : Component
{
	int vShader = 0;
	int gShader = 0;
	int pShader = 0;

	InputAssembler::topology topology = InputAssembler::topology::triList;

	int pointsNum = 1;
	int vertexNum = 6;

	float frustumRadius = 10.0f;

	RenderCompress compress = RenderCompress::none;
};

#endif