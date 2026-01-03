#include "blpch.h"
#include "Time.h"

namespace BaldLion {

	HashTable<ui32, Timer*> Time::s_additionalTimers;
	Timer Time::s_globalTimer;
	ui32 Time::s_timerIDprovider;

	void Time::Init()
	{
		s_timerIDprovider = 1;
		s_globalTimer = Timer(s_timerIDprovider++, (ui64)std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
		s_additionalTimers = HashTable<ui32, Timer*>(AllocationType::FreeList_Main, 8);		 
	}

	void Time::Stop()
	{
		s_additionalTimers.Delete();
	}

	void Time::UpdateGlobalCurrentTime()
	{
		const ui64 newCurrentTime = (ui64)std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

		s_globalTimer.SetCurrentTimeMs(newCurrentTime);
		BL_HASHTABLE_FOR(s_additionalTimers, it)
		{
			it.GetValue()->SetCurrentTimeMs(newCurrentTime);
		}

	}

	void Time::RequestNewTimer(Timer& timer)
	{
		const ui32 newTimerID = s_timerIDprovider++;
		timer = Timer(newTimerID, s_globalTimer.GetCurrentTimeMs());
		s_additionalTimers.Emplace(newTimerID, &timer);
	}

	void Time::RemoveTimer(ui32 timerID)
	{		
		s_additionalTimers.Remove(timerID);
	}

}
