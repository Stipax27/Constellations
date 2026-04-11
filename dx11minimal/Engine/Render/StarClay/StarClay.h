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

	Blob(point3d Pos, float Radius, double StartTime)
		: pos(Pos), radius(Radius), startTime(StartTime)
	{};
};


struct StarClay : Component
{
	int vShader = 31;
	int pShader = 31;

	float rate = 10.0f;
	double lifetime = 5000;

	float coreRadius = 0.5f;
	std::pair<float, float> blobsRadius = { 0.25f, 0.5f };

	float out_blobsRate = 5.0f;
	std::pair<float, float> out_blobsRadius = { 0.1f, 0.25f };

	// Technical fields //

	double lastEmitTime = timer::currentTime;
	std::vector<Blob> blobs;

	double out_LastEmitTime = timer::currentTime;
	std::vector<Blob> out_blobs;
};

#endif