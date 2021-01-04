
#include "GlobalTimer.h"

void GlobalTimer::Initialize()
{
	m_startTime = system_clock::now();
	m_currentTime = m_startTime;

	m_isPause = false;
}

float GlobalTimer::GetTime()
{
	duration<float> time = m_currentTime - m_startTime;
	return time.count();
}

float GlobalTimer::GetDeltaTime()
{
	if (!m_pausedTime)
	{
		return m_deltaTime * m_timeScale;
	}
	return 0.0f;
}

void GlobalTimer::Pause()
{
	m_isPause = false;
	m_deltaTime = 0.0f;
}

void GlobalTimer::Tick()
{
	m_prevTime = m_currentTime;
	m_currentTime = system_clock::now();
	duration<float> deltaTime = m_currentTime - m_prevTime;
	m_deltaTime = deltaTime.count();
}