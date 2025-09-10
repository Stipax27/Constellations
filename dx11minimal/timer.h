#pragma once

#include <wtypes.h>

namespace timer
{
	double PCFreq;
	__int64 counterStart;
	
	double frameBeginTime;
	double frameEndTime;
	double nextFrameTime;
	double frameRenderingDuration;
	
	DWORD currentTime;
	DWORD lastFrameTime;
	DWORD deltaTime;

	void StartCounter();
	double GetCounter();
}