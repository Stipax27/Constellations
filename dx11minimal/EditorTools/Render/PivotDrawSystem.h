#ifndef _PIVOT_DRAW_SYSTEM_H_
#define _PIVOT_DRAW_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "../../Engine/ECS_Base/system.h"
#include "../../Engine/BasicComponents/Transform.h"

#include "../../Engine/Camera/frustumclass.h"


class PivotDrawSystem : public System
{
public:
	PivotDrawSystem();
	void Initialize();
	void Shutdown();

	void Update(EntityStorage&, float);

private:
	FrustumClass* frustum;
};

#endif