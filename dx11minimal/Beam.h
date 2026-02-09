#ifndef _BEAM_H_
#define _BEAM_H_

#include "component.h"
#include "point3d.h"
#include <vector>
#include <utility>


struct Beam : Component
{
	point3d point1 = point3d();
	point3d point2 = point3d(0, 1, 0);

	float size1 = 0.15f;
	float size2 = 0.15f;

	point3d color1 = point3d(1, 1, 1);
	point3d color2 = point3d(1, 1, 1);

	float opacity1 = 1.0f;
	float opacity2 = 1.0f;

	int pShader = 4;
};

#endif