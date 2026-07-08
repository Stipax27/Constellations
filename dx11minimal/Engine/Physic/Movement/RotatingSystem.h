#ifndef _ROTATING_SYSTEM_H_
#define _ROTATING_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "../../ECS_Base/system.h"
#include "../../BasicComponents/Transform.h"
#include "RotatingBody.h"


class RotatingSystem : public System
{
public:
	RotatingSystem();
	void Initialize() override;
	void Shutdown() override;

	void Update(EntityStorage&, float) override;
};

#endif