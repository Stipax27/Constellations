#include "timer.h"


double timer::PCFreq = 0.0;
__int64 timer::counterStart = 0;

double timer::frameBeginTime = 0;
double timer::frameEndTime = 0;
double timer::nextFrameTime = 0;
double timer::frameRenderingDuration = 0.0;

double timer::currentTime = 0;
double timer::lastFrameTime = 0;
double timer::deltaTime = 0;

std::vector<double> timer::deltaList;
double timer::deltaAverage = 0;

void timer::StartCounter()
{
	LARGE_INTEGER li;

#if Debug
	if (!QueryPerformanceFrequency(&li)) MessageBox(hWnd, "timer fail", NULL, 0);
#else
	QueryPerformanceFrequency(&li);
#endif	

	PCFreq = double(li.QuadPart) / 1000.0;

	QueryPerformanceCounter(&li);
	counterStart = li.QuadPart;
}

double timer::GetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart - counterStart) / PCFreq;
}

void timer::CalcDeltaAverage()
{
	deltaList.push_back(timer::deltaTime);

	int size = deltaList.size();
	if (size > DELTALIST_COUNT) {
		deltaList.erase(deltaList.begin());
	}

	double count = 0;
	for (double c : deltaList) {
		count += c;
	}
	deltaAverage = count / size;
}