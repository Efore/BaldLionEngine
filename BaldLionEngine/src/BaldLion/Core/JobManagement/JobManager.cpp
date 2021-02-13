#include "blpch.h"
#include "JobManager.h"

namespace BaldLion
{
	namespace JobManagement {

		std::mutex JobManager::s_currentJobToWaitForMutex;
		StringId JobManager::s_currentJobToWaitFor;

		void JobManager::Init(ui32 threadsCount /*= std::thread::hardware_concurrency() - 1*/)
		{
			ThreadPool::Init(threadsCount);
		}

		void JobManager::Stop()
		{
			ThreadPool::Stop();
		}

		void JobManager::QueueJob(Job& job)
		{
			s_currentJobToWaitForMutex.lock();
			if (s_currentJobToWaitFor > 0 && s_currentJobToWaitFor != job.JobParentID)
				job.JobDependencyID = s_currentJobToWaitFor;
			s_currentJobToWaitForMutex.unlock();

			ThreadPool::QueueJob(job);
		}

		void JobManager::SetJobDependency(StringId jobIDs)
		{
			s_currentJobToWaitForMutex.lock();
			s_currentJobToWaitFor = jobIDs;
			s_currentJobToWaitForMutex.unlock();
		}

		void JobManager::WaitForJobs()
		{
			ThreadPool::WaitForJobs();
		}

	}
}