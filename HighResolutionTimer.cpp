#include "HighResolutionTimer.h"



HighResolutionTimer::HighResolutionTimer()
{
}


HighResolutionTimer::~HighResolutionTimer()
{
}

void HighResolutionTimer::StartTime()
{
	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);
	countsPerSecond = (double)(frequencyCount.QuadPart);
	QueryPerformanceCounter(&frequencyCount);
	CounterStart = frequencyCount.QuadPart;

}

double HighResolutionTimer::GetTime()
{
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter(&currentCount);
	return (double)((currentCount.QuadPart - CounterStart) / countsPerSecond);
	return 0.0;
}

double HighResolutionTimer::GetFrameTime()
{
	LARGE_INTEGER currentTime;
	__int64 ticketCount;
	QueryPerformanceCounter(&currentTime);
	ticketCount = currentTime.QuadPart - frameTimeOld;
	frameTimeOld = currentTime.QuadPart;
	return (double)(ticketCount / countsPerSecond);
}
