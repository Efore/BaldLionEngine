#include "blpch.h"
#include "HTNManager.h"
#include "Serialization/HTNSerializer.h"
#include "Operators/HTNOperatorMoveTo.h"
#include "Operators/HTNOperatorPlayAnimation.h"

namespace BaldLion::AI::HTN
{
	HashMap <HTNOperatorType, HTNOperator> HTNManager::s_definedOperators;
	HashMap<StringId, HTNDomain> HTNManager::s_definedDomains;
	DynamicArray<HTNTask> HTNManager::s_definedTasks;	
	DynamicArray<HTNAgent> HTNManager::s_agents;
	HashMap<StringId, HTNWorldStateBlackboard> HTNManager::s_definedWorldStateBlackboards;

	void HTNManager::Init()
	{
		s_definedOperators = HashMap<HTNOperatorType, HTNOperator>(AllocationType::FreeList_Main, 128);
		s_definedDomains = HashMap<StringId, HTNDomain>(AllocationType::FreeList_Main, 8);
		s_definedTasks = DynamicArray<HTNTask>(AllocationType::FreeList_Main, 256);
		s_agents = DynamicArray<HTNAgent>(AllocationType::FreeList_Main, 16);
		s_definedWorldStateBlackboards = HashMap<StringId, HTNWorldStateBlackboard>(AllocationType::FreeList_Main, 16);

		HTNOperatorMoveTo::Init();
		s_definedOperators.Emplace(HTNOperatorType::MoveTo, { HTNOperatorMoveTo::OperatorStartFunc ,  HTNOperatorMoveTo::OperatorInterruptFunc });

		HTNOperatorPlayAnimation::Init();
		s_definedOperators.Emplace(HTNOperatorType::PlayAnimation, { HTNOperatorPlayAnimation::OperatorStartFunc ,  HTNOperatorPlayAnimation::OperatorInterruptFunc });

		HTNSerializer::DeserializeHTNData();
	}

	void HTNManager::Stop()
	{
		s_definedTasks.Delete();
		s_agents.Delete();
		s_definedDomains.Delete();
		s_definedOperators.Delete();

		HTNOperatorMoveTo::Stop();
	}

	i32 HTNManager::RequestAgent(StringId worldStateBlackboardId, StringId domainId)
	{
		BL_DYNAMICARRAY_FOR(i, s_agents, 0)
		{
			if (!s_agents[i].m_isActive)
			{
				s_agents[i].m_worldStateBlackboardID = worldStateBlackboardId;
				s_agents[i].m_currentDomainID = domainId;
				s_agents[i].m_isActive = true;
				return i;
			}
		}

		HTNAgent* newAgent = s_agents.EmplaceBack(worldStateBlackboardId, domainId, s_agents.Size());
		newAgent->m_isActive = true;
		return s_agents.Size() - 1;
	}

	void HTNManager::DeactivateAgent(i32 agentIdx)
	{
		if (s_agents[agentIdx].m_isActive)
		{
			s_agents[agentIdx].m_isActive = false;
			s_agents[agentIdx].m_currentState = HTNAgent::HTNAgentState::Idle;
		}
	}

	bool HTNManager::RunPlanner(StringId worldStateBlackboardId,
		StringId domainID, DynamicArray<ui32>& plan)
	{
		HTNWorldStateBlackboard* worldStateBlackboard = nullptr;
		if (s_definedWorldStateBlackboards.TryGet(worldStateBlackboardId, worldStateBlackboard))
		{
			HTNWorldStateBlackboard tempWorldStateBlackboard(AllocationType::Linear_Frame, *worldStateBlackboard);
			DynamicArray<HTNWorldStateEffect> prevValues(AllocationType::Linear_Frame, 64);
			const HTNDomain& domain = s_definedDomains.Get(domainID);

			return CheckTask(tempWorldStateBlackboard, prevValues, domain.mainTask, plan);
		}
		return false;
	}

	bool HTNManager::CheckTask(HTNWorldStateBlackboard& tempWorldStateBlackboard, 
		DynamicArray<HTNWorldStateEffect>& prevBlackboardValues,
		ui32 taskIndex, 
		DynamicArray<ui32>& plan)
	{
		const HTNTask& htnTask = s_definedTasks[taskIndex];
		if (htnTask.taskType == HTNTask::PrimitiveTask)
		{					
			//We need to keep track of the world state values before being modified in case of backtracking.
			//Therefore, we add the prev value to a list
			BL_DYNAMICARRAY_FOREACH(htnTask.effects)
			{					
				Variant blackboardEntry;
				if (tempWorldStateBlackboard.TryGet(htnTask.effects[0].blackboardKey, blackboardEntry))
				{
					HTNWorldStateEffect prevValue{ blackboardEntry, htnTask.effects[0].blackboardKey };
					prevBlackboardValues.EmplaceBack(prevValue);
				}
			}

			htnTask.ApplyEffects(tempWorldStateBlackboard);
			plan.EmplaceBack(taskIndex);

			return true;			
		}
		else
		{
			BL_DYNAMICARRAY_FOREACH(htnTask.methods)
			{
				const HTNMethod& method = htnTask.methods[i];

				if (method.CheckConditions(tempWorldStateBlackboard))
				{
					bool methodSuccessful = true;
					const ui32 currentPlanSize = plan.Size();

					for (ui32 j = 0; j < method.subtasksIndices.Size() && methodSuccessful; ++j)
					{
						methodSuccessful &= CheckTask(tempWorldStateBlackboard, prevBlackboardValues, method.subtasksIndices[j], plan);
					}

					if (methodSuccessful)
					{
						return true;
					}
					else
					{
						//Backtracking: removing plans added in this method
						while (plan.Size() > currentPlanSize)
						{							
							const ui32 lastTaskIdx = plan.Back();
							const HTNTask& lastTask = s_definedTasks[lastTaskIdx];

							//Restoring temp worldstate's prevs values applied with the task that is leaving the plan
							BL_DYNAMICARRAY_FOREACH(lastTask.effects)
							{
								const HTNWorldStateEffect& prevValue = prevBlackboardValues.Back();

								Variant& value = tempWorldStateBlackboard.Get(prevValue.blackboardKey);
								value = prevValue.blackboardValue;
								
								prevBlackboardValues.PopBack();
							}

							plan.PopBack();
						}
					}
				}
			}
		}

		return false;
	}

	void HTNManager::Serialize()
	{
		HTNSerializer::SerializeHTNData();
	}

}