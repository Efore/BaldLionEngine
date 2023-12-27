#pragma once

#include "BaldLion/Core/Containers/DynamicArray.h"
#include "BaldLion/Core/StringId.h"
#include <atomic>
#include <mutex>

namespace BaldLion
{
	namespace Threading
	{
		struct Task
		{
			void Wait();

			void SetCounter(ui32 counter);
			void ReduceCounter();

		private:

			std::atomic<ui32> m_counter;
			std::condition_variable m_jobsFinishedCV;
			std::mutex m_waitForJobsMutex;
		};
	}
}
