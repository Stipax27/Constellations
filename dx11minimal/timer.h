#ifndef _TIMER_H_
#define _TIMER_H_

#include <wtypes.h>
#include <vector>

#define DELTALIST_COUNT 60

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

	extern std::vector<double> deltaList;
	extern double deltaAverage;

	void StartCounter();
	double GetCounter();
	void CalcDeltaAverage();
}

#endif