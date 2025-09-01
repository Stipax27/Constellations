#ifndef _TRANSFORM_C_
#define _TRANSFORM_C_

#include "component.h"
#include "point3d.h"


struct Transform : Component
{
	point3d position = point3d();
	point3d rotation = point3d();
	point3d scale = point3d(1.0f, 1.0f, 1.0f);
};

#endif