#ifndef _ENTITY_MANAGER_SYSTEM_H_
#define _ENTITY_MANAGER_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "../../ECS_Base/system.h"
#include "../../Lib/timer.h"

#include "DelayedDestroy.h"


class DelayedDestroySystem : public System
{
public:
	DelayedDestroySystem();
	void Initialize();
	void Shutdown();

	void Update(EntityStorage&, float);
};

#endif