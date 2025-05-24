#include "blpch.h"
#include "HTNAgent.h"
#include "HTNManager.h"

namespace BaldLion::AI::HTN
{
	HTNAgent::HTNAgent(StringId domainID, i32 agentID) : currentDomainId(domainID), m_agentID(agentID)
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
			const HTNTask& newTask = HTNManager::s_definedTasks[m_plan[m_currentPlanStep]];
			currentOperatorType = newTask.taskOperatorType;
			HTNManager::s_definedOperators.Get(currentOperatorType).OperatorStartFunc(this);
		}
		else
		{
			RequestPlan();
		}
	}

	void HTNAgent::StopPlan()
	{
		if (currentOperatorType < HTNOperatorType::Count)
		{
			HTNManager::s_definedOperators.Get(currentOperatorType).OperatorInterruptFunc(this);
			currentOperatorType = HTNOperatorType::Count;
			m_currentPlanStep = 0;
			m_plan.Clear();
		}
	}

	void HTNAgent::RequestPlan()
	{
		m_requestPlanTask.Wait();
		EventManager::RegisterHandler("RunPlannerFinished", BL_BIND_OBJECT_FUNCTION(HTNAgent::OnRunPlannerFinished));
		Threading::TaskScheduler::KickSingleTask(m_requestPlanTask, 
			[this] {
				BL_PROFILE_SCOPE("HTN Planner", Optick::Category::GameLogic);
				m_validPlan = HTNManager::RunPlanner(m_worldStateBlackboardIndex, currentDomainId, m_plan);

				EventEntry runPlannerFinished;

				runPlannerFinished.eventID = BL_STRING_TO_STRINGID("RunPlannerFinished");
				runPlannerFinished.senderID = m_agentID;

				runPlannerFinished.eventData1 = m_validPlan;

				EventManager::QueueEvent(runPlannerFinished);				
			});
	}

	void HTNAgent::OnOperatorFinished(HTNOperatorType typeFinished, bool success)
	{		
		if (typeFinished == currentOperatorType)
		{			
			if (success)
			{
				const HTNTask& newTask = HTNManager::s_definedTasks[m_plan[m_currentPlanStep]];
				newTask.ApplyEffects(HTNManager::s_definedWorldStateBlackboards[m_worldStateBlackboardIndex]);
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
		if ((i32)e.senderID != m_agentID)
		{
			return false;
		}

		EventManager::UnregisterHandler("RunPlannerFinished", BL_BIND_OBJECT_FUNCTION(HTNAgent::OnRunPlannerFinished));

		if ((bool)e.eventData1) //Valid plan?
		{
			RunPlan();
		}

		return true;
	}

}