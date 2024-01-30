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
		s_additionalTimers = HashTable<ui32, Timer*>(AllocationType::FreeList_Main, 10);
	}

	void Time::Stop()
	{
		s_additionalTimers.Delete();
	}

	void Time::UpdateCurrentTime(double currentTime, bool& processNewFrame)
	{
		const float maxPeriod = 1.0f / MAX_FPS_GAMEPLAY;

		if ((float)currentTime - s_globalTimer.GetCurrentTime() > maxPeriod) {

			s_globalTimer.SetCurrentTime(currentTime);

			BL_HASHTABLE_FOR(s_additionalTimers, it)
			{
				it.GetValue()->SetCurrentTime(currentTime);
			}
			processNewFrame = true;
			return;
		}
		processNewFrame = false;
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
