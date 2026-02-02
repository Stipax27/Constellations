#ifndef _PARTICLE_EMITTER_H_
#define _PARTICLE_EMITTER_H_

#include "component.h"
#include "renderCompress.h"
#include "dx11.h"


struct ParticleEmitter : Component
{
	int vShader = 21;
	int gShader = 0;
	int pShader = 21;

	float size = 1.0f;
	float brightness = 1.0f;
	point3d color = point3d(1.0f, 1.0f, 1.0f);
	DWORD lifetime = 1000;
	float rate = 10.0f;

	RenderCompress compress = RenderCompress::none;

	DWORD lastEmitTime;
	vector<DWORD> particles;
};

#endif