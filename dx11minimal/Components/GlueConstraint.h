#ifndef _GLUE_CONSTRAINT_H_
#define _GLUE_CONSTRAINT_H_

#include "../Engine/ECS_Base/component.h"
#include <vector>

struct GlueConstraint : Component
{
	vector<int> gluedEntities;
};

#endif