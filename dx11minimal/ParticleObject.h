#ifndef _PARTICLE_OBJECT_H_
#define _PARTICLE_OBJECT_H_

#include "Point3d.h"
#include <vector>

using namespace std;


struct Particle
{
	float startTime;
	point3d pos;

	Particle(point3d Pos, float CurTime)
		: pos(Pos), startTime(CurTime)
	{}
};


struct ParticleObject
{
	bool active = false;

	int rate = 1;
	float lifetimeMin = 1.0f;
	float lifetimeMax = 1.0f;

	point3d position = point3d();
	float lastEmit = 0;
	vector<Particle*> particles;
};

#endif