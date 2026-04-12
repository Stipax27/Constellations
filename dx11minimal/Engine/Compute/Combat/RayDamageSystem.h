#ifndef _RAY_DAMAGE_SYSTEM_H_
#define _RAY_DAMAGE_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "../../ECS_Base/system.h"
#include "Health.h"
#include "RayDamager.h"

#include "../../Physic/Collision/CollisionManagerClass.h"


class RayDamageSystem : public System
{
public:
	RayDamageSystem();
	void Initialize();
	void Shutdown();

	void Update(EntityStorage&, float);

private:
	CollisionManagerClass* collisionManagerClass;
};

#endif