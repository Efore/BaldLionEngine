#include "blpch.h"
#include "Time.h"

namespace BaldLion {

	HashTable<ui32, Timer> Time::s_additionalTimers;
	Timer Time::s_globalTimer;
	ui32 Time::s_timerIDprovider;

	void Time::Init()
	{
		s_timerIDprovider = 0;
		s_globalTimer = Timer(s_timerIDprovider++, 0.0f);
		s_additionalTimers = HashTable<ui32, Timer>(AllocationType::FreeList_Main, 10);
	}

	void Time::Stop()
	{
		s_additionalTimers.Delete();
	}

	bool Time::SetCurrentTime(double currentTime)
	{
		const float maxPeriod = 1.0f / MAX_FPS;

		if ((float)currentTime - s_globalTimer.GetCurrentTime() > maxPeriod) {

			s_globalTimer.SetCurrentTime(currentTime);

			for (HashTable<ui32, Timer>::Iterator it = s_additionalTimers.Begin(); it != s_additionalTimers.End(); ++it)
			{
				it.GetValue().SetCurrentTime(currentTime);
			}
			return true;
		}
		return false;
	}

	Timer* Time::RequestNewTimer()
	{
		const ui32 newTimerID = s_timerIDprovider++;
		s_additionalTimers.Emplace(newTimerID, newTimerID, s_globalTimer.GetCurrentTimeAsDouble());

		return &s_additionalTimers.Get(newTimerID);
	}

	void Time::RemoveTimer(ui32 timerID)
	{
		s_additionalTimers.Remove(timerID);
	}

}