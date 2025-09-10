#pragma once

#include <wtypes.h>

namespace timer
{
	extern double PCFreq;
	extern __int64 counterStart;
	
	extern double frameBeginTime;
	extern double frameEndTime;
	extern double nextFrameTime;
	extern double frameRenderingDuration;
	
	extern DWORD currentTime;
	extern DWORD lastFrameTime;
	extern DWORD deltaTime;

	void StartCounter();
	double GetCounter();
}