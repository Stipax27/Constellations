#ifndef _PIVOT_DRAW_SYSTEM_H_
#define _PIVOT_DRAW_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "../../Engine/ECS_Base/system.h"
#include "../../Engine/BasicComponents/Transform.h"

#include "../../Engine/Camera/cameraclass.h"


#define PIVOT_MIN_SCREEN_SIZE 0.02f
#define PIVOT_BASIC_RADIUS 0.1f;


class PivotDrawSystem : public System
{
public:
	PivotDrawSystem();
	void Initialize();
	void Shutdown();

	void Update(EntityStorage&, float);

private:
	FrustumClass* frustum;
	CameraClass* camera;
};

#endif