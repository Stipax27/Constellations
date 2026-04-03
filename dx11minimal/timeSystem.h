#ifndef _TIME_SYSTEM_H_
#define _TIME_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Engine/Lib/timer.h"


class TimeSystem : public System
{
public:
	TimeSystem();
	void Initialize();
	void Shutdown();

	void Update(EntityStorage&, float);
};

#endif