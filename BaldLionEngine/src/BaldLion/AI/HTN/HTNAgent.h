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

		void SetWorldStateBlackboardId(StringId worldStateBlackboardId);

		ui32 GetAgentId() const { return m_agentID; }
		void OnOperatorFinished(HTNOperatorType typeFinished, bool success);
		bool OnRunPlannerFinished(const EventEntry& e);

	private:
		DynamicArray<ui32> m_plan;
		StringId m_worldStateBlackboardId;
		ui32 m_currentPlanStep = 0;
		i32 m_agentID = -1;			

		Threading::TaskID m_requestPlanTask;
		bool m_validPlan = false;

	public:
		HTNOperatorType currentOperatorType = HTNOperatorType::Count;
		StringId currentDomainId;
	};
}


