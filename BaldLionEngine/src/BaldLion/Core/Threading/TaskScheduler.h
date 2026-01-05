#pragma once

#include "BaldLion/Core/Containers/Queue.h"
#include "BaldLion/Core/Containers/LockFree.h"

#include <thread>
#include <future>
#include <functional>
#include <type_traits>
#include <mutex>

namespace BaldLion
{
	namespace Threading
	{
		typedef std::function<void()> SingleJobFunction;
		typedef std::function<void(ui32 firstIterationIndex, ui32 lastIterationIndex)> ParallelJobFunction;		

		struct TaskID
		{
			inline void Wait() const
			{
				while (counter > 0)
				{

				}
			}

			std::atomic<ui32> counter;
		};

		struct Task
		{
			TaskID* taskID = nullptr;
			SingleJobFunction job;
		};


		class TaskScheduler
		{

		public:
			static void Init(ui32 threadsCount = std::thread::hardware_concurrency() / 2);
			static void Stop();

			static void WaitForAllJobs();

			static void KickSingleTask(TaskID& taskID, SingleJobFunction jobFunction);
			static void KickParallelTask(TaskID& taskID, ui32 iterationCount, ParallelJobFunction jobFunction);

			static void* ThreadProcess(ui32 threadIndex);

		private:

			static DynamicArray<std::thread > s_workerThreads;
			static Queue<Task> s_taskQueue;
			static std::mutex s_taskQueueMutex;
			static std::atomic<ui32> s_activeJobs;
			static bool s_running;
		};


	}
}