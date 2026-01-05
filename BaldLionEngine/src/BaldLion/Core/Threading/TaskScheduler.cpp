#include "blpch.h"
#include "TaskScheduler.h"

#include <glm/glm.hpp>

namespace BaldLion
{
	namespace Threading 
	{
		//--------------------------------------------------------------------------
		DynamicArray<std::thread> TaskScheduler::s_workerThreads;
		Queue<Task> TaskScheduler::s_taskQueue;

		std::mutex TaskScheduler::s_taskQueueMutex;
		bool TaskScheduler::s_running = false;

		std::atomic<ui32> TaskScheduler::s_activeJobs = 0;

		void TaskScheduler::Init(ui32 threadsCount /*= std::thread::hardware_concurrency() / 2*/)
		{
			s_workerThreads = DynamicArray<std::thread>(AllocationType::FreeList_Main, threadsCount);
			s_taskQueue = Queue<Task>(AllocationType::FreeList_Main, 512);

			s_running = true;
			for (ui32 i = 0; i < threadsCount; ++i)
			{
				s_workerThreads.EmplaceBack(TaskScheduler::ThreadProcess, i);
			}
		}

		void TaskScheduler::Stop()
		{
			s_running = false;
			WaitForAllJobs();
			s_workerThreads.DeleteNoDestructor();
		}

		void TaskScheduler::WaitForAllJobs()
		{
			while (s_activeJobs > 0);
		}

		void TaskScheduler::KickSingleTask(TaskID& taskID, SingleJobFunction jobFunction)
		{
			taskID.counter.fetch_add(1,std::memory_order_relaxed);			

			Task newEntry;

			newEntry.taskID = &taskID;
			newEntry.job = jobFunction;

			s_taskQueueMutex.lock();
			s_taskQueue.EmplaceBack(newEntry);
			s_taskQueueMutex.unlock();
		}

		void TaskScheduler::KickParallelTask(TaskID& taskID, ui32 iterationCount, ParallelJobFunction jobFunction)
		{
			const ui32 workerThreadCount = s_workerThreads.Capacity();
			const ui32 iterationsPerBatchBase = iterationCount / workerThreadCount;
			ui32 iterationsPerBatchRemaining = iterationCount % workerThreadCount;

			const ui32 requiredBatches = glm::min(iterationCount, workerThreadCount);

			ui32 iterationsDispatched = 0u;				
		

			for (ui32 i = 0u; i < requiredBatches; i++)
			{
				taskID.counter.fetch_add(1, std::memory_order_relaxed);
				ui32 iterationsForThisBatch = iterationsPerBatchBase;

				if (iterationsPerBatchRemaining > 0u)
				{
					iterationsForThisBatch++;
					iterationsPerBatchRemaining--;
				}

				Task newEntry;

				newEntry.taskID = &taskID;
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
			while (s_running)
			{				
#if BL_PROFILE_ACTIVE
				char name[10];
				sprintf(name, "Worker%i", threadIndex);
				BL_PROFILE_THREAD(name);
#endif

				Task entry;

				s_taskQueueMutex.lock();
				if (s_taskQueue.Size() > 0)
				{
					entry = s_taskQueue.Front();
					s_taskQueue.Pop();
					s_activeJobs.fetch_add(1);
				}
				s_taskQueueMutex.unlock();

				if (entry.taskID != nullptr)
				{					
					entry.job();
					entry.taskID->counter.fetch_sub(1);
					s_activeJobs.fetch_sub(1);
				}
			}
			return nullptr;
		}	

	}
}