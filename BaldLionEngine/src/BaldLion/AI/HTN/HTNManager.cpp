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
		return CheckTask(tempWorldStateBlackboard, domain.mainTask, plan);
	}

	bool HTNManager::CheckTask(HTNWorldStateBlackboard& worldStateBlackboard,
		ui32 taskIndex, DynamicArray<ui32>& plan)
	{
		const HTNTask& task = s_DefinedTasks[taskIndex];
		if (task.type == HTNTask::PrimitiveTask)
		{
			if (task.methods[0].CheckConditions(worldStateBlackboard))
			{
				task.ApplyEffects(worldStateBlackboard);
				plan.EmplaceBack(taskIndex);

				return true;
			}
		}
		else
		{
			BL_DYNAMICARRAY_FOREACH(task.methods)
			{
				const HTNMethod& method = task.methods[i];

				if (method.CheckConditions(worldStateBlackboard))
				{
					bool methodSuccessful = true;
					const ui32 currentPlanSize = plan.Size();

					for (ui32 j = 0; j < method.subtasksIndices.Size() && methodSuccessful; ++j)
					{
						methodSuccessful &= CheckTask(worldStateBlackboard, method.subtasksIndices[j], plan);
					}

					if (methodSuccessful)
					{
						return true;
					}
					else
					{
						while (plan.Size() > currentPlanSize)
						{
							plan.PopBack();
						}
					}
				}
			}
		}

		return false;
	}
}