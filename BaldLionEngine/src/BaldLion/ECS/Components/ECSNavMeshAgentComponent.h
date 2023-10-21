#pragma once

#include "BaldLion/ECS/ECSComponent.h"
#include "BaldLion/AI/Navigation/NavigationManager.h"

namespace BaldLion 
{
	namespace ECS
	{
		class ECSNavMeshAgentComponent : public ECSComponent {

		public:

			ECSNavMeshAgentComponent(const glm::vec3& startPosition, float maxSpeed, float maxAcceleration) :
				ECSComponent(ECSComponentType::NavMeshAgent), agentMaxSpeed(maxSpeed), agentMaxAcceleration(maxAcceleration)
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
