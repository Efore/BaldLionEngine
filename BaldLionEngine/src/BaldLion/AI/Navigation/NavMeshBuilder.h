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
		float tileSize;
		int maxTiles;
		int maxPolys;
	};

	class NavMeshBuilder
	{

	public:
		static void Init();
		static void Stop();

		static bool LoadGeom();
		static bool BuildNavMesh();
		static InputGeom* GetInputGeom();		
		static dtNavMesh* GetNavMesh();
		static dtNavMeshQuery* GetNavMeshQuery();
		static bool NavMeshIsValid();
		static bool NavMeshIsBaking();

	private:

		static bool InternalBuildNavMesh();
		static void BuildAllTiles();
		static void LogLine(rcContext& ctx, rcTimerLabel label, const char* name, const float pc);
		static void LogBuildTimes(rcContext& ctx, const int totalTimeUsec);
		static unsigned char* BuildTileMesh(const int tx, const int ty, const float* bmin, const float* bmax, int& dataSize);
		static void CleanUp();

	public:
		static NavMeshBuildSettings navMeshConfig;
		

	private:	
		static InputGeom* s_geom;
		static dtNavMesh* s_navMesh;
		static dtNavMeshQuery* s_navQuery;		

		static unsigned char* s_triareas;
		static rcHeightfield* s_solid;
		static rcCompactHeightfield* s_chf;
		static rcContourSet* s_cset;
		static rcPolyMesh* s_pmesh;
		static rcPolyMeshDetail* s_dmesh;

		static rcConfig s_cfg;
		static BuildContext s_ctx;

		static float s_lastBuiltTileBmin[3];
		static float s_lastBuiltTileBmax[3];

		static float s_totalBuildTimeMs;
		static float s_tileBuildTime;
		static float s_tileMemUsage;
		static int s_tileTriCount;

		static bool s_isBakingNavmesh;
	};
}

