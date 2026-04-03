#ifndef _GRAVITY_SYSTEM_H_
#define _GRAVITY_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "../../../system.h"

#include "../../BasicComponents/Transform.h"
#include "../../../PhysicBody.h"
#include "GravityPoint.h"

/////////////
// GLOBALS //
/////////////
#define GRAVITY_CONSTANT 100
#define GRAVITATION_FADE_START 0.75f


class GravitySystem : public System
{
public:
	GravitySystem();
	void Initialize();
	void Shutdown();

	void Update(EntityStorage&, float);

private:
	float CalcFalloff(float radius, float maxRadius);
};

#endif