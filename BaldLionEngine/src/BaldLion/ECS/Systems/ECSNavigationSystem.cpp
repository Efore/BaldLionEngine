#include "blpch.h"
#include "ECSNavigationSystem.h"

#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/ECS/Components/ECSNavMeshAgentComponent.h"

#include "BaldLion/Utils/MathUtils.h"
#include "BaldLion/AI/Navigation/NavigationManager.h"

namespace BaldLion 
{
	using namespace AI::Navigation;
	namespace ECS 
	{

		ECSNavigationSystem::ECSNavigationSystem(const char* systemName, const ECSSignature & signature, class ECSManager* ecsManager) :
				ECSSystem(systemName, signature, ecsManager, true, true) 
		{
				
		}		

		void ECSNavigationSystem::OnUpdate()
		{
			NavigationManager::Update();

			ECSSystem::OnUpdate();
		}

		void ECSNavigationSystem::UpdateComponents(ECSComponentLookUp* componentLookUp)
		{
			ECSTransformComponent* transform = componentLookUp->Write<ECSTransformComponent>(ECSComponentType::Transform);
			const ECSNavMeshAgentComponent* navMeshAgent = componentLookUp->Read<ECSNavMeshAgentComponent>(ECSComponentType::NavMeshAgent);

			i32 agentIdx = navMeshAgent->crowdAgentIdx;
			if (agentIdx != -1)
			{
				const dtCrowdAgent* agent = NavigationManager::GetCrowdAgent(agentIdx);
				transform->position = *(glm::vec3*)agent->npos;
			}
		}
	}
}
