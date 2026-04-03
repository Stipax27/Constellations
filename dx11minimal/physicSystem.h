#ifndef _PHYSIC_SYSTEM_H_
#define _PHYSIC_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Engine/BasicComponents/Transform.h"
#include "PhysicBody.h"


class PhysicSystem : public System
{
public:
	PhysicSystem();
	void Initialize();
	void Shutdown();

	void Update(EntityStorage&, float);
};

#endif