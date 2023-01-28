#pragma once

#include "BaldLion/ECS/ECSComponent.h"
#include "BaldLion/Core/StringId.h"
#include "BaldLion/Animation/AnimationManager.h"

namespace BaldLion 
{
	namespace ECS
	{
		class ECSNavMeshAgentComponent : public ECSComponent {

		public:

			ECSNavMeshAgentComponent(float movementSpeed, float rotationSpeed) :
				ECSComponent(ECSComponentType::NavMeshAgent), movementSpeed(movementSpeed), rotationSpeed(rotationSpeed), currentPathIndex(0)
			{
				navMeshPath = DynamicArray<glm::vec3>(AllocationType::FreeList_ECS, 10);
			}

			void SetDestination(const glm::vec3 origin, const glm::vec3& destination)
			{
				pathStart = origin;
				pathEnd = destination;
				currentState = NavMeshAgentState::FindingPath;
			}

		public:	
			
			glm::vec3 pathStart;
			glm::vec3 pathEnd;
			float movementSpeed;
			float rotationSpeed;
			DynamicArray<glm::vec3> navMeshPath;
			ui32 currentPathIndex;

			enum class NavMeshAgentState {
				None,
				FindingPath,
				PathFound
			} currentState;
		};
	}
}
