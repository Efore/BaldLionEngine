#pragma once
#include "HTNTypes.h"
#include "BaldLion/Core/Threading/TaskScheduler.h"
#include "BaldLion/Core/EventManager.h"

namespace BaldLion::AI::HTN
{
	class HTNAgent
	{	

	public:

		enum class HTNAgentState : ui8
		{
			Idle,
			WaitingForPlan,
			RunningPlan
		};

		HTNAgent(StringId worldStateBlackboardID, StringId initialDomainID, i32 agentIdx);
		~HTNAgent();

		void RequestPlan();
		void RunPlan();		
		void StopPlan();

		void SetCurrentDomain(StringId domainID);

		ui32 GetAgentId() const { return m_agentIdx; }
		void OnOperatorFinished(HTNOperatorType typeFinished, bool success);
		bool OnRunPlannerFinished(const EventEntry& e);

	private:
		DynamicArray<ui32> m_plan;
		ui32 m_currentPlanStep = 0;
		i32 m_agentIdx = -1;			

		Threading::TaskID m_requestPlanTask;
		bool m_validPlan = false;		

	public:

		HTNOperatorType m_currentOperatorType = HTNOperatorType::Count;
		StringId m_worldStateBlackboardID;
		StringId m_currentDomainID;		
		HTNAgentState m_currentState = HTNAgentState::Idle;
		bool m_isActive;
	};
}


