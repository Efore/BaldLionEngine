#include "blpch.h"
#include "HTNAgent.h"
#include "HTNManager.h"

namespace BaldLion::AI::HTN
{
	HTNAgent::HTNAgent(StringId domainID, i32 agentID) : currentDomainId(domainID), m_agentID(agentID)
	{
		m_plan = DynamicArray<ui32>(AllocationType::FreeList_Main, 32);
		m_worldStateBlackboard = HashTable<StringId, Variant>(AllocationType::FreeList_Main, 32);
	}

	HTNAgent::~HTNAgent()
	{
		m_requestPlanTask.Wait();
		m_plan.Delete();
		m_worldStateBlackboard.Delete();
	}

	void HTNAgent::AdvancePlan()
	{
		const HTNTask& newTask = HTNManager::GetTask(m_currentPlanStep);
		newTask.ApplyEffects(m_worldStateBlackboard);

		++m_currentPlanStep;
		if (m_currentPlanStep < m_plan.Size())
		{
			const HTNTask& newTask = HTNManager::GetTask(m_currentPlanStep);
			currentOperatorType = newTask.operatorType;
			HTNManager::GetOperator(currentOperatorType).OperatorStartFunc(this);			
		}
		else
		{
			m_currentPlanStep = 0;
			RunPlan();
		}
	}

	void HTNAgent::RunPlan()
	{		
		const HTNTask& newTask = HTNManager::GetTask(m_currentPlanStep);
		currentOperatorType = newTask.operatorType;
		HTNManager::GetOperator(currentOperatorType).OperatorStartFunc(this);		
	}

	void HTNAgent::StopPlan()
	{
		if (currentOperatorType < HTNOperatorType::Count)
		{
			HTNManager::GetOperator(currentOperatorType).OperatorInterruptFunc(this);
			currentOperatorType = HTNOperatorType::Count;
			m_plan.Clear();
		}
	}

	void HTNAgent::RequestPlan()
	{
		m_requestPlanTask.Wait();
		Threading::TaskScheduler::KickSingleTask(m_requestPlanTask, 
			[this] {
				BL_PROFILE_SCOPE("HTN Planner", Optick::Category::GameLogic);
				m_validPlan = HTNManager::RunPlanner(m_worldStateBlackboard, currentDomainId, m_plan);
			}, 
			[this] {
				if (m_validPlan) RunPlan();
			});
	}

}