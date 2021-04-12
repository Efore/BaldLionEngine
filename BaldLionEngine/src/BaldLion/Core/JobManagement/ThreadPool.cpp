#include "blpch.h"
#include "ThreadPool.h"

namespace BaldLion {

	namespace JobManagement
	{		
		Queue<Job> ThreadPool::s_jobQueue;
		HashTable<StringId, ui32> ThreadPool::s_activeJobs;
		DynamicArray<std::thread> ThreadPool::s_threads;

		bool ThreadPool::s_jobsFinished;

		std::mutex ThreadPool::s_queueMutex;	
		std::mutex ThreadPool::s_jobsFinishedMutex;

		std::condition_variable ThreadPool::s_cvDependencyFinished;
		std::condition_variable ThreadPool::s_cvChildrenFinished;
		std::condition_variable ThreadPool::s_cvJobsFinished;

		void ThreadPool::Init(ui32 workersCount /*= std::thread::hardware_concurrency()*/)
		{
			s_jobQueue = Queue<Job>(AllocationType::FreeList_Main, 100);
			s_activeJobs = HashTable<StringId, ui32>(AllocationType::FreeList_Main, 100);
			s_threads = DynamicArray<std::thread>(AllocationType::FreeList_Main, workersCount);
			s_jobsFinished = true;

			//Starting threads
			for (ui32 i = 0; i < workersCount; ++i)
			{
				s_threads.EmplaceBack(std::thread(ThreadUsage,i));
			}		
		}

		void ThreadPool::Stop()
		{
			s_threads.ClearNoDestructor();
		}

		void ThreadPool::QueueJob(const Job& job)
		{
			BL_ASSERT("Job has no task", job.Task != nullptr);

			s_jobsFinishedMutex.lock();
			s_jobsFinished = false;
			s_jobsFinishedMutex.unlock();

			//Enqueuing job
			s_queueMutex.lock();
			s_jobQueue.EmplaceBack(job);
			s_queueMutex.unlock();
		}	

		void* ThreadPool::ThreadUsage(ui32 threadIndex)
		{	
			while(true)
			{
				OPTICK_THREAD("Worker");
				Job job;
				std::function<void()> task = nullptr;

				//Taking next job in queue and assigning Task
				s_queueMutex.lock();

				if (!s_jobQueue.IsEmpty())
				{
					job = s_jobQueue.Front();
					task = job.Task;
					s_jobQueue.Pop();
				}

				if (!task)
				{
					s_queueMutex.unlock();
				}
				else
				{
					//Adding JobID and its number off children to the active Job map
					s_activeJobs.Emplace(job.JobID, std::move(job.ChildrenCount));
					s_queueMutex.unlock();

					//IF this job depends on another to start, wait until the other job is no longer active
					if (job.JobDependencyID > 0)
					{
						std::unique_lock<std::mutex> dependencyLock(s_queueMutex);
						const StringId jobDependencyID = job.JobDependencyID;
						s_cvDependencyFinished.wait(dependencyLock, [jobDependencyID] { return !s_activeJobs.Contains(jobDependencyID); });
					}

					task();

					//After the task has been processed, if it has children, wait until all of them are processed 
					if (job.ChildrenCount > 0)
					{
						std::unique_lock<std::mutex> checkChildrenLock(s_queueMutex);
						s_cvChildrenFinished.wait(checkChildrenLock, [job] { return s_activeJobs.Get(job.JobID) == 0; });
					}

					//Closure of the job logic, making sure that only one thread closes its job at the same time
					s_jobsFinishedMutex.lock();

					//Removing the job from the active jobs map
					s_queueMutex.lock();
					
					bool jobQueueEmpty = s_jobQueue.IsEmpty();

					s_activeJobs.Remove(job.JobID);

					//If this job has a parent, reduce its pending children in one
					if (job.JobParentID > 0) {
						ui32 children = s_activeJobs.Get(job.JobParentID) - 1;
						s_activeJobs.Set(job.JobParentID, children);
					}

					bool activeJobsEmpty = s_activeJobs.Size() == 0;

					s_queueMutex.unlock();

					//Checking if all jobs are finished
					s_jobsFinished = jobQueueEmpty && activeJobsEmpty;

					//Closure is finished
					s_jobsFinishedMutex.unlock();

					//Notifying dependency and children condition variables
					s_cvDependencyFinished.notify_all();
					s_cvChildrenFinished.notify_all();

					//Notifying job finished condition variable
					s_cvJobsFinished.notify_all();
				}
			}
		}

		void ThreadPool::WaitForJobs()
		{	
			std::unique_lock<std::mutex> waitActiveJobsLock(s_jobsFinishedMutex);
			s_cvJobsFinished.wait(waitActiveJobsLock, [] { return s_jobsFinished; });						
		}
	}
}