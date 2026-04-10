#ifndef _STARCLAY_H_
#define _STARCLAY_H_

#include "../../ECS_Base/component.h"
#include "../../Types/Point3d.h"
#include <vector>

struct StarClay : Component
{
	int vShader = 18;
	int pShader = 18;

	std::vector<point3d> blobs;
};

#endif