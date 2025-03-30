#pragma once

#include "Recast.h"
#include "DetourNavMesh.h"
#include "DetourObstacleAvoidance.h"
#include "DetourCrowd.h"

#include "BaldLion/Core/Threading/TaskScheduler.h"
#include <glm/glm.hpp>

namespace BaldLion::AI::Navigation
{
	enum MoveToResult : ui8
	{
		InvalidAgent,
		InvalidPath,
		Sucess
	};

	class NavigationManager {


	public:

		static void Init();		
		static void Stop();
		static void Update(float deltaTime);

		static void InitCrowd();
		static i32 CreateCrowdAgent(const glm::vec3& pos, float maxSpeed, float maxAcceleration);
		static void DestroyCrowdAgent(i32 agentIdx);

		static const dtCrowdAgent* GetCrowdAgent(i32 agentIdx);
		static glm::vec3 GetCrowdAgentPosition(i32 agentIdx);
		static void RequestMoveTarget(i32 agentIndex, const glm::vec3& targetPos);
		static void UpdateCrowdAgent(i32 agentIndex, float maxSpeed, float maxAcceleration);

	private:
			
		static void CheckWalkingAgents();

	private:
		
		static constexpr int MAX_AGENTS = 128;

		static dtCrowdAgentDebugInfo s_agentDebug;
		static dtCrowd* s_crowd;
		static dtObstacleAvoidanceDebugData* s_vod;

		static bool s_navigationActive;
		static Threading::TaskID s_updateTask;

		static DynamicArray<i32> s_walkingAgents;

	};
}