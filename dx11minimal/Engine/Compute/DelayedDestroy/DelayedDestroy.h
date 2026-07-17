#ifndef _DELAYED_DESTROY_H_
#define _DELAYED_DESTROY_H_

#include "../../ECS_Base/component.h"


struct DelayedDestroy : Component
{
	double lifeTime = 1000;
	double startTime = -1;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DelayedDestroy,
	active,
	lifeTime,
	startTime)

#endif