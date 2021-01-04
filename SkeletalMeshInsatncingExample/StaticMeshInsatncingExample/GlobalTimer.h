
#pragma once

#include <windows.h>
#include <chrono>
#include "Singleton.h"

using namespace std;
using namespace std::chrono;

class GlobalTimer : public TSingleton<GlobalTimer>
{
public:
	GlobalTimer(token) {};

public:
	void Initialize();
	void Pause();
	void Tick();

	float GetTime();
	float GetDeltaTime();

public:
	float m_deltaTime		= 0.0f;
	float m_duration		= 0.0f;
	float m_timeScale		= 1.0f;
	float m_pausedTime		= 0.0f;

	bool m_isPause = false;

	system_clock::time_point m_prevTime;
	system_clock::time_point m_currentTime;
	system_clock::time_point m_startTime;
};