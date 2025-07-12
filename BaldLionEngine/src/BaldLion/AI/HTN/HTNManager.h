#pragma once
#include "HTNTypes.h"
#include "HTNTask.h"
#include "HTNAgent.h"
#include "BaldLion/Core/Containers/HashMap.h"

namespace BaldLion::AI::HTN
{
	class HTNManager
	{
		
	public:
		static void Init();
		static void Stop();
		
		static void Serialize();

		static bool RunPlanner(StringId worldStateBlackboardId,
			StringId domainID, DynamicArray<ui32>& plan);

		static HashMap <HTNOperatorType, HTNOperator> s_definedOperators;

		static HashMap<StringId, HTNWorldStateBlackboard> s_definedWorldStateBlackboards;
		static HashMap<StringId, HTNDomain> s_definedDomains;
		static DynamicArray<HTNTask> s_definedTasks;
		static DynamicArray<HTNAgent> s_agents;

	private:

		static bool CheckTask(HTNWorldStateBlackboard& tempWorldStateBlackboard,
			DynamicArray<HTNWorldStateEffect>& prevBlackboardValues,
			ui32 taskIndex, 
			DynamicArray<ui32>& plan);

	};
}

