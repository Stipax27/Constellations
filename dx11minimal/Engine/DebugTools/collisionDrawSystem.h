#ifndef _DEBUG_COLLISION_DRAW_SYSTEM_H_
#define _DEBUG_COLLISION_DRAW_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "../ECS_Base/system.h"
#include "../BasicComponents/Transform.h"

#include "../../SphereCollider.h"
#include "../../PlaneCollider.h"
#include "../../SurfaceCollider.h"

#include "../Camera/cameraclass.h"

/////////////
// GLOBALS //
/////////////
const bool DEPTH_OFF = true;


class CollisionDrawSystem : public System
{
public:
	CollisionDrawSystem();
	void Initialize();
	void Shutdown();

	void Update(EntityStorage&, float);
};

#endif