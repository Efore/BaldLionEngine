#pragma once
#include "BaldLion/AI/HTN/HTNTypes.h"
#include "BaldLion/AI/HTN/HTNManager.h"
#include "BaldLion/AI/HTN/HTNAgent.h"
#include "BaldLion/AI/Navigation/NavigationManager.h"
#include "BaldLion/ECS/ECSManager.h"
#include "BaldLion/Core/EventManager.h"

namespace BaldLion::AI::HTN
{
	struct HTNOperatorMoveTo
	{
		inline static DynamicArray<i32> AgentsInOperator;

		static void Init()
		{
			AgentsInOperator = DynamicArray<i32>(AllocationType::FreeList_Main, 16);
		}

		static void Stop()
		{
			AgentsInOperator.Delete();
		}

		static void OperatorStartFunc(HTNAgent* htnAgent)
		{
			AgentsInOperator.EmplaceBack(htnAgent->GetAgentId());
			if (AgentsInOperator.Size() == 1)
			{
				EventManager::RegisterHandler("MoveToFinished", BL_BIND_STATIC_FUNCTION(HTNOperatorMoveTo::OnMoveToFinished));
			}

			//TODO Find a way to get the nav agent from the htn Agent
		}

		static void OperatorInterruptFunc(HTNAgent* htnAgent)
		{
			//TODO Stop the agent

			AgentsInOperator.RemoveFast(htnAgent->GetAgentId());
			if (AgentsInOperator.Size() == 0)
			{
				EventManager::UnregisterHandler("MoveToFinished", BL_BIND_STATIC_FUNCTION(HTNOperatorMoveTo::OnMoveToFinished));
			}

			htnAgent->OnOperatorFinished(HTNOperatorType::MoveTo, false);
		}

		static bool OnMoveToFinished(const EventEntry& e)
		{
			const i32 navMeshAgentID = e.eventData1;
			//TODO Get Agent's nav agent id
			const i32 htnAgentID = 0;

			if (true) //TODO if navmesh agent id is the same one as the one the htn has
			{
				const i32 agentIDIndex = AgentsInOperator.FindIndex(htnAgentID);

				if (agentIDIndex >= 0)
				{				
					AgentsInOperator.RemoveAtFast(agentIDIndex);
					if (AgentsInOperator.Size() == 0)
					{
						EventManager::UnregisterHandler("MoveToFinished", BL_BIND_STATIC_FUNCTION(HTNOperatorMoveTo::OnMoveToFinished));
					}

					HTNAgent& htnAgent = HTNManager::s_agents[htnAgentID];

					const Navigation::MoveToResult result = (Navigation::MoveToResult)(ui8)e.eventData2;

					htnAgent.OnOperatorFinished(HTNOperatorType::MoveTo, result == Navigation::Sucess);

					return true;
				}
			}

			return false;
		}	

	};
}
