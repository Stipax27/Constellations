#ifndef _HEALTH_C_
#define _HEALTH_C_

#include "component.h"

struct Health : Component
{
	float hp = 100.0f;
	float maxHp = 100.0f;
};

#endif