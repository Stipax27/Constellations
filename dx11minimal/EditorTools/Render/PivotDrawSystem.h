#ifndef _PIVOT_DRAW_SYSTEM_H_
#define _PIVOT_DRAW_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "../Engine/ECS_Base/system.h"
#include "../../BasicComponents/Transform.h"
#include "Nebula.h"

#include "../../Camera/frustumclass.h"


class PivotDrawSystem : public System
{
public:
	PivotDrawSystem();
	void Initialize();
	void Shutdown();

	void Update(EntityStorage&, float);

private:
	FrustumClass* frustum;

private:
	void PSModeSet(pMode, bool);
};

#endif