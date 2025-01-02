#include "blpch.h"
#include "HTNManager.h"

namespace BaldLion::AI::HTN
{
	DynamicArray<HTNTask> HTNManager::s_DefinedTasks;	
	HashMap<StringId, HTNDomain> HTNManager::s_DefinedDomains;

	void HTNManager::Init()
	{
		s_DefinedTasks = DynamicArray<HTNTask>(AllocationType::FreeList_Main, 256);
		s_DefinedDomains = HashMap<StringId, HTNDomain>(AllocationType::FreeList_Main, 8);
	}

	void HTNManager::Stop()
	{

	}

	bool HTNManager::RunPlanner(const HTNWorldStateBlackboard& worldStateBlackboard,
		const HTNDomain& domain, DynamicArray<ui32>& plan)
	{
		HTNWorldStateBlackboard tempWorldStateBlackboard(AllocationType::Linear_Frame, worldStateBlackboard);
		DynamicArray<HTNWorldStateProperty> prevValues(AllocationType::Linear_Frame, 64);

		return CheckTask(tempWorldStateBlackboard, prevValues, domain.mainTask, plan);
	}

	bool HTNManager::CheckTask(HTNWorldStateBlackboard& tempWorldStateBlackboard, 
		DynamicArray<HTNWorldStateProperty>& prevBlackboardValues,
		ui32 taskIndex, 
		DynamicArray<ui32>& plan)
	{
		const HTNTask& task = s_DefinedTasks[taskIndex];
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
							const HTNTask& lastTask = s_DefinedTasks[lastTaskIdx];

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
}