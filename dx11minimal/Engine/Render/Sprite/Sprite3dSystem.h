#ifndef _SPRITE3D_SYSTEM_H_
#define _SPRITE3D_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "../../ECS_Base/system.h"
#include "../../BasicComponents/Transform.h"

#include "../../Camera/frustumclass.h"

#include "Sprite.h"




class Sprite3dSystem : public System
{
public:
	Sprite3dSystem();
	void Initialize();
	void Shutdown();

	void Update(EntityStorage&, float);

private:
	FrustumClass* frustum;

private:
	void UpdateWorldMatrix(Transform);
};

#endif
