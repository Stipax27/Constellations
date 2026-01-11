#ifndef _STAR_C_
#define _STAR_C_

#include "component.h"
#include "Point3d.h"


struct Star : Component
{
	float radius = 0.5f;
	float crownRadius = 0.9f;

	point3d color1 = point3d(1, 0.95, 0.25);
	point3d color2 = point3d(0.75, 0.015, 0);
	point3d crownColor = point3d(1, 0.95, 0.3);
};

#endif