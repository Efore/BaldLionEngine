#include "blpch.h"
#include "ECSHTNPlannerSystem.h"

#include "BaldLion/ECS/Components/ECSHTNAgentComponent.h"
#include "BaldLion/AI/HTN/HTNManager.h"

using namespace BaldLion::AI::HTN;
namespace BaldLion::ECS
{

	ECSHTNPlannerSystem::ECSHTNPlannerSystem(const char* systemName, ECSSystemType systemType, const ECSSignature& signature, class ECSManager* ecsManager):
		ECSSystem(systemName, systemType, signature, ecsManager, false) 
	{

	}


	void ECSHTNPlannerSystem::UpdateComponents(ECSEntityID entityID, ECSComponentLookUp* componentLookUp, float deltaTime)
	{
		BL_PROFILE_FUNCTION();
		const ECSHTNAgentComponent* htnAgentComponent = componentLookUp->Read<ECSHTNAgentComponent>(ECSComponentType::HTNAgent);
		if (HTNManager::s_agents[htnAgentComponent->htnAgentIdx].m_currentState == HTNAgent::HTNAgentState::Idle)
		{
			HTNManager::s_agents[htnAgentComponent->htnAgentIdx].RequestPlan();
		}
	}
}
