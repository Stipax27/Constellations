#ifndef _GLUE_CONSTRAINT_SYSTEM_H_
#define _GLUE_CONSTRAINT_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"

#include "Engine/BasicComponents/Transform.h"
#include "PhysicBody.h"
#include "Components/GlueConstraint.h"

#include "componentutils.h"


class GlueConstraintSystem : public System
{
public:
	GlueConstraintSystem();
	void Initialize();
	void Shutdown();

	void Update(vector<Entity*>&, float);

private:
	EntityStorage* entityStorage;
};

#endif