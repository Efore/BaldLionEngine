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
			enum JobType : ui32
			{
				Unknown,
				ECS,
				Rendering,
				Navigation,
				Editor,

				Count
			} Type;

			static ui32 AllJobTypesMask;

			StringId JobName;
			std::function<void()> Task;
			StringId JobDependencyName;
			StringId JobParentName;
			ui32 ChildrenCount;		
			bool finish = false;

			Job(){}

			Job(const char* cName, JobType type = JobType::Unknown, StringId jobParentID = 0, ui32 childrenCount = 0 )
			{
				JobParentName = jobParentID;
				JobDependencyName = 0;
				Type = type;
				ChildrenCount = childrenCount;
				JobName = BL_STRING_TO_STRINGID(cName);
			}

			Job(const Job& other) : JobName(other.JobName), Type(other.Type), Task(other.Task), JobDependencyName(other.JobDependencyName), JobParentName(other.JobParentName), ChildrenCount(other.ChildrenCount){}

			Job& operator=(Job&& other) noexcept
			{
				if (&other == this)
					return *this;

				JobName = other.JobName;
				Type = other.Type;
				Task = other.Task;
				other.Task = nullptr;
				JobDependencyName = other.JobDependencyName;
				JobParentName = other.JobParentName;
				ChildrenCount = other.ChildrenCount;

				return *this;
			}

			Job& operator=(const Job& other) noexcept
			{
				if (&other == this)
					return *this;

				JobName = other.JobName;
				Type = other.Type;
				Task = other.Task;
				JobDependencyName = other.JobDependencyName;
				JobParentName = other.JobParentName;
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
			static void WaitForJobs(ui32 jobTypeMask);

			static void *ThreadProcess(ui32 threadIndex);
						
			static Queue<Job> s_jobQueue;
			static DynamicArray<std::thread> s_threads;
			static HashTable<StringId,ui32> s_activeJobs;	

			static ui32 s_activeJobsByType[Job::JobType::Count];
			static ui32 s_queuedJobsByType[Job::JobType::Count];

			static ui32 s_finishedTypes;

			static std::mutex s_jobManagingMutex;
			static std::mutex s_finishedTypesMutex;

			static std::condition_variable s_cvDependencyFinished;
			static std::condition_variable s_cvChildrenFinished;
			static std::condition_variable s_cvFinishedType;
		};
	}

}

