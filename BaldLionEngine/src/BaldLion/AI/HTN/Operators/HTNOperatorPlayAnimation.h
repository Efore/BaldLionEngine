#pragma once
#include "BaldLion/AI/HTN/HTNTypes.h"
#include "BaldLion/AI/HTN/HTNManager.h"
#include "BaldLion/AI/HTN/HTNAgent.h"
#include "BaldLion/AI/Navigation/NavigationManager.h"
#include "BaldLion/SceneManagement/SceneManager.h"
#include "BaldLion/ECS/ECSManager.h"
#include "BaldLion/ECS/ECSComponentLookUp.h"
#include "BaldLion/ECS/Components/ECSHTNAgentComponent.h"
#include "BaldLion/ECS/Components/ECSAnimationComponent.h"
#include "BaldLion/Core/EventManager.h"

using namespace BaldLion::ECS;

namespace BaldLion::AI::HTN
{
	struct HTNOperatorPlayAnimation
	{
		inline static DynamicArray<ui32> HTNAgentsIdxInOperator;		
		inline static DynamicArray<ECSEntityID> ECSEntityIDInOperator;

		static void Init()
		{
			HTNAgentsIdxInOperator = DynamicArray<ui32>(AllocationType::FreeList_Main, 16);
			ECSEntityIDInOperator = DynamicArray<ECSEntityID>(AllocationType::FreeList_Main, 16);
		}

		static void Stop()
		{
			HTNAgentsIdxInOperator.Delete();
			ECSEntityIDInOperator.Delete();
		}

		static void OperatorStartFunc(HTNAgent* htnAgent, const Variant* operatorData)
		{
			const ui32 htnAgentIdx = htnAgent->GetAgentIdx();

			ECSEntityID entityID = -1;

			const DynamicArray<ECS::ECSHTNAgentComponent>* htnAgentComponents =
				SceneManagement::SceneManager::GetECSManager()->GetComponentPool<ECS::ECSHTNAgentComponent>(ECS::ECSComponentType::HTNAgent);

			//We iterate through all htnAgentComponents to find the operating agent
			BL_DYNAMICARRAY_FOREACH(*htnAgentComponents)
			{
				if ((*htnAgentComponents)[i].htnAgentIdx == htnAgentIdx)
				{
					entityID = (*htnAgentComponents)[i].GetOwnerEntityID();
					break;
				}
			}

			//If a nav agent was found
			if (entityID >= 0)
			{
				//If found, we get the component look up of that entity and get the animation component from there
				ECS::ECSComponentLookUp* entityComponentLookUp = nullptr;
				if (SceneManagement::SceneManager::GetECSManager()->GetEntityComponents().
					TryGet(entityID, entityComponentLookUp))
				{
					ECS::ECSAnimationComponent* animationComponent =
						entityComponentLookUp->Write<ECS::ECSAnimationComponent>(ECS::ECSComponentType::Animation);

					if (animationComponent != nullptr)
					{
						//We apply the chosen one shot animation id
						animationComponent->currentOneShotAnimationID = (ui32)operatorData[0];

						//Store both htnagent idx and entityID		
						HTNAgentsIdxInOperator.EmplaceBack(htnAgentIdx);
						ECSEntityIDInOperator.EmplaceBack(entityID);

						if (HTNAgentsIdxInOperator.Size() == 1)
						{
							EventManager::RegisterEventHandler("OneShotAnimationFinished", BL_BIND_STATIC_FUNCTION(HTNOperatorPlayAnimation::OnOneShotAnimationFinished));
						}

						//Success and early out
						return;
					}
					
				}
			}

			//At this point, the start function has failed so we interrupt it
			OperatorInterruptFunc(htnAgent, operatorData);
		}

		static void OperatorInterruptFunc(HTNAgent* htnAgent, const Variant* operatorData)
		{
			const i32 agentIdxInOperator = HTNAgentsIdxInOperator.FindIndex(htnAgent->GetAgentIdx());

			const ECSEntityID entityID = ECSEntityIDInOperator[agentIdxInOperator];

			HTNAgentsIdxInOperator.RemoveAtFast(agentIdxInOperator);
			ECSEntityIDInOperator.RemoveAtFast(agentIdxInOperator);

			if (HTNAgentsIdxInOperator.Size() == 0)
			{
				EventManager::UnregisterEventHandler("OneShotAnimationFinished", BL_BIND_STATIC_FUNCTION(HTNOperatorPlayAnimation::OnOneShotAnimationFinished));
			}

			htnAgent->OnOperatorFinished(HTNOperatorType::PlayAnimation, false);
		}

		static bool OnOneShotAnimationFinished(const EventEntry& e)
		{
			const i32 agentIdxInOperator = ECSEntityIDInOperator.FindIndex(e.eventData1);

			if (agentIdxInOperator >= 0)
			{
				const ui32 htnAgentIdx = HTNAgentsIdxInOperator[agentIdxInOperator];

				HTNAgentsIdxInOperator.RemoveAtFast(agentIdxInOperator);
				ECSEntityIDInOperator.RemoveAtFast(agentIdxInOperator);

				if (HTNAgentsIdxInOperator.Size() == 0)
				{
					EventManager::UnregisterEventHandler("OneShotAnimationFinished", BL_BIND_STATIC_FUNCTION(HTNOperatorPlayAnimation::OnOneShotAnimationFinished));
				}

				HTNAgent& htnAgent = HTNManager::s_agents[htnAgentIdx];
				htnAgent.OnOperatorFinished(HTNOperatorType::PlayAnimation, true);

				return true;

			}

			return false;
		}

	};
}
#pragma once
