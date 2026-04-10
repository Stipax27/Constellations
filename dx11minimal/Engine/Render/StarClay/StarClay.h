#ifndef _STARCLAY_H_
#define _STARCLAY_H_

#include "../../ECS_Base/component.h"
#include "../../Types/Point3d.h"
#include "../../Lib/timer.h"

#include <vector>

struct Blob {
	point3d pos = point3d();
	float radius = 1.0f;

	double startTime = timer::currentTime;

	Blob(point3d Pos, float Radius)
		: pos(Pos), radius(Radius)
	{};
};

struct StarClay : Component
{
	int vShader = 31;
	int pShader = 31;

	std::vector<Blob> blobs;
};

#endif