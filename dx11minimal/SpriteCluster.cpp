#ifndef _SPRITECLUSTER_C_
#define _SPRITECLUSTER_C_

#include "component.h"
#include "renderCompress.h"


struct SpriteCluster : Component
{
	int vShader = 0;
	int pShader = 0;

	int pointsNum = 1;
	RenderCompress compress = RenderCompress::none;
};

#endif