#pragma once

#include "ThreadPool.h"

namespace BaldLion
{
	namespace JobManagement {

		class JobManager {

		public:
			static void Init(ui32 threadsCount = std::thread::hardware_concurrency() - 1);
			static void Stop();
			
			static void QueueJob(Job& job);
			static void SetJobDependency(StringId jobIDs);
			static void WaitForJobs();

		private:
			static std::mutex s_currentJobToWaitForMutex;
			static StringId s_currentJobToWaitFor;			
		};
	}
}