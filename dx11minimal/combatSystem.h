#ifndef _COMBAT_SYSTEM_H_
#define _COMBAT_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.h"
#include "Health.h"


class CombatSystem : public System
{
public:
	CombatSystem();
	void Initialize();
	void Shutdown();

	void Update(vector<Entity*>&, float);
};

#endif