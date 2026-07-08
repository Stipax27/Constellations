#pragma once

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "../ECS_Base/system.h"
#include "../BasicComponents/Transform.h"

#include "../Camera/cameraclass.h"



class LinkRadiusDrawSystem : public System
{
public:
	LinkRadiusDrawSystem();
	void Initialize() override;
	void Shutdown() override;

	void Update(EntityStorage&, float) override;
};