#ifndef _RECT_C_
#define _RECT_C_

#include "component.h"
#include "point3d.h"

struct Rect : Component
{
	point3d anchorPoint = point3d();
	point3d color = point3d();
};

#endif