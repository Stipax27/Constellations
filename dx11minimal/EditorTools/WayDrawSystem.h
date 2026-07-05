#pragma once

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "../Engine/ECS_Base/system.h"
#include "../Engine/BasicComponents/Transform.h"

#include "../Engine/Physic/Collision/SphereCollider.h"
#include "../Engine/Physic/Collision/PlaneCollider.h"
#include "../Engine/Physic/Collision/SurfaceCollider.h"

#include "../Engine/Camera/cameraclass.h"



class WayDrawSystem : public System
{
public:
	WayDrawSystem();
	void Initialize() override;
	void Shutdown() override;

	void Update(EntityStorage&, float) override;
};