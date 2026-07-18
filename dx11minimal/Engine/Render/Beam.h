#ifndef _BEAM_H_
#define _BEAM_H_

#include "../ECS_Base/component.h"
#include "../Types/Point3d.h"
#include <vector>


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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Beam,
	active,
	point1,
	point2,
	size1,
	size2,
	color1,
	color2,
	opacity1,
	opacity2,
	pShader)

#endif