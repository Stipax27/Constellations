#pragma once

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "../ECS_Base/system.h"
#include "../BasicComponents/Transform.h"

#include "../Physic/Movement/RotatingBody.h"

#include "../Camera/cameraclass.h"



class WayDrawSystem : public System
{
public:
	WayDrawSystem();
	void Initialize() override;
	void Shutdown() override;

	void Update(EntityStorage&, float) override;
};