#ifndef _GLUE_CONSTRAINT_H_
#define _GLUE_CONSTRAINT_H_

#include "../component.h"
#include <vector>

struct GlueConstraint : Component
{
	vector<int> gluedEntities;
};

#endif