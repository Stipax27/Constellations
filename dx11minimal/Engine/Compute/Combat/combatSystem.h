#ifndef _COMBAT_SYSTEM_H_
#define _COMBAT_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "../../ECS_Base/system.h"
#include "Health.h"
#include "DamageBlocker.h"


class CombatSystem : public System
{
public:
	CombatSystem();
	void Initialize();
	void Shutdown();

	void Update(EntityStorage&, float);
};

#endif