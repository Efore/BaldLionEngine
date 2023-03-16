#pragma once

#include "Recast.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "RecastClasses/BuildContext.h"
#include "RecastClasses/InputGeom.h"

namespace BaldLion::AI::Navigation 
{
	struct NavMeshBuildSettings
	{
		float cellSize;
		float cellHeight;
		float agentHeight;
		float agentRadius;
		float agentMaxClimb;
		float agentMaxSlope;
		float regionMinSize;
		float regionMergeSize;
		float edgeMaxLen;
		float edgeMaxError;
		float vertsPerPoly;
		float detailSampleDist;
		float detailSampleMaxError;
		int partitionType;
	};

	class NavMeshBuilder
	{

	public:

		static bool BuildNavMesh(const char* pathfilepath);		
		static const InputGeom* GetInputGeom();

	private:

		static void LogLine(rcContext& ctx, rcTimerLabel label, const char* name, const float pc);
		static void LogBuildTimes(rcContext& ctx, const int totalTimeUsec);

	public:
		static NavMeshBuildSettings navMeshConfig;
		

	private:	
		static InputGeom* m_geom;
		static dtNavMesh* m_navMesh;
		static dtNavMeshQuery* m_navQuery;		

		static unsigned char* m_triareas;
		static rcHeightfield* m_solid;
		static rcCompactHeightfield* m_chf;
		static rcContourSet* m_cset;
		static rcPolyMesh* m_pmesh;
		static rcPolyMeshDetail* m_dmesh;

		static rcConfig m_cfg;
		static BuildContext m_ctx;
	};
}

