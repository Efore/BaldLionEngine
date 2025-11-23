#pragma once
#include "BaldLion/AI/HTN/HTNTypes.h"
#include "BaldLion/AI/HTN/HTNManager.h"
#include "BaldLion/AI/HTN/HTNAgent.h"
#include "BaldLion/AI/Navigation/NavigationManager.h"
#include "BaldLion/SceneManagement/SceneManager.h"
#include "BaldLion/ECS/ECSManager.h"
#include "BaldLion/ECS/ECSComponentLookUp.h"
#include "BaldLion/ECS/Components/ECSHTNAgentComponent.h"
#include "BaldLion/ECS/Components/ECSNavMeshAgentComponent.h"
#include "BaldLion/Core/EventManager.h"

namespace BaldLion::AI::HTN
{
	struct HTNOperatorMoveTo
	{
		inline static DynamicArray<ui32> HTNAgentsIdxInOperator;
		inline static DynamicArray<ui32> NavAgentsIdxInOperator;

		static void Init()
		{
			HTNAgentsIdxInOperator = DynamicArray<ui32>(AllocationType::FreeList_Main, 16);
			NavAgentsIdxInOperator = DynamicArray<ui32>(AllocationType::FreeList_Main, 16);
		}

		static void Stop()
		{
			HTNAgentsIdxInOperator.Delete();
			NavAgentsIdxInOperator.Delete();
		}

		static void OperatorStartFunc(HTNAgent* htnAgent)
		{
			const ui32 htnAgentIdx = htnAgent->GetAgentIdx();

			i32 navAgentIdx = -1;

			const DynamicArray<ECS::ECSHTNAgentComponent>* htnAgentComponents =
				SceneManagement::SceneManager::GetECSManager()->GetComponentPool<ECS::ECSHTNAgentComponent>(ECS::ECSComponentType::HTNAgent);

			//We iterate through all htnAgentComponents to find the operating agint
			BL_DYNAMICARRAY_FOREACH(*htnAgentComponents)
			{
				if ((*htnAgentComponents)[i].htnAgentIdx == htnAgentIdx)
				{
					//If found, we get the component look up of that entity and get the nav mesh agent component from there
					ECS::ECSComponentLookUp* entityComponentLookUp = nullptr;
					if (SceneManagement::SceneManager::GetECSManager()->GetEntityComponents().
						TryGet((*htnAgentComponents)[i].GetOwnerEntityID(), entityComponentLookUp))
					{
						const ECS::ECSNavMeshAgentComponent* navMeshAgentComponent = 
							entityComponentLookUp->Read<ECS::ECSNavMeshAgentComponent>(ECS::ECSComponentType::NavMeshAgent);

						if (navMeshAgentComponent != nullptr)
						{
							navAgentIdx = navMeshAgentComponent->crowdAgentIdx;
						}
					}
					break;
				}
			}

			//If a nav agent was found
			if (navAgentIdx >= 0)
			{
				//Store it together with the htn index
				NavAgentsIdxInOperator.EmplaceBack(navAgentIdx);
				HTNAgentsIdxInOperator.EmplaceBack(htnAgentIdx);

				if (HTNAgentsIdxInOperator.Size() == 1)
				{
					EventManager::RegisterEventHandler("MoveToFinished", BL_BIND_STATIC_FUNCTION(HTNOperatorMoveTo::OnMoveToFinished));
				}

				//Get the move to target from the blackboard and request a move to it
				const HTNWorldStateBlackboard& agentBlackboard = HTNManager::s_definedWorldStateBlackboards.Get(htnAgent->m_worldStateBlackboardID);

				const Variant& movetoTarget = agentBlackboard.Get(BL_STRING_TO_STRINGID("MoveToTarget"));

				AI::Navigation::NavigationManager::RequestMoveTo(navAgentIdx, movetoTarget);
			}
		}

		static void OperatorInterruptFunc(HTNAgent* htnAgent)
		{			
			const i32 agentIdxInOperator = HTNAgentsIdxInOperator.FindIndex(htnAgent->GetAgentIdx());

			const ui32 navAgentIdx = NavAgentsIdxInOperator[agentIdxInOperator];

			HTNAgentsIdxInOperator.RemoveAtFast(agentIdxInOperator);
			NavAgentsIdxInOperator.RemoveAtFast(agentIdxInOperator);

			if (HTNAgentsIdxInOperator.Size() == 0)
			{
				EventManager::UnregisterEventHandler("MoveToFinished", BL_BIND_STATIC_FUNCTION(HTNOperatorMoveTo::OnMoveToFinished));
			}

			AI::Navigation::NavigationManager::InterruptMoveTo(navAgentIdx);

			htnAgent->OnOperatorFinished(HTNOperatorType::MoveTo, false);
		}

		static bool OnMoveToFinished(const EventEntry& e)
		{			
			const i32 agentIdxInOperator = NavAgentsIdxInOperator.FindIndex(e.eventData1);

			if (agentIdxInOperator >= 0)
			{			
				const ui32 htnAgentIdx = HTNAgentsIdxInOperator[agentIdxInOperator];

				HTNAgentsIdxInOperator.RemoveAtFast(agentIdxInOperator);
				NavAgentsIdxInOperator.RemoveAtFast(agentIdxInOperator);

				if (HTNAgentsIdxInOperator.Size() == 0)
				{
					EventManager::UnregisterEventHandler("MoveToFinished", BL_BIND_STATIC_FUNCTION(HTNOperatorMoveTo::OnMoveToFinished));
				}

				HTNAgent& htnAgent = HTNManager::s_agents[htnAgentIdx];

				const Navigation::MoveToResult result = (Navigation::MoveToResult)(ui8)e.eventData2;

				htnAgent.OnOperatorFinished(HTNOperatorType::MoveTo, result == Navigation::Success);

				return true;
				
			}

			return false;
		}	

	};
}
