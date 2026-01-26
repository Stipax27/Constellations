#ifndef _RECT_H_
#define _RECT_H_

#include "component.h"
#include "cornerType.h"


struct Rect : Component
{
	float opacity = 1.0f;
	point3d color = point3d(1, 1, 1);

	float cornerRadius = 0.0f;
	CornerType cornerType = CornerType::Strict;
};

#endif