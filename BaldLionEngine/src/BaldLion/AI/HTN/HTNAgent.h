#pragma once
#include "HTNTypes.h"
#include "BaldLion/Core/Threading/TaskScheduler.h"

namespace BaldLion::AI::HTN
{
	class HTNAgent
	{

	public:
		HTNAgent(StringId domainID, i32 agentID);
		~HTNAgent();

		void RequestPlan();
		void RunPlan();		
		void StopPlan();
		void AdvancePlan();

		ui32 GetAgentId() const { return m_agentID; }


	private:
		HTNWorldStateBlackboard m_worldStateBlackboard;
		DynamicArray<ui32> m_plan;
		ui32 m_currentPlanStep = 0;
		i32 m_agentID = -1;			

		Threading::Task m_requestPlanTask;
		bool m_validPlan;

	public:
		HTNOperatorType currentOperatorType = HTNOperatorType::Count;
		StringId currentDomainId;
	};
}


