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

		static bool LoadGeom(const char* filepath);
		static bool BuildNavMesh();
		static const InputGeom* GetInputGeom();
		static const dtNavMesh* GetNavMesh();
		static bool NavMeshIsValid();

	private:

		static bool InternalBuildNavMesh(bool parallelize);
		static void BuildAllTiles();
		static void LogLine(rcContext& ctx, rcTimerLabel label, const char* name, const float pc);
		static void LogBuildTimes(rcContext& ctx, const int totalTimeUsec);
		static unsigned char* BuildTileMesh(const int tx, const int ty, const float* bmin, const float* bmax, int& dataSize);
		static void CleanUp();

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

		static float m_lastBuiltTileBmin[3];
		static float m_lastBuiltTileBmax[3];

		static float m_totalBuildTimeMs;
		static float m_tileBuildTime;
		static float m_tileMemUsage;
		static int m_tileTriCount;
	};
}

