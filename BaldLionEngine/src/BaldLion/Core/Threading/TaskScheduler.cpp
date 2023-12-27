#include "blpch.h"
#include "TaskScheduler.h"

#include <glm/glm.hpp>

namespace BaldLion
{
	namespace Threading 
	{
		DynamicArray<std::thread> TaskScheduler::s_workerThreads;
		Queue<TaskEntry> TaskScheduler::s_taskQueue;
		std::mutex TaskScheduler::s_taskQueueMutex;

		std::atomic<ui32> TaskScheduler::s_activeJobs = 0;

		std::mutex TaskScheduler::s_waitForJobsMutex;
		std::condition_variable TaskScheduler::s_waitForJobsCV;

		void TaskScheduler::Init(ui32 threadsCount /*= std::thread::hardware_concurrency() / 2*/)
		{
			s_workerThreads = DynamicArray<std::thread>(AllocationType::FreeList_Main, threadsCount);
			s_taskQueue = Queue<TaskEntry>(AllocationType::FreeList_Main, 1000);

			for (ui32 i = 0; i < threadsCount; ++i)
			{
				s_workerThreads.EmplaceBack(ThreadProcess,i);
			}
		}

		void TaskScheduler::Stop()
		{
			WaitForAllJobs();
			s_workerThreads.DeleteNoDestructor();
		}

		void TaskScheduler::WaitForAllJobs()
		{
			std::unique_lock<std::mutex> waitActiveJobsLock(s_waitForJobsMutex);
			s_waitForJobsCV.wait(waitActiveJobsLock, [] { return s_activeJobs == 0; });
		}

		void TaskScheduler::KickSingleTask(Task& task, SingleJobFunction jobFunction)
		{
			task.SetCounter(1);

			TaskEntry newEntry;

			newEntry.task = &task;
			newEntry.job = jobFunction;

			s_taskQueueMutex.lock();
			s_taskQueue.EmplaceBack(newEntry);
			s_taskQueueMutex.unlock();
		}

		void TaskScheduler::KickParallelTask(Task& task, ui32 iterationCount, ParallelJobFunction jobFunction)
		{
			const ui32 workerThreadCount = s_workerThreads.Capacity();
			const ui32 iterationsPerBatchBase = iterationCount / workerThreadCount;
			ui32 iterationsPerBatchRemainder = iterationCount % workerThreadCount;

			const ui32 requiredBatches = glm::min(iterationCount, workerThreadCount);

			ui32 iterationsDispatched = 0u;
						
			task.SetCounter(requiredBatches);

			for (ui32 i = 0u; i < requiredBatches; i++)
			{
				ui32 iterationsForThisBatch = iterationsPerBatchBase;

				if (iterationsPerBatchRemainder > 0u)
				{
					iterationsForThisBatch++;
					iterationsPerBatchRemainder--;
				}

				TaskEntry newEntry;

				newEntry.task = &task;
				newEntry.job = [jobFunction,iterationsDispatched, iterationsForThisBatch]()
					{
						jobFunction(iterationsDispatched, iterationsDispatched + iterationsForThisBatch - 1u);
					};

				iterationsDispatched += iterationsForThisBatch;

				s_taskQueueMutex.lock();
				s_taskQueue.EmplaceBack(newEntry);
				s_taskQueueMutex.unlock();
			}
		}

		void* TaskScheduler::ThreadProcess(ui32 threadIndex)
		{
			while (true)
			{
				TaskEntry entry;

				s_taskQueueMutex.lock();
				if (s_taskQueue.Size() > 0)
				{
					entry = s_taskQueue.Front();
					s_taskQueue.Pop();
					s_activeJobs++;
				}
				s_taskQueueMutex.unlock();

				if (entry.task != nullptr)
				{					
					entry.job();
					entry.task->ReduceCounter();
					s_activeJobs--;
					s_waitForJobsCV.notify_all();
				}
			}
		}

	}
}