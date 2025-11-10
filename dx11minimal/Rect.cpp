#ifndef _RECT_C_
#define _RECT_C_

#include "component.h"
#include "point3d.h"

enum ScreenAspectRatio {
	XY,
	XX,
	YY
};

enum CornerType {
	Strict,
	Smooth
};

struct Rect : Component
{
	point3d anchorPoint = point3d();
	ScreenAspectRatio ratio = ScreenAspectRatio::XY;

	point3d color = point3d(1, 1, 1);
	float opacity = 1.0f;

	float cornerRadius = 0.0f;
	CornerType cornerType = CornerType::Strict;

	bool clickable = false;
};

#endif