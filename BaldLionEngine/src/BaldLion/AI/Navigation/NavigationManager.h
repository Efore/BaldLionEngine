#pragma once
#pragma once

#include "Recast.h"
#include "DetourNavMesh.h"
#include "DetourObstacleAvoidance.h"
#include "DetourCrowd.h"

#include "BaldLion/Core/JobManagement/JobManager.h"

#include <glm/glm.hpp>

namespace BaldLion::AI::Navigation
{
	class NavigationManager {


	public:

		static void Init();		
		static void Stop();
		static void Update();

		static void InitCrowd();
		static i32 CreateCrowdAgent(const glm::vec3& pos, float maxSpeed, float maxAcceleration);
		static void DestroyCrowdAgent(i32 agentIdx);

		static const dtCrowdAgent* GetCrowdAgent(i32 agentIdx);
		static void RequestMoveTarget(i32 agentIndex, const glm::vec3& targetPos);

	private:
		
		static const int MAX_AGENTS = 128;

		static dtCrowdAgentDebugInfo s_agentDebug;
		static dtCrowd* s_crowd;
		static dtObstacleAvoidanceDebugData* s_vod;

		static bool s_navigationActive;

	};
}