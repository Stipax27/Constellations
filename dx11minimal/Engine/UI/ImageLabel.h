#ifndef _IMAGE_LABEL_H_
#define _IMAGE_LABEL_H_

#include "../ECS_Base/component.h"
#include "../UI/cornerType.h"
#include "../Types/Point3d.h"


struct ImageLabel : Component
{
	std::string textureName = "";

	float opacity = 1.0f;
	point3d color = point3d(1, 1, 1);

	int vShader = 13;
	int pShader = 28;
};

#endif