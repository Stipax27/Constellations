#ifndef _RECT_C_
#define _RECT_C_

#include "component.h"
#include "point3d.h"

enum ScreenAspectRatio {
	XY,
	XX,
	YY
};

struct Rect : Component
{
	point3d anchorPoint = point3d();
	ScreenAspectRatio ratio = ScreenAspectRatio::XY;

	point3d color = point3d(1, 1, 1);
	float opacity = 1.0f;
	float corner = 0.0f;
};

#endif