#include "blpch.h"
#include "ThreadPool.h"

namespace BaldLion {

	namespace Threading
	{
		std::vector<Worker> ThreadPool::s_workers;
		std::vector<Job> ThreadPool::s_jobQueue;

		std::mutex ThreadPool::s_QueueMutex;

		std::condition_variable ThreadPool::s_jobsPending;
		std::condition_variable ThreadPool::s_jobsCompleted;

		uint32_t ThreadPool::s_workersActiveMask = 0;


		void ThreadPool::Init(uint32_t workersCount /*= std::thread::hardware_concurrency()*/)
		{
			s_workers = std::vector<Worker>(workersCount);			
			s_jobQueue = std::vector<Job>(workersCount);

			for (uint32_t i = 0; i < workersCount; ++i)
			{
				Worker worker(i);
			}

		
		}

		void ThreadPool::Stop()
		{
		
		}

		void ThreadPool::QueueJob(const Job& job)
		{

		}


	}
}