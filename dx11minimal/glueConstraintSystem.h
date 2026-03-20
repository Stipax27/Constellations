#ifndef _GLUE_CONSTRAINT_SYSTEM_H_
#define _GLUE_CONSTRAINT_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"

#include "Transform.h"
#include "PhysicBody.h"
#include "Components/GlueConstraint.h"


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