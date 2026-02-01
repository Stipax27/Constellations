#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "component.h"
#include "cornerType.h"

struct Button : Component
{
	bool isClicked = false;

	point3d color = point3d(1, 1, 1);
	point3d clickColor = point3d(1, 0, 0);
	float opacity = 1.0f;

	float cornerRadius = 0.0f;
	CornerType cornerType = CornerType::Strict;

	int vShader = 13;
	int pShader = 13;
};

#endif