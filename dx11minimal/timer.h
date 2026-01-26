#ifndef _TIMER_H_
#define _TIMER_H_

#include <wtypes.h>

namespace timer
{
	extern double PCFreq;
	extern __int64 counterStart;
	
	extern double frameBeginTime;
	extern double frameEndTime;
	extern double nextFrameTime;
	extern double frameRenderingDuration;
	
	extern double currentTime;
	extern double lastFrameTime;
	extern double deltaTime;

	void StartCounter();
	double GetCounter();
}

#endif