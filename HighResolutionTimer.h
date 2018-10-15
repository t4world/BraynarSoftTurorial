#pragma once
#include "windows.h"
class HighResolutionTimer
{
public:
	HighResolutionTimer();
	~HighResolutionTimer();
	void StartTime();
	double GetTime();
	double GetFrameTime();
private:
	double countsPerSecond = 0.0;
	__int64 CounterStart = 0;
	__int64 frameTimeOld = 0;
	double frameTime;
};

