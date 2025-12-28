#include "blpch.h"
#include "Time.h"

namespace BaldLion {

	HashTable<ui32, Timer*> Time::s_additionalTimers;
	Timer Time::s_globalTimer;
	ui32 Time::s_timerIDprovider;

	void Time::Init()
	{
		s_timerIDprovider = 1;
		s_globalTimer = Timer(s_timerIDprovider++, 0.0f);
		s_additionalTimers = HashTable<ui32, Timer*>(AllocationType::FreeList_Main, 8);
	}

	void Time::Stop()
	{
		s_additionalTimers.Delete();
	}

	void Time::UpdateCurrentTime(double currentTime)
	{
		constexpr float targetMs = (1.0f / MAX_FPS_GAMEPLAY) * 1000.0f;

		const float deltaMs = ((float)currentTime - s_globalTimer.GetCurrentTime()) * 1000.0f;
		const float deltaToTargetMs = targetMs - deltaMs;

		if(deltaToTargetMs > 0.0f)
		{
			const ui64 begin = std::chrono::duration_cast<std::chrono::nanoseconds>
				(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

			const ui64 ms = (ui64)(deltaToTargetMs);

			// Sleep till only 2 ms remain...
			const i64 timeToSleep = ms - 2;
			if (timeToSleep > 0)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(timeToSleep));
			}

			const ui64 deltaToTargetNS = (ui64)(deltaToTargetMs * 1e6);

			// ...and spin for the remaining time
			while (deltaToTargetNS > 
				ui64(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch())
				.count() - begin))
			{
			}
		}

		s_globalTimer.SetCurrentTime(currentTime);

		BL_HASHTABLE_FOR(s_additionalTimers, it)
		{
			it.GetValue()->SetCurrentTime(currentTime);
		}	
	}

	void Time::RequestNewTimer(Timer& timer)
	{
		const ui32 newTimerID = s_timerIDprovider++;
		timer = Timer(newTimerID, s_globalTimer.GetCurrentTimeAsDouble());
		s_additionalTimers.Emplace(newTimerID, &timer);
	}

	void Time::RemoveTimer(ui32 timerID)
	{		
		s_additionalTimers.Remove(timerID);
	}

}
