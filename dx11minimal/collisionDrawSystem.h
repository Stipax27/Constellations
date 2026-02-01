#ifndef _DEBUG_COLLISION_DRAW_SYSTEM_H_
#define _DEBUG_COLLISION_DRAW_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.h"

#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "SurfaceCollider.h"

#include "cameraclass.h"

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

	void Update(vector<Entity*>&, float);
};

#endif