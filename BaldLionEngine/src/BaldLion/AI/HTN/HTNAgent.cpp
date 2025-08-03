#include "blpch.h"
#include "HTNAgent.h"
#include "HTNManager.h"

namespace BaldLion::AI::HTN
{
	HTNAgent::HTNAgent(StringId worldStateBlackboardID, StringId initialDomainID, i32 agentIdx) 
		: m_worldStateBlackboardID(worldStateBlackboardID), m_currentDomainID(initialDomainID), m_agentIdx(agentIdx)
	{
		m_plan = DynamicArray<ui32>(AllocationType::FreeList_Main, 32);
	}

	HTNAgent::~HTNAgent()
	{
		m_requestPlanTask.Wait();
		m_plan.Delete();
	}

	void HTNAgent::RunPlan()
	{		
		if (m_currentPlanStep < m_plan.Size())
		{
			m_currentState = HTNAgentState::RunningPlan;
			const HTNTask& newTask = HTNManager::s_definedTasks[m_plan[m_currentPlanStep]];
			m_currentOperatorType = newTask.taskOperatorType;
			HTNManager::s_definedOperators.Get(m_currentOperatorType).OperatorStartFunc(this);
		}
		else
		{
			RequestPlan();
		}
	}

	void HTNAgent::StopPlan()
	{
		if (m_currentOperatorType < HTNOperatorType::Count)
		{
			HTNManager::s_definedOperators.Get(m_currentOperatorType).OperatorInterruptFunc(this);
			m_currentOperatorType = HTNOperatorType::Count;
			m_currentPlanStep = 0;
			m_currentState = HTNAgentState::Idle;
			m_plan.Clear();
		}
	}

	void HTNAgent::RequestPlan()
	{
		m_requestPlanTask.Wait();
		m_currentState = HTNAgentState::WaitingForPlan;

		EventManager::RegisterHandler("RunPlannerFinished", BL_BIND_OBJECT_FUNCTION(HTNAgent::OnRunPlannerFinished));

		Threading::TaskScheduler::KickSingleTask(m_requestPlanTask, 
			[this] {
				BL_PROFILE_SCOPE("HTN Planner", Optick::Category::GameLogic);
				m_validPlan = HTNManager::RunPlanner(m_worldStateBlackboardID, m_currentDomainID, m_plan);

				EventEntry runPlannerFinished;

				runPlannerFinished.eventID = BL_STRING_TO_STRINGID("RunPlannerFinished");
				runPlannerFinished.senderID = m_agentIdx;

				runPlannerFinished.eventData1 = m_validPlan;

				EventManager::QueueEvent(runPlannerFinished);				
			});
	}

	void HTNAgent::OnOperatorFinished(HTNOperatorType typeFinished, bool success)
	{		
		if (typeFinished == m_currentOperatorType)
		{			
			if (success)
			{
				const HTNTask& newTask = HTNManager::s_definedTasks[m_plan[m_currentPlanStep]];

				HTNWorldStateBlackboard* worldStateBlackboard = nullptr;
				if (HTNManager::s_definedWorldStateBlackboards.TryGet(m_worldStateBlackboardID, worldStateBlackboard))
				{
					newTask.ApplyEffects(*worldStateBlackboard);
				}
				++m_currentPlanStep;
				RunPlan();
			}
			else
			{
				RequestPlan();			
			}
		}
	}

	bool HTNAgent::OnRunPlannerFinished(const EventEntry& e)
	{
		if ((i32)e.senderID != m_agentIdx)
		{
			return false;
		}

		EventManager::UnregisterHandler("RunPlannerFinished", BL_BIND_OBJECT_FUNCTION(HTNAgent::OnRunPlannerFinished));

		if ((bool)e.eventData1) //Valid plan?
		{
			RunPlan();
		}
		else
		{
			m_currentState = HTNAgentState::Idle;
		}

		return true;
	}

	void HTNAgent::SetCurrentDomain(StringId domainID)
	{
		StopPlan();
		m_currentDomainID = domainID;
	}

}