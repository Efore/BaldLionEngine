#include "blpch.h"
#include "HTNManager.h"

namespace BaldLion::AI::HTN
{
	HashMap<StringId, HTNDomain> HTNManager::s_definedDomains;
	HashMap <HTNOperatorType, HTNOperator> HTNManager::s_definedOperators;
	DynamicArray<HTNTask> HTNManager::s_definedTasks;	
	DynamicArray<HTNAgent> HTNManager::s_agents;

	void HTNManager::Init()
	{
		s_definedDomains = HashMap<StringId, HTNDomain>(AllocationType::FreeList_Main, 8);
		s_definedOperators = HashMap<HTNOperatorType, HTNOperator>(AllocationType::FreeList_Main, 128);
		s_definedTasks = DynamicArray<HTNTask>(AllocationType::FreeList_Main, 256);
		s_agents = DynamicArray<HTNAgent>(AllocationType::FreeList_Main, 16);
	}

	void HTNManager::Stop()
	{
		s_definedTasks.Delete();
		s_agents.Delete();
		s_definedDomains.Delete();
		s_definedOperators.Delete();
	}

	bool HTNManager::RunPlanner(const HTNWorldStateBlackboard& worldStateBlackboard,
		StringId domainID, DynamicArray<ui32>& plan)
	{
		HTNWorldStateBlackboard tempWorldStateBlackboard(AllocationType::Linear_Frame, worldStateBlackboard);
		DynamicArray<HTNWorldStateProperty> prevValues(AllocationType::Linear_Frame, 64);
		const HTNDomain& domain = s_definedDomains.Get(domainID);

		return CheckTask(tempWorldStateBlackboard, prevValues, domain.mainTask, plan);
	}

	bool HTNManager::CheckTask(HTNWorldStateBlackboard& tempWorldStateBlackboard, 
		DynamicArray<HTNWorldStateProperty>& prevBlackboardValues,
		ui32 taskIndex, 
		DynamicArray<ui32>& plan)
	{
		const HTNTask& task = s_definedTasks[taskIndex];
		if (task.type == HTNTask::PrimitiveTask)
		{
			if (task.methods[0].CheckConditions(tempWorldStateBlackboard))
			{			
				//We need to keep track of the world state values before being modified in case of backtracking
				//so, we add the prev value to a list
				BL_DYNAMICARRAY_FOREACH(task.effects)
				{					
					Variant blackboardEntry;
					if (tempWorldStateBlackboard.TryGet(task.effects[0].blackboardKey, blackboardEntry))
					{
						HTNWorldStateProperty prevValue{ task.effects[0].blackboardKey, blackboardEntry };
						prevBlackboardValues.EmplaceBack(prevValue);
					}
				}

				task.ApplyEffects(tempWorldStateBlackboard);
				plan.EmplaceBack(taskIndex);

				return true;
			}
		}
		else
		{
			BL_DYNAMICARRAY_FOREACH(task.methods)
			{
				const HTNMethod& method = task.methods[i];

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
								const HTNWorldStateProperty& prevValue = prevBlackboardValues.Back();

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

	const HTNTask& HTNManager::GetTask(ui32 taskIndex)
	{
		BL_ASSERT_LOG(taskIndex < s_definedTasks.Size(), "Task index out of bounds");
		return s_definedTasks[taskIndex];
	}

	const HTNOperator& HTNManager::GetOperator(HTNOperatorType operatorType)
	{
		return s_definedOperators.Get(operatorType);
	}

	void HTNManager::ThrowOperatorEvent(GameplayEvent& e)
	{
		
	}

}