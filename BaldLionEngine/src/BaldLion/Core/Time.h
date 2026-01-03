#pragma once
#include "Containers/HashTable.h"

namespace BaldLion
{
	class Timer {

	public:

		Timer() = default;

		Timer(ui32 timerID, ui64 currentTimeMs) : m_timerID(timerID), m_currentTimeMs(currentTimeMs), m_startTimeMs(currentTimeMs), m_deltaTime(0.0f), m_timeScale(1.0f), m_prevTimeScale(1.0f) {}

		void SetTimeScale(float timeScale) { m_timeScale = timeScale;}

		void Pause() 
		{ 
			if (m_timeScale > 0.0f) 
			{
				m_prevTimeScale = m_timeScale;
				m_timeScale = 0.0f;
			}
		}

		void Resume()
		{
			if (m_timeScale == 0.0f)
			{
				m_timeScale = m_prevTimeScale;
			}
		}

		void Reset() 
		{
			m_startTimeMs = m_currentTimeMs;
		}

		double GetUnscaledDeltaTime() const { return m_deltaTime; }
		double GetDeltaTime() const { return m_deltaTime * m_timeScale; }
		double GetDeltaTimeInMilliseconds() const { return m_deltaTime * m_timeScale * 1000.0f; }

		ui64 GetCurrentTimeMs() const { return m_currentTimeMs; }
		float GetStartTimeMs() const { return  m_startTimeMs; }

		void SetCurrentTimeMs(ui64 currentTimeMs) { 

			m_deltaTime = (double)(currentTimeMs - m_currentTimeMs) / 1000.f;
			m_currentTimeMs = currentTimeMs;
		}

		ui32 GetTimerID() const 
		{
			return m_timerID;
		}

	private:

		ui64 m_currentTimeMs;
		ui64 m_startTimeMs;
		double m_deltaTime;
		float m_timeScale;
		float m_prevTimeScale;
		ui32 m_timerID = 0;
	};

	class Time {

	public:

		static void Init();
		static void Stop();

		static void SetTimeScale(float timeScale) { s_globalTimer.SetTimeScale(timeScale); }

		static float GetGlobalDeltaTime() { return s_globalTimer.GetDeltaTime(); }
		static float GetGlobalDeltaTimeInMilliseconds() { return s_globalTimer.GetDeltaTimeInMilliseconds(); }

		static ui64 GetCurrentTimeMs() { return s_globalTimer.GetCurrentTimeMs(); }

		static void UpdateGlobalCurrentTime();

		static void RequestNewTimer(Timer& timer);
		static void RemoveTimer(ui32 timerID);

	private:

		static HashTable<ui32, Timer*> s_additionalTimers;
		static Timer s_globalTimer;
		static ui32 s_timerIDprovider;

	};

}
