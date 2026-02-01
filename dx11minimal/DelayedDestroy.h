#ifndef _DELAYED_DESTROY_H_
#define _DELAYED_DESTROY_H_

#include "component.h"
#include "timer.h"


struct DelayedDestroy : Component
{
	double lifeTime = 1000;
	double startTime = timer::currentTime;
};

#endif