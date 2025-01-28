#pragma once
#include "HTNTypes.h"
#include "HTNTask.h"
#include "HTNAgent.h"
#include "BaldLion/Core/Containers/HashMap.h"

namespace BaldLion::AI::HTN
{

	struct HTNOperator
	{
		typedef void (*htnOperatorFunc)(HTNAgent* htnAgent);

		htnOperatorFunc OperatorStartFunc;
		htnOperatorFunc OperatorInterruptFunc;
	};

	class HTNManager
	{
		//static void GenerateTask(HTNTask::TaskType TaskType, )
	public:
		static void Init();
		static void Stop();

		static bool RunPlanner(const HTNWorldStateBlackboard& worldStateBlackboard, 
			StringId domainID, DynamicArray<ui32>& plan);

		static const HTNTask& GetTask(ui32 taskIndex);
		static const HTNOperator& GetOperator(HTNOperatorType operatorType);


	private:
		static HashMap<StringId, HTNDomain> s_definedDomains;
		static HashMap <HTNOperatorType, HTNOperator> s_definedOperators;
		static DynamicArray<HTNTask> s_definedTasks;
		static DynamicArray<HTNAgent> s_agents;

		static bool CheckTask(HTNWorldStateBlackboard& tempWorldStateBlackboard,
			DynamicArray<HTNWorldStateProperty>& prevBlackboardValues,
			ui32 taskIndex, 
			DynamicArray<ui32>& plan);

	};
}

