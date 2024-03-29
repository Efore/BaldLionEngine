#include "blpch.h"
#include "ECSNavigationSystem.h"

#include "BaldLion/ECS/Components/ECSLocomotionComponent.h"
#include "BaldLion/ECS/Components/ECSNavMeshAgentComponent.h"

#include "BaldLion/Utils/MathUtils.h"
#include "BaldLion/AI/Navigation/NavigationManager.h"

namespace BaldLion 
{
	using namespace AI::Navigation;
	namespace ECS 
	{

		ECSNavigationSystem::ECSNavigationSystem(const char* systemName, ECSSystemType systemType, const ECSSignature & signature, class ECSManager* ecsManager) :
				ECSSystem(systemName, systemType, signature, ecsManager, true) 
		{
				
		}		

		void ECSNavigationSystem::OnUpdate(float deltaTime)
		{
			BL_PROFILE_FUNCTION();
			NavigationManager::Update(deltaTime);
			ECSSystem::OnUpdate(deltaTime);
		}

		void ECSNavigationSystem::UpdateComponents(ECSEntityID entityID, ECSComponentLookUp* componentLookUp, float deltaTime)
		{
			BL_PROFILE_FUNCTION();

			ECSLocomotionComponent* locomotionComponent = componentLookUp->Write<ECSLocomotionComponent>(ECSComponentType::Locomotion);
			const ECSNavMeshAgentComponent* navMeshAgent = componentLookUp->Read<ECSNavMeshAgentComponent>(ECSComponentType::NavMeshAgent);

			i32 agentIdx = navMeshAgent->crowdAgentIdx;
			if (agentIdx != -1)
			{
				const dtCrowdAgent* agent = NavigationManager::GetCrowdAgent(agentIdx);			
				
				locomotionComponent->desiredVelocity = *(glm::vec3*)agent->nvel;
				locomotionComponent->maxAcceleration = navMeshAgent->agentMaxAcceleration;

				NavigationManager::UpdateCrowdAgent(agentIdx, navMeshAgent->agentMaxSpeed, navMeshAgent->agentMaxAcceleration);
			}
		}
	}
}
