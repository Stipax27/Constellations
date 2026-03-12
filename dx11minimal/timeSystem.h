#ifndef _TIME_SYSTEM_H_
#define _TIME_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "timer.h"


class TimeSystem : public System
{
public:
	TimeSystem();
	void Initialize();
	void Shutdown();

	void Update(vector<Entity*>&, float);
};

#endif