#ifndef _STARCLAY_SYSTEM_H_
#define _STARCLAY_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "../../ECS_Base/system.h"
#include "../../BasicComponents/Transform.h"
#include "StarClay.h"

#include "../../Camera/frustumclass.h"
#include "../../Camera/cameraclass.h"


class StarClaySystem : public System
{
public:
	StarClaySystem();
	void Initialize();
	void Shutdown();

	void Update(EntityStorage&, float);

private:
	FrustumClass* frustum;
	CameraClass* camera;

private:
	void UpdateWorldMatrix(Transform worldTransform);
	int GetVertexCount(point3d position, int min, int max, float size);
};

#endif