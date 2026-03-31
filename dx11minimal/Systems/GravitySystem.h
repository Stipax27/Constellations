#ifndef _GRAVITY_SYSTEM_H_
#define _GRAVITY_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "../system.h"

#include "../Transform.h"
#include "../PhysicBody.h"
#include "../Components/GravityPoint.h"


class GravitySystem : public System
{
public:
	GravitySystem();
	void Initialize();
	void Shutdown();

	void Update(vector<Entity*>&, float);
};

#endif