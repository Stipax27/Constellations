#ifndef _DEBUG_COLLISION_DRAW_SYSTEM_H_
#define _DEBUG_COLLISION_DRAW_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "../ECS_Base/system.h"
#include "../BasicComponents/Transform.h"

#include "../Physic/Collision/SphereCollider.h"
#include "../Physic/Collision/PlaneCollider.h"
#include "../Physic/Collision/SurfaceCollider.h"

#include "../Camera/cameraclass.h"



class CollisionDrawSystem : public System
{
public:
	CollisionDrawSystem();
	void Initialize();
	void Shutdown();

	void Update(EntityStorage&, float);
};

#endif