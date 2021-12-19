#pragma once
#include "Containers/HashTable.h"

#define MAX_FPS 144.0f

namespace BaldLion
{
	class Timer {

	public:

		Timer() = default;

		Timer(ui32 timerID, double currentTime) : m_timerID(timerID), m_currentTime(currentTime), m_deltaTime(0.0f), m_timeScale(1.0f) {}

		void SetTimeScale(float timeScale) { m_timeScale = timeScale;}

		float GetUnscaledDeltaTime() const { return m_deltaTime; }
		float GetDeltaTime() const { return m_deltaTime * m_timeScale; }
		float GetDeltaTimeInMilliseconds() const { return m_deltaTime * m_timeScale * 1000.0f; }

		double GetCurrentTimeAsDouble() const { return m_currentTime; }
		float GetCurrentTime() const { return (float)m_currentTime; }
		float GetCurrentTimeInMilliseconds() const { return (float)m_currentTime * 1000.0f; }

		void SetCurrentTime(double currentTime) { 

			m_deltaTime = (float)currentTime - (float)m_currentTime;
			m_currentTime = currentTime; 
		}

	private:

		double m_currentTime;
		float m_deltaTime;
		float m_timeScale;
		ui32 m_timerID;
	};

	class Time {

	public:

		static void Init();
		static void Stop();

		static void SetTimeScale(float timeScale) { s_globalTimer.SetTimeScale(timeScale); }

		static float GetDeltaTime() { return s_globalTimer.GetDeltaTime(); }
		static float GetDeltaTimeInMilliseconds() { return s_globalTimer.GetDeltaTimeInMilliseconds(); }

		static double GetCurrentTimeAsDouble() { return s_globalTimer.GetCurrentTimeAsDouble(); }
		static float GetCurrentTime() { return s_globalTimer.GetCurrentTime(); }
		static float GetCurrentTimeInMilliseconds() { return s_globalTimer.GetCurrentTimeInMilliseconds(); }

		static bool SetCurrentTime(double currentTime);

		static Timer* RequestNewTimer();
		static void RemoveTimer(ui32 timerID);

	private:

		static HashTable<ui32, Timer> s_additionalTimers;
		static Timer s_globalTimer;
		static ui32 s_timerIDprovider;

	};

}
