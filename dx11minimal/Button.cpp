#ifndef _BUTTON_C_
#define _BUTTON_C_

#include "component.h"
#include "point3d.h"

struct Button : Component
{
	point3d anchorPoint = point3d();
	point3d color = point3d();
	bool screenAspect = true;
};

#endif