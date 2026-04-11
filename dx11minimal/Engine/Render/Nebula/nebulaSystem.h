#ifndef _NEBULA_SYSTEM_H_
#define _NEBULA_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "../../ECS_Base/system.h"
#include "../../BasicComponents/Transform.h"
#include "Nebula.h"

#include "../../Camera/frustumclass.h"


class NebulaSystem : public System
{
public:
	NebulaSystem();
	void Initialize();
	void Shutdown();

	void Update(EntityStorage&, float);

private:
	FrustumClass* frustum;

private:
	void PSModeSet(pMode, bool);
};

#endif