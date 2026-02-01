#ifndef _ENTITY_MANAGER_SYSTEM_H_
#define _ENTITY_MANAGER_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "timer.h"

#include "DelayedDestroy.h"


class EntityManagerSystem : public System
{
public:
	EntityManagerSystem();
	void Initialize();
	void Shutdown();

	void Update(vector<Entity*>&, float);
};

#endif