#ifndef _DEBUG_GRAVITY_DRAW_SYSTEM_H_
#define _DEBUG_GRAVITY_DRAW_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "../ECS_Base/system.h"
#include "../BasicComponents/Transform.h"

#include "../Physic/Gravitation/GravityPoint.h"

/////////////
// GLOBALS //
/////////////
//const bool DEPTH_OFF = true;


class GravityDrawSystem : public System
{
public:
	GravityDrawSystem();
	void Initialize();
	void Shutdown();

	void Update(EntityStorage&, float);
};

#endif