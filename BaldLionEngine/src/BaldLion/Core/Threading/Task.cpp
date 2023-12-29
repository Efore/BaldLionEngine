#include "blpch.h"
#include "Task.h"
#include "TaskScheduler.h"

namespace BaldLion
{
	namespace Threading
	{		
		void Task::Wait() const
		{
			while (m_counter > 0);
		}

		void Task::SetCounter(ui32 counter)
		{
			m_counter.fetch_add(counter,std::memory_order_relaxed);
		}

		void Task::ReduceCounter()
		{
			m_counter.fetch_sub(1u, std::memory_order_relaxed);			
		}

	}
}


