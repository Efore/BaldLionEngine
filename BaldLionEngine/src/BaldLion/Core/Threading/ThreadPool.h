#pragma once

#include <thread>
#include <future>
#include <functional>
#include <type_traits>
#include <mutex>

#include "BaldLion/Core/StringId.h"
#include "BaldLion/Core/Containers/DynamicArray.h"

namespace BaldLion
{
	namespace Threading {

		struct Job
		{
			StringId Name;
			std::function<void()> Task;

			Job(){}

			Job(const char* cName)
			{
				Name = STRING_TO_ID(cName);
			}
		};

		struct Worker
		{
			uint32_t ID;
			std::thread Thread;

			Job* CurrentJob;
			bool Alive;

			Worker(){}
			Worker(uint32_t iID) : ID(iID), CurrentJob(0), Alive(true) {}
		};

		class ThreadPool
		{

		public:

			static void Init(uint32_t workersCount = std::thread::hardware_concurrency());
			static void Stop();
			static void QueueJob(const Job& job);

		protected:
			static std::vector<Worker> s_workers;
			static std::vector<Job> s_jobQueue;

			static std::mutex s_QueueMutex;
			static std::condition_variable s_jobsPending;
			static std::condition_variable s_jobsCompleted;

			static uint32_t s_workersActiveMask;
		};
	}

}

