#include "blpch.h"
#include "ECSPathfindingSystem.h"

#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/ECS/Components/ECSNavMeshAgentComponent.h"

#include "BaldLion/Utils/MathUtils.h"

namespace BaldLion 
{
	namespace ECS 
	{
		//using namespace AI::Navigation;

		void ECSPathfindingSystem::UpdateComponents(ECSComponentLookUp* componentLookUp)
		{
			//ECSNavMeshAgentComponent* navMeshAgent = componentLookUp->Write<ECSNavMeshAgentComponent>(ECSComponentType::NavMeshAgent);
			//
			//if (navMeshAgent->currentState == ECSNavMeshAgentComponent::NavMeshAgentState::FindingPath)
			//{
			//	ui32 initialPolygon = NavigationMeshGenerator::GetPolygonIndex(navMeshAgent->pathStart);
			//	ui32 lastPolygon = NavigationMeshGenerator::GetPolygonIndex(navMeshAgent->pathEnd);

			//	if (NavigationMeshGenerator::FindPath(initialPolygon, lastPolygon, navMeshAgent->navMeshPath)) 
			//	{
			//		navMeshAgent->currentState = ECSNavMeshAgentComponent::NavMeshAgentState::None;
			//	}
			//	else
			//	{
			//		navMeshAgent->currentState =  ECSNavMeshAgentComponent::NavMeshAgentState::PathFound;
			//	}
			//}
			//else if(navMeshAgent->currentState == ECSNavMeshAgentComponent::NavMeshAgentState::PathFound){

			//	ECSTransformComponent* transform = componentLookUp->Write<ECSTransformComponent>(ECSComponentType::Transform);
			//	const glm::vec3 currentDestination = navMeshAgent->navMeshPath[navMeshAgent->currentPathIndex];

			//	glm::vec3 directionToNextPathpoint = currentDestination - transform->position;

			//	//Rotation
			//	{
			//		directionToNextPathpoint.y = 0.0f;
			//		directionToNextPathpoint = glm::normalize(directionToNextPathpoint);

			//		glm::vec3 currentTransformForward = MathUtils::GetTransformForwardDirection(transform->GetTransformMatrix());
			//		currentTransformForward.y = 0.0f;
			//		currentTransformForward = glm::normalize(currentTransformForward);

			//		float dot = glm::dot(directionToNextPathpoint, currentTransformForward);
			//		float angleToDirection = glm::acos(dot);

			//		float yaw = angleToDirection * navMeshAgent->rotationSpeed * Time::GetDeltaTime();
			//		transform->rotation.y += yaw;
			//	}

			//	//Movement
			//	{
			//		transform->position += directionToNextPathpoint * navMeshAgent->movementSpeed * Time::GetDeltaTime();
			//	}
			//	
			//	//Checking destination reached
			//	{
			//		glm::vec3 prevPointToNextPoint = currentDestination - navMeshAgent->navMeshPath[navMeshAgent->currentPathIndex - 1];
			//		prevPointToNextPoint.y = 0.0f;

			//		const float prevPointToNextPointMagnitude = glm::length(prevPointToNextPoint);

			//		glm::vec3 prevPointToCurrentPosition = transform->position - navMeshAgent->navMeshPath[navMeshAgent->currentPathIndex - 1];
			//		prevPointToCurrentPosition.y = 0.0f;

			//		const float pathCompletionRate = (glm::dot(prevPointToCurrentPosition, prevPointToNextPoint) / prevPointToNextPointMagnitude) / prevPointToNextPointMagnitude;

			//		if (pathCompletionRate > 0.95f)
			//		{
			//			navMeshAgent->currentPathIndex++;
			//			if (navMeshAgent->currentPathIndex >= navMeshAgent->navMeshPath.Size())
			//			{
			//				navMeshAgent->currentPathIndex = 0;
			//				navMeshAgent->currentState = ECSNavMeshAgentComponent::NavMeshAgentState::None;
			//			}
			//		}
			//	}
			//}
		}
	}
}
