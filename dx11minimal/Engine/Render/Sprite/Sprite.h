#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "../../ECS_Base/component.h"


struct Sprite : Component
{
	std::string textureName = "";

	float opacity = 1.0f;
	point3d color = point3d(1, 1, 1);

	int vShader = 32;
	int pShader = 28;
};

#endif