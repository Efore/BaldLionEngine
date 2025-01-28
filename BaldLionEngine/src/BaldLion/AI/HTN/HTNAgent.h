#pragma once
#include "HTNTypes.h"
#include "BaldLion/Core/Threading/TaskScheduler.h"
#include "BaldLion/Core/EventManager.h"

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

		ui32 GetAgentId() const { return m_agentID; }
		bool OnOperatorFinished(const EventEntry& e);

	private:
		HTNWorldStateBlackboard m_worldStateBlackboard;
		DynamicArray<ui32> m_plan;
		ui32 m_currentPlanStep = 0;
		i32 m_agentID = -1;			

		Threading::Task m_requestPlanTask;
		bool m_validPlan = false;

	public:
		HTNOperatorType currentOperatorType = HTNOperatorType::Count;
		StringId currentDomainId;
	};
}


