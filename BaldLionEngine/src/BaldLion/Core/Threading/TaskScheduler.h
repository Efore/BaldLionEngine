#pragma once

#include "Task.h"
#include "BaldLion/Core/Containers/Queue.h"

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

		struct TaskEntry
		{
			Task* task = nullptr;
			SingleJobFunction job;
		};

		struct WorkerThread
		{
			WorkerThread(ui32 threadIndex);

			char name[10];
			std::thread thread;
		};

		class TaskScheduler
		{

		public:
			static void Init(ui32 threadsCount = std::thread::hardware_concurrency() / 2);
			static void Stop();

			static void WaitForAllJobs();

			static void KickSingleTask(Task& task, SingleJobFunction jobFunction);
			static void KickParallelTask(Task& task, ui32 iterationCount, ParallelJobFunction jobFunction);

			static void* ThreadProcess(ui32 threadIndex);

		private:

			static DynamicArray<WorkerThread> s_workerThreads;
			static Queue<TaskEntry> s_taskQueue;
			static std::mutex s_taskQueueMutex;
			static std::atomic<ui32> s_activeJobs;
		};


	}
}