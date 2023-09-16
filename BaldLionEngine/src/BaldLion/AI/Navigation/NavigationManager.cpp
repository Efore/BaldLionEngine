#include "blpch.h"
#include "NavigationManager.h"
#include "NavMeshBuilder.h"

namespace BaldLion::AI::Navigation
{
	dtObstacleAvoidanceDebugData* NavigationManager::s_vod = nullptr;
	dtCrowd* NavigationManager::s_crowd = nullptr;
	dtCrowdAgentDebugInfo NavigationManager::s_agentDebug;

	bool NavigationManager::s_navigationActive = false;

	void NavigationManager::Init()
	{
		NavMeshBuilder::Init();
		s_crowd = dtAllocCrowd();

		s_vod = dtAllocObstacleAvoidanceDebugData();
		s_vod->init(2048);

		memset(&s_agentDebug, 0, sizeof(s_agentDebug));
		s_agentDebug.idx = -1;
		s_agentDebug.vod = s_vod;
	}

	void NavigationManager::Stop()
	{
		NavMeshBuilder::Stop();
		dtFreeCrowd(s_crowd);
		dtFreeObstacleAvoidanceDebugData(s_vod);
	}	

	void NavigationManager::InitCrowd()
	{
		if (NavMeshBuilder::GetNavMesh() != nullptr)
		{
			s_navigationActive = true;
			s_crowd->init(MAX_AGENTS, NavMeshBuilder::navMeshConfig.agentRadius, NavMeshBuilder::GetNavMesh());
			s_crowd->getEditableFilter(0)->setExcludeFlags(SAMPLE_POLYFLAGS_DISABLED);

			// Setup local avoidance params to different qualities.
			dtObstacleAvoidanceParams params;
			// Use mostly default settings, copy from dtCrowd.
			memcpy(&params, s_crowd->getObstacleAvoidanceParams(0), sizeof(dtObstacleAvoidanceParams));

			// Low (11)
			params.velBias = 0.5f;
			params.adaptiveDivs = 5;
			params.adaptiveRings = 2;
			params.adaptiveDepth = 1;
			s_crowd->setObstacleAvoidanceParams(0, &params);

			// Medium (22)
			params.velBias = 0.5f;
			params.adaptiveDivs = 5;
			params.adaptiveRings = 2;
			params.adaptiveDepth = 2;
			s_crowd->setObstacleAvoidanceParams(1, &params);

			// Good (45)
			params.velBias = 0.5f;
			params.adaptiveDivs = 7;
			params.adaptiveRings = 2;
			params.adaptiveDepth = 3;
			s_crowd->setObstacleAvoidanceParams(2, &params);

			// High (66)
			params.velBias = 0.5f;
			params.adaptiveDivs = 7;
			params.adaptiveRings = 3;
			params.adaptiveDepth = 3;

			s_crowd->setObstacleAvoidanceParams(3, &params);
		}
	}

	void NavigationManager::Update()
	{
		if (!s_navigationActive) 
		{
			return;
		}

		JobManagement::Job navigationUpdateJob("Update navigation job", JobManagement::Job::JobType::Navigation);

		navigationUpdateJob.Task = [] {
			BL_PROFILE_SCOPE("Crowd update",Optick::Category::GameLogic);
			s_crowd->update(Time::GetDeltaTime(), &s_agentDebug);
		};

		JobManagement::JobManager::QueueJob(navigationUpdateJob);
	}

	i32 NavigationManager::CreateCrowdAgent(const glm::vec3& pos, float maxSpeed, float maxAcceleration)
	{
		dtCrowdAgentParams ap;
		memset(&ap, 0, sizeof(ap));
		ap.radius = NavMeshBuilder::navMeshConfig.agentRadius;
		ap.height = NavMeshBuilder::navMeshConfig.agentHeight;
		ap.collisionQueryRange = ap.radius * 12.0f;
		ap.pathOptimizationRange = ap.radius * 30.0f;

		ap.maxAcceleration = maxAcceleration;
		ap.maxSpeed = maxSpeed;

		ap.updateFlags = 0;
		ap.obstacleAvoidanceType = (unsigned char)3.0f;
		ap.separationWeight = 2.0f;

		return s_crowd->addAgent((const float*)&pos, &ap);
	}

	void NavigationManager::DestroyCrowdAgent(i32 agentIdx)
	{
		s_crowd->removeAgent(agentIdx);

		if (agentIdx == s_agentDebug.idx)
			s_agentDebug.idx = -1;
	}

	const dtCrowdAgent* NavigationManager::GetCrowdAgent(i32 agentIdx)
	{
		return s_crowd->getAgent(agentIdx);
	}

	void NavigationManager::RequestMoveTarget(i32 agentIndex, const glm::vec3& targetPos)
	{
		const dtQueryFilter* filter = s_crowd->getFilter(0);
		const float* ext = s_crowd->getQueryExtents();
		dtNavMeshQuery* navquery = NavMeshBuilder::GetNavMeshQuery();

		dtPolyRef targetRef;
		float targetPosition[3];

		navquery->findNearestPoly((float*)&targetPos, ext, filter, &targetRef, targetPosition);

		const dtCrowdAgent* ag = s_crowd->getAgent(agentIndex);
		if (!ag->active) return;

		s_crowd->requestMoveTarget(agentIndex, targetRef, targetPosition);
	}

	void NavigationManager::UpdateCrowdAgent(i32 agentIndex, float maxSpeed, float maxAcceleration)
	{
		const dtCrowdAgent* ag = s_crowd->getAgent(agentIndex);
		if (!ag->active) return;

		dtCrowdAgentParams params = ag->params;
		params.maxSpeed = maxSpeed;
		params.maxAcceleration = maxAcceleration;

		s_crowd->updateAgentParameters(agentIndex, &params);
	}

}