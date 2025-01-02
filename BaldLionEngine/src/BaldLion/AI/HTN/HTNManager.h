#pragma once
#include "HTNTypes.h"
#include "HTNTask.h"
#include "BaldLion/Core/Containers/HashMap.h"

namespace BaldLion::AI::HTN
{
	class HTNManager
	{
		//static void GenerateTask(HTNTask::TaskType TaskType, )

		static void Init();
		static void Stop();

		static bool RunPlanner(const HTNWorldStateBlackboard& worldStateBlackboard, 
			const HTNDomain& domain, DynamicArray<ui32>& plan);
		
	private:
		static DynamicArray<HTNTask> s_DefinedTasks;
		static HashMap<StringId, HTNDomain> s_DefinedDomains;

		static bool CheckTask(HTNWorldStateBlackboard& tempWorldStateBlackboard,
			DynamicArray<HTNWorldStateProperty>& prevBlackboardValues,
			ui32 taskIndex, 
			DynamicArray<ui32>& plan);
	};
}

