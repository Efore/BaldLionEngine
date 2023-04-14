#pragma once

#include "BaldLion/ECS/ECSComponent.h"
#include "BaldLion/AI/Navigation/NavigationManager.h"

namespace BaldLion 
{
	namespace ECS
	{
		class ECSNavMeshAgentComponent : public ECSComponent {

		public:

			ECSNavMeshAgentComponent(const glm::vec3& startPosition) :
				ECSComponent(ECSComponentType::NavMeshAgent), agentMaxSpeed(3.5f), agentMaxAcceleration(8.0f)
			{
				crowdAgentIdx = AI::Navigation::NavigationManager::CreateCrowdAgent(startPosition, agentMaxSpeed, agentMaxAcceleration);
			}

			~ECSNavMeshAgentComponent() 
			{
				AI::Navigation::NavigationManager::DestroyCrowdAgent(crowdAgentIdx);
			}
		public:	
			
			i32 crowdAgentIdx;
			float agentMaxSpeed;
			float agentMaxAcceleration;
		};
	}
}
