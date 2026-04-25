#include "blpch.h"
#include "Time.h"

namespace BaldLion {

	HashTable<ui32, Timer*> Time::s_additionalTimers;
	Timer Time::s_globalTimer;
	ui32 Time::s_timerIDprovider;
	std::atomic<bool> Time::s_sleepingUntilNextFrame = false;
	std::atomic<ui64> Time::s_endOfFrameTimeNs = 0;
	std::atomic<ui64> Time::s_endOfFrameBeforeBusyWorkTimeNs = 0;
	

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

	void Time::SleepThread(ui64 sleepMiliseconds)
	{
		const ui64 beginNs = std::chrono::duration_cast<std::chrono::nanoseconds>
			(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
				
		const ui64 timeToSleep = sleepMiliseconds - (sleepMiliseconds * 0.33f);
		if (timeToSleep > 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(timeToSleep));
		}		

		const ui64 sleepNS = sleepMiliseconds * 1000000;

		// ...and spin for the remaining time 
		while (sleepNS >
			ui64(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch())
				.count()) - beginNs)
		{
		}		
	}

	void Time::SleepThreadUntilEndOfFrame()
	{		
		BL_PROFILE_FUNCTION();

		const std::chrono::steady_clock::time_point busyWorkTimePoint
			= std::chrono::steady_clock::time_point(std::chrono::nanoseconds(s_endOfFrameBeforeBusyWorkTimeNs.load()));

		const std::chrono::steady_clock::time_point endOfFrameTimePoint 
			= std::chrono::steady_clock::time_point(std::chrono::nanoseconds(s_endOfFrameTimeNs.load()));

		std::this_thread::sleep_until(busyWorkTimePoint);

		//Busy work for the remaining time
		while (std::chrono::steady_clock::now() < endOfFrameTimePoint)
		{
		}
	}

}
