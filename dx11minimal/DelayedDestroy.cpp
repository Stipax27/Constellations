#ifndef _DELAYED_DESTROY_C_
#define _DELAYED_DESTROY_C_

#include "component.h"
#include "timer.h"


struct DelayedDestroy : Component
{
	DWORD lifeTime = 1000;
	DWORD startTime = timer::currentTime;
};

#endif