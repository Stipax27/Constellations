#ifndef _DELAYED_DESTROY_H_
#define _DELAYED_DESTROY_H_

#include "component.h"
#include "timer.h"


struct DelayedDestroy : Component
{
	DWORD lifeTime = 1000;
	DWORD startTime = timer::currentTime;
};

#endif