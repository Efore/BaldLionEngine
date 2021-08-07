#pragma once

#include <thread>
#include <future>
#include <functional>
#include <type_traits>
#include <mutex>
#include <unordered_map>

#include "BaldLion/Core/Containers/DynamicArray.h"
#include "BaldLion/Core/Containers/Queue.h"
#include "BaldLion/Core/Containers/HashTable.h"
#include "BaldLion/Core/StringId.h"

namespace BaldLion
{
	namespace JobManagement {

		struct Job
		{
			StringId JobID;
			std::function<void()> Task;
			StringId JobDependencyID;
			StringId JobParentID;
			ui32 ChildrenCount;

			Job(){}

			Job(const char* cName, StringId jobParentID = 0, ui32 childrenCount = 0)
			{
				JobParentID = jobParentID;
				JobDependencyID = 0;
				ChildrenCount = childrenCount;
				JobID = STRING_TO_STRINGID(cName);
			}

			Job(const Job& other) : JobID(other.JobID), Task(other.Task), JobDependencyID(other.JobDependencyID), JobParentID(other.JobParentID), ChildrenCount(other.ChildrenCount){}

			Job& Job::operator=(Job&& other) noexcept
			{
				if (&other == this)
					return *this;

				JobID = other.JobID;
				Task = other.Task;
				other.Task = nullptr;
				JobDependencyID = other.JobDependencyID;
				JobParentID = other.JobParentID;
				ChildrenCount = other.ChildrenCount;

				return *this;
			}

			Job& Job::operator=(const Job& other) noexcept
			{
				if (&other == this)
					return *this;

				JobID = other.JobID;
				Task = other.Task;
				JobDependencyID = other.JobDependencyID;
				JobParentID = other.JobParentID;
				ChildrenCount = other.ChildrenCount;

				return *this;
			}
		};

		class ThreadPool
		{	
			friend class JobManager;

		private:

			static void Init(ui32 threadsCount);
			static void Stop();
			static void QueueJob(const Job& job);
			static void WaitForJobs();

			static void *ThreadUsage(ui32 threadIndex);
						
			static Queue<Job> s_jobQueue;
			static DynamicArray<std::thread> s_threads;
			static HashTable<StringId,ui32> s_activeJobs;	

			static bool s_jobsFinished;

			static std::mutex s_queueMutex;
			static std::mutex s_jobsFinishedMutex;

			static std::condition_variable s_cvDependencyFinished;
			static std::condition_variable s_cvChildrenFinished;
			static std::condition_variable s_cvJobsFinished;
		};
	}

}

