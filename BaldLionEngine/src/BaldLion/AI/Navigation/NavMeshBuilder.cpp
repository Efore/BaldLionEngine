#include "blpch.h"
#include "NavMeshBuilder.h"
#include "RecastClasses/InputGeom.h"
#include "DetourNavMeshBuilder.h"
#include "BaldLion/Core/JobManagement/JobManager.h"


namespace BaldLion::AI::Navigation
{
	rcPolyMeshDetail* NavMeshBuilder::s_dmesh = nullptr;
	rcPolyMesh* NavMeshBuilder::s_pmesh = nullptr;
	rcContourSet* NavMeshBuilder::s_cset = nullptr;
	rcCompactHeightfield* NavMeshBuilder::s_chf = nullptr;
	rcHeightfield* NavMeshBuilder::s_solid = nullptr;
	unsigned char* NavMeshBuilder::s_triareas = nullptr;
	
	dtNavMeshQuery* NavMeshBuilder::s_navQuery = nullptr;
	dtNavMesh* NavMeshBuilder::s_navMesh = nullptr;
	InputGeom* NavMeshBuilder::s_geom = nullptr;

	BuildContext NavMeshBuilder::s_ctx;
	rcConfig NavMeshBuilder::s_cfg;
	NavMeshBuildSettings NavMeshBuilder::navMeshConfig;

	float NavMeshBuilder::s_lastBuiltTileBmax[3];
	float NavMeshBuilder::s_lastBuiltTileBmin[3];

	float NavMeshBuilder::s_tileMemUsage;
	float NavMeshBuilder::s_tileBuildTime;
	float NavMeshBuilder::s_totalBuildTimeMs;
	int NavMeshBuilder::s_tileTriCount;

	void NavMeshBuilder::Init()
	{
		navMeshConfig.cellSize = 0.3f;
		navMeshConfig.cellHeight = 0.2f;
		navMeshConfig.agentHeight = 2.0f;
		navMeshConfig.agentRadius = 0.6f;
		navMeshConfig.agentMaxClimb = 0.9f;
		navMeshConfig.agentMaxSlope = 45.0f;
		navMeshConfig.regionMinSize = 8.0f;
		navMeshConfig.regionMergeSize = 20.0f;
		navMeshConfig.edgeMaxLen = 12.0f;
		navMeshConfig.edgeMaxError = 1.3f;
		navMeshConfig.vertsPerPoly = 6.0f;
		navMeshConfig.detailSampleDist = 6.0f;
		navMeshConfig.detailSampleMaxError = 1.0f;
		navMeshConfig.partitionType = 0;
		navMeshConfig.maxTiles = 0;
		navMeshConfig.tileSize = 32;
		navMeshConfig.maxPolys = 0;

		s_navQuery = dtAllocNavMeshQuery();

		s_totalBuildTimeMs = 0;
		s_triareas = 0;
		s_solid = 0;
		s_chf = 0;
		s_cset = 0;
		s_pmesh = 0;
		s_dmesh = 0;		
		s_tileBuildTime = 0;
		s_tileMemUsage = 0;
		s_tileTriCount = 0;

		memset(s_lastBuiltTileBmin, 0, sizeof(s_lastBuiltTileBmin));
		memset(s_lastBuiltTileBmax, 0, sizeof(s_lastBuiltTileBmax));
	}

	void NavMeshBuilder::Stop()
	{
		CleanUp();

		if (s_navQuery != nullptr)
		{
			dtFreeNavMeshQuery(s_navQuery);
			s_navQuery = nullptr;
		}

		if (s_navMesh != nullptr)
		{
			dtFreeNavMesh(s_navMesh);
			s_navMesh = nullptr;
		}
	}

	bool NavMeshBuilder::LoadGeom(const char* filepath)
	{
		if (s_ctx.GetInitialized())
		{
			s_ctx.Stop();
		}
		s_ctx.Init();

		if (s_geom != nullptr)
		{
			MemoryManager::Delete(s_geom);
		}

		s_geom = MemoryManager::New<InputGeom>("NavMesh Geom", AllocationType::FreeList_ECS);

		BuildSettings buildSettings;

		buildSettings.cellSize = navMeshConfig.cellSize;
		buildSettings.cellHeight = navMeshConfig.cellHeight;
		buildSettings.agentHeight = navMeshConfig.agentHeight;
		buildSettings.agentRadius = navMeshConfig.agentRadius;
		buildSettings.agentMaxClimb = navMeshConfig.agentMaxClimb;
		buildSettings.agentMaxSlope = navMeshConfig.agentMaxSlope;
		buildSettings.regionMinSize = navMeshConfig.regionMinSize;
		buildSettings.regionMergeSize = navMeshConfig.regionMergeSize;
		buildSettings.edgeMaxLen = navMeshConfig.edgeMaxLen;
		buildSettings.edgeMaxError = navMeshConfig.edgeMaxError;
		buildSettings.vertsPerPoly = navMeshConfig.vertsPerPoly;
		buildSettings.detailSampleDist = navMeshConfig.detailSampleDist;
		buildSettings.detailSampleMaxError = navMeshConfig.detailSampleMaxError;
		buildSettings.partitionType = navMeshConfig.partitionType;
		buildSettings.tileSize = navMeshConfig.tileSize;

		s_geom->saveGeomSet(&buildSettings);

		if (!s_geom->load(&s_ctx, filepath))
		{
			MemoryManager::Delete(s_geom);
			s_geom = nullptr;
			return false;
		}
		return true;
	}

	bool NavMeshBuilder::BuildNavMesh()
	{	
		if (s_geom == nullptr)
		{
			return false;
		}

		InternalBuildNavMesh(true);

		return true;
	}	
	
	bool NavMeshBuilder::InternalBuildNavMesh(bool parallelize)
	{
		if (!s_geom || !s_geom->getMesh())
		{
			s_ctx.log(RC_LOG_ERROR, "buildTiledNavigation: No vertices and triangles.");
			return false;
		}

		dtFreeNavMesh(s_navMesh);

		s_navMesh = dtAllocNavMesh();
		if (!s_navMesh)
		{
			s_ctx.log(RC_LOG_ERROR, "buildTiledNavigation: Could not allocate navmesh.");
			return false;
		}

		dtNavMeshParams params;
		rcVcopy(params.orig, s_geom->getNavMeshBoundsMin());
		params.tileWidth = navMeshConfig.tileSize *  navMeshConfig.cellSize;
		params.tileHeight = navMeshConfig.tileSize *  navMeshConfig.cellSize;
		params.maxTiles = navMeshConfig.maxTiles;
		params.maxPolys = navMeshConfig.maxPolys;

		dtStatus status;

		status = s_navMesh->init(&params);
		if (dtStatusFailed(status))
		{
			s_ctx.log(RC_LOG_ERROR, "buildTiledNavigation: Could not init navmesh.");
			return false;
		}

		status = s_navQuery->init(s_navMesh, 2048);
		if (dtStatusFailed(status))
		{
			s_ctx.log(RC_LOG_ERROR, "buildTiledNavigation: Could not init Detour navmesh query");
			return false;
		}

		JobManagement::Job systemUpdateJob("Bake Nav Mesh");

		systemUpdateJob.Task = [] {

			BuildAllTiles();
		};

		JobManagement::JobManager::QueueJob(systemUpdateJob);

		return true;
	}

	void NavMeshBuilder::BuildAllTiles()
	{
		if (!s_geom) return;
		if (!s_navMesh) return;

		const float* bmin = s_geom->getNavMeshBoundsMin();
		const float* bmax = s_geom->getNavMeshBoundsMax();
		int gw = 0, gh = 0;
		rcCalcGridSize(bmin, bmax, navMeshConfig.cellSize, &gw, &gh);
		const int ts = (int)navMeshConfig.tileSize;
		const int tw = (gw + ts - 1) / ts;
		const int th = (gh + ts - 1) / ts;
		const float tcs = navMeshConfig.tileSize * navMeshConfig.cellSize;

		// Start the build process.
		s_ctx.startTimer(RC_TIMER_TEMP);

		for (int y = 0; y < th; ++y)
		{
			for (int x = 0; x < tw; ++x)
			{
				s_lastBuiltTileBmin[0] = bmin[0] + x * tcs;
				s_lastBuiltTileBmin[1] = bmin[1];
				s_lastBuiltTileBmin[2] = bmin[2] + y * tcs;

				s_lastBuiltTileBmax[0] = bmin[0] + (x + 1)*tcs;
				s_lastBuiltTileBmax[1] = bmax[1];
				s_lastBuiltTileBmax[2] = bmin[2] + (y + 1)*tcs;

				int dataSize = 0;
				unsigned char* data = BuildTileMesh(x, y, s_lastBuiltTileBmin, s_lastBuiltTileBmax, dataSize);
				if (data)
				{
					// Remove any previous data (navmesh owns and deletes the data).
					s_navMesh->removeTile(s_navMesh->getTileRefAt(x, y, 0), 0, 0);
					// Let the navmesh own the data.
					dtStatus status = s_navMesh->addTile(data, dataSize, DT_TILE_FREE_DATA, 0, 0);
					if (dtStatusFailed(status))
						dtFree(data);
				}
			}
		}

		// Start the build process.	
		s_ctx.stopTimer(RC_TIMER_TEMP);

		s_totalBuildTimeMs = s_ctx.getAccumulatedTime(RC_TIMER_TEMP) / 1000.0f;
	}

	unsigned char* NavMeshBuilder::BuildTileMesh(const int tx, const int ty, const float* bmin, const float* bmax, int& dataSize)
	{
		if (!s_geom || !s_geom->getMesh() || !s_geom->getChunkyMesh())
		{
			s_ctx.log(RC_LOG_ERROR, "buildNavigation: Input mesh is not specified.");
			return 0;
		}

		s_tileMemUsage = 0;
		s_tileBuildTime = 0;

		CleanUp();

		const float* verts = s_geom->getMesh()->getVerts();
		const int nverts = s_geom->getMesh()->getVertCount();
		const int ntris = s_geom->getMesh()->getTriCount();
		const rcChunkyTriMesh* chunkyMesh = s_geom->getChunkyMesh();

		// Init build configuration from GUI
		memset(&s_cfg, 0, sizeof(s_cfg));
		s_cfg.cs = navMeshConfig.cellSize;
		s_cfg.ch = navMeshConfig.cellHeight;
		s_cfg.walkableSlopeAngle = navMeshConfig.agentMaxSlope;
		s_cfg.walkableHeight = (int)ceilf(navMeshConfig.agentHeight / s_cfg.ch);
		s_cfg.walkableClimb = (int)floorf(navMeshConfig.agentMaxClimb / s_cfg.ch);
		s_cfg.walkableRadius = (int)ceilf(navMeshConfig.agentRadius / s_cfg.cs);
		s_cfg.maxEdgeLen = (int)(navMeshConfig.edgeMaxLen / navMeshConfig.cellSize);
		s_cfg.maxSimplificationError = navMeshConfig.edgeMaxError;
		s_cfg.minRegionArea = (int)rcSqr(navMeshConfig.regionMinSize);		// Note: area = size*size
		s_cfg.mergeRegionArea = (int)rcSqr(navMeshConfig.regionMergeSize);	// Note: area = size*size
		s_cfg.maxVertsPerPoly = (int)navMeshConfig.vertsPerPoly;
		s_cfg.tileSize = (int)navMeshConfig.tileSize;
		s_cfg.borderSize = s_cfg.walkableRadius + 3; // Reserve enough padding.
		s_cfg.width = s_cfg.tileSize + s_cfg.borderSize * 2;
		s_cfg.height = s_cfg.tileSize + s_cfg.borderSize * 2;
		s_cfg.detailSampleDist = navMeshConfig.detailSampleDist < 0.9f ? 0 : navMeshConfig.cellSize * navMeshConfig.detailSampleDist;
		s_cfg.detailSampleMaxError = navMeshConfig.cellHeight * navMeshConfig.detailSampleDist;

		// Expand the heighfield bounding box by border size to find the extents of geometry we need to build this tile.
		//
		// This is done in order to make sure that the navmesh tiles connect correctly at the borders,
		// and the obstacles close to the border work correctly with the dilation process.
		// No polygons (or contours) will be created on the border area.
		//
		// IMPORTANT!
		//
		//   :''''''''':
		//   : +-----+ :
		//   : |     | :
		//   : |     |<--- tile to build
		//   : |     | :  
		//   : +-----+ :<-- geometry needed
		//   :.........:
		//
		// You should use this bounding box to query your input geometry.
		//
		// For example if you build a navmesh for terrain, and want the navmesh tiles to match the terrain tile size
		// you will need to pass in data from neighbour terrain tiles too! In a simple case, just pass in all the 8 neighbours,
		// or use the bounding box below to only pass in a sliver of each of the 8 neighbours.
		rcVcopy(s_cfg.bmin, bmin);
		rcVcopy(s_cfg.bmax, bmax);
		s_cfg.bmin[0] -= s_cfg.borderSize*s_cfg.cs;
		s_cfg.bmin[2] -= s_cfg.borderSize*s_cfg.cs;
		s_cfg.bmax[0] += s_cfg.borderSize*s_cfg.cs;
		s_cfg.bmax[2] += s_cfg.borderSize*s_cfg.cs;

		// Reset build times gathering.
		s_ctx.resetTimers();

		// Start the build process.
		s_ctx.startTimer(RC_TIMER_TOTAL);

		s_ctx.log(RC_LOG_PROGRESS, "Building navigation:");
		s_ctx.log(RC_LOG_PROGRESS, " - %d x %d cells", s_cfg.width, s_cfg.height);
		s_ctx.log(RC_LOG_PROGRESS, " - %.1fK verts, %.1fK tris", nverts / 1000.0f, ntris / 1000.0f);

		// Allocate voxel heightfield where we rasterize our input data to.
		s_solid = rcAllocHeightfield();
		if (!s_solid)
		{
			s_ctx.log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
			return 0;
		}
		if (!rcCreateHeightfield(&s_ctx, *s_solid, s_cfg.width, s_cfg.height, s_cfg.bmin, s_cfg.bmax, s_cfg.cs, s_cfg.ch))
		{
			s_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
			return 0;
		}

		// Allocate array that can hold triangle flags.
		// If you have multiple meshes you need to process, allocate
		// and array which can hold the max number of triangles you need to process.
		s_triareas = new unsigned char[chunkyMesh->maxTrisPerChunk];
		if (!s_triareas)
		{
			s_ctx.log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", chunkyMesh->maxTrisPerChunk);
			return 0;
		}

		float tbmin[2], tbmax[2];
		tbmin[0] = s_cfg.bmin[0];
		tbmin[1] = s_cfg.bmin[2];
		tbmax[0] = s_cfg.bmax[0];
		tbmax[1] = s_cfg.bmax[2];
		int cid[512];// TODO: Make grow when returning too many items.
		const int ncid = rcGetChunksOverlappingRect(chunkyMesh, tbmin, tbmax, cid, 512);
		if (!ncid)
			return 0;

		s_tileTriCount = 0;

		for (int i = 0; i < ncid; ++i)
		{
			const rcChunkyTriMeshNode& node = chunkyMesh->nodes[cid[i]];
			const int* ctris = &chunkyMesh->tris[node.i * 3];
			const int nctris = node.n;

			s_tileTriCount += nctris;

			memset(s_triareas, 0, nctris * sizeof(unsigned char));
			rcMarkWalkableTriangles(&s_ctx, s_cfg.walkableSlopeAngle,
				verts, nverts, ctris, nctris, s_triareas);

			if (!rcRasterizeTriangles(&s_ctx, verts, nverts, ctris, s_triareas, nctris, *s_solid, s_cfg.walkableClimb))
				return 0;
		}

		delete[] s_triareas;
		s_triareas = 0;
		

		// Once all geometry is rasterized, we do initial pass of filtering to
		// remove unwanted overhangs caused by the conservative rasterization
		// as well as filter spans where the character cannot possibly stand.
		rcFilterLowHangingWalkableObstacles(&s_ctx, s_cfg.walkableClimb, *s_solid);
		rcFilterLedgeSpans(&s_ctx, s_cfg.walkableHeight, s_cfg.walkableClimb, *s_solid);
		rcFilterWalkableLowHeightSpans(&s_ctx, s_cfg.walkableHeight, *s_solid);

		// Compact the heightfield so that it is faster to handle from now on.
		// This will result more cache coherent data as well as the neighbours
		// between walkable cells will be calculated.
		s_chf = rcAllocCompactHeightfield();
		if (!s_chf)
		{
			s_ctx.log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
			return 0;
		}
		if (!rcBuildCompactHeightfield(&s_ctx, s_cfg.walkableHeight, s_cfg.walkableClimb, *s_solid, *s_chf))
		{
			s_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
			return 0;
		}

		
		rcFreeHeightField(s_solid);
		s_solid = 0;
		

		// Erode the walkable area by agent radius.
		if (!rcErodeWalkableArea(&s_ctx, s_cfg.walkableRadius, *s_chf))
		{
			s_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
			return 0;
		}

		// (Optional) Mark areas.
		const ConvexVolume* vols = s_geom->getConvexVolumes();
		for (int i = 0; i < s_geom->getConvexVolumeCount(); ++i)
			rcMarkConvexPolyArea(&s_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *s_chf);


		// Partition the heightfield so that we can use simple algorithm later to triangulate the walkable areas.
		// There are 3 martitioning methods, each with some pros and cons:
		// 1) Watershed partitioning
		//   - the classic Recast partitioning
		//   - creates the nicest tessellation
		//   - usually slowest
		//   - partitions the heightfield into nice regions without holes or overlaps
		//   - the are some corner cases where this method creates produces holes and overlaps
		//      - holes may appear when a small obstacles is close to large open area (triangulation can handle this)
		//      - overlaps may occur if you have narrow spiral corridors (i.e stairs), this make triangulation to fail
		//   * generally the best choice if you precompute the nacmesh, use this if you have large open areas
		// 2) Monotone partioning
		//   - fastest
		//   - partitions the heightfield into regions without holes and overlaps (guaranteed)
		//   - creates long thin polygons, which sometimes causes paths with detours
		//   * use this if you want fast navmesh generation
		// 3) Layer partitoining
		//   - quite fast
		//   - partitions the heighfield into non-overlapping regions
		//   - relies on the triangulation code to cope with holes (thus slower than monotone partitioning)
		//   - produces better triangles than monotone partitioning
		//   - does not have the corner cases of watershed partitioning
		//   - can be slow and create a bit ugly tessellation (still better than monotone)
		//     if you have large open areas with small obstacles (not a problem if you use tiles)
		//   * good choice to use for tiled navmesh with medium and small sized tiles

		if (navMeshConfig.partitionType == SAMPLE_PARTITION_WATERSHED)
		{
			// Prepare for region partitioning, by calculating distance field along the walkable surface.
			if (!rcBuildDistanceField(&s_ctx, *s_chf))
			{
				s_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not build distance field.");
				return 0;
			}

			// Partition the walkable surface into simple regions without holes.
			if (!rcBuildRegions(&s_ctx, *s_chf, s_cfg.borderSize, s_cfg.minRegionArea, s_cfg.mergeRegionArea))
			{
				s_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not build watershed regions.");
				return 0;
			}
		}
		else if (navMeshConfig.partitionType == SAMPLE_PARTITION_MONOTONE)
		{
			// Partition the walkable surface into simple regions without holes.
			// Monotone partitioning does not need distancefield.
			if (!rcBuildRegionsMonotone(&s_ctx, *s_chf, s_cfg.borderSize, s_cfg.minRegionArea, s_cfg.mergeRegionArea))
			{
				s_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not build monotone regions.");
				return 0;
			}
		}
		else // SAMPLE_PARTITION_LAYERS
		{
			// Partition the walkable surface into simple regions without holes.
			if (!rcBuildLayerRegions(&s_ctx, *s_chf, s_cfg.borderSize, s_cfg.minRegionArea))
			{
				s_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not build layer regions.");
				return 0;
			}
		}

		// Create contours.
		s_cset = rcAllocContourSet();
		if (!s_cset)
		{
			s_ctx.log(RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
			return 0;
		}
		if (!rcBuildContours(&s_ctx, *s_chf, s_cfg.maxSimplificationError, s_cfg.maxEdgeLen, *s_cset))
		{
			s_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
			return 0;
		}

		if (s_cset->nconts == 0)
		{
			return 0;
		}

		// Build polygon navmesh from the contours.
		s_pmesh = rcAllocPolyMesh();
		if (!s_pmesh)
		{
			s_ctx.log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
			return 0;
		}
		if (!rcBuildPolyMesh(&s_ctx, *s_cset, s_cfg.maxVertsPerPoly, *s_pmesh))
		{
			s_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not triangulate contours.");
			return 0;
		}

		// Build detail mesh.
		s_dmesh = rcAllocPolyMeshDetail();
		if (!s_dmesh)
		{
			s_ctx.log(RC_LOG_ERROR, "buildNavigation: Out of memory 'dmesh'.");
			return 0;
		}

		if (!rcBuildPolyMeshDetail(&s_ctx, *s_pmesh, *s_chf,
			s_cfg.detailSampleDist, s_cfg.detailSampleMaxError,
			*s_dmesh))
		{
			s_ctx.log(RC_LOG_ERROR, "buildNavigation: Could build polymesh detail.");
			return 0;
		}
		
		rcFreeCompactHeightfield(s_chf);
		s_chf = 0;
		rcFreeContourSet(s_cset);
		s_cset = 0;		

		unsigned char* navData = 0;
		int navDataSize = 0;
		if (s_cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
		{
			if (s_pmesh->nverts >= 0xffff)
			{
				// The vertex indices are ushorts, and cannot point to more than 0xffff vertices.
				s_ctx.log(RC_LOG_ERROR, "Too many vertices per tile %d (max: %d).", s_pmesh->nverts, 0xffff);
				return 0;
			}

			// Update poly flags from areas.
			for (int i = 0; i < s_pmesh->npolys; ++i)
			{
				if (s_pmesh->areas[i] == RC_WALKABLE_AREA)
					s_pmesh->areas[i] = SAMPLE_POLYAREA_GROUND;

				if (s_pmesh->areas[i] == SAMPLE_POLYAREA_GROUND ||
					s_pmesh->areas[i] == SAMPLE_POLYAREA_GRASS ||
					s_pmesh->areas[i] == SAMPLE_POLYAREA_ROAD)
				{
					s_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK;
				}
				else if (s_pmesh->areas[i] == SAMPLE_POLYAREA_WATER)
				{
					s_pmesh->flags[i] = SAMPLE_POLYFLAGS_SWIM;
				}
				else if (s_pmesh->areas[i] == SAMPLE_POLYAREA_DOOR)
				{
					s_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
				}
			}

			dtNavMeshCreateParams params;
			memset(&params, 0, sizeof(params));
			params.verts = s_pmesh->verts;
			params.vertCount = s_pmesh->nverts;
			params.polys = s_pmesh->polys;
			params.polyAreas = s_pmesh->areas;
			params.polyFlags = s_pmesh->flags;
			params.polyCount = s_pmesh->npolys;
			params.nvp = s_pmesh->nvp;
			params.detailMeshes = s_dmesh->meshes;
			params.detailVerts = s_dmesh->verts;
			params.detailVertsCount = s_dmesh->nverts;
			params.detailTris = s_dmesh->tris;
			params.detailTriCount = s_dmesh->ntris;
			params.offMeshConVerts = s_geom->getOffMeshConnectionVerts();
			params.offMeshConRad = s_geom->getOffMeshConnectionRads();
			params.offMeshConDir = s_geom->getOffMeshConnectionDirs();
			params.offMeshConAreas = s_geom->getOffMeshConnectionAreas();
			params.offMeshConFlags = s_geom->getOffMeshConnectionFlags();
			params.offMeshConUserID = s_geom->getOffMeshConnectionId();
			params.offMeshConCount = s_geom->getOffMeshConnectionCount();
			params.walkableHeight = navMeshConfig.agentHeight;
			params.walkableRadius = navMeshConfig.agentRadius;
			params.walkableClimb = navMeshConfig.agentMaxClimb;
			params.tileX = tx;
			params.tileY = ty;
			params.tileLayer = 0;
			rcVcopy(params.bmin, s_pmesh->bmin);
			rcVcopy(params.bmax, s_pmesh->bmax);
			params.cs = s_cfg.cs;
			params.ch = s_cfg.ch;
			params.buildBvTree = true;

			if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
			{
				s_ctx.log(RC_LOG_ERROR, "Could not build Detour navmesh.");
				return 0;
			}
		}
		s_tileMemUsage = navDataSize / 1024.0f;

		s_ctx.stopTimer(RC_TIMER_TOTAL);

		// Show performance stats.
		//duLogBuildTimes(m_ctx, m_ctx.getAccumulatedTime(RC_TIMER_TOTAL));
		s_ctx.log(RC_LOG_PROGRESS, ">> Polymesh: %d vertices  %d polygons", s_pmesh->nverts, s_pmesh->npolys);

		s_tileBuildTime = s_ctx.getAccumulatedTime(RC_TIMER_TOTAL) / 1000.0f;

		dataSize = navDataSize;
		return navData;
	}

	void NavMeshBuilder::LogLine(rcContext& ctx, rcTimerLabel label, const char* name, const float pc)
	{
		const int t = ctx.getAccumulatedTime(label);
		if (t < 0) return;
		ctx.log(RC_LOG_PROGRESS, "%s:\t%.2fms\t(%.1f%%)", name, t / 1000.0f, t*pc);
	}

	void NavMeshBuilder::LogBuildTimes(rcContext& ctx, const int totalTimeUsec)
	{		
		const float pc = 100.0f / totalTimeUsec;

		ctx.log(RC_LOG_PROGRESS, "Build Times");
		LogLine(ctx, RC_TIMER_RASTERIZE_TRIANGLES, "- Rasterize", pc);
		LogLine(ctx, RC_TIMER_BUILD_COMPACTHEIGHTFIELD, "- Build Compact", pc);
		LogLine(ctx, RC_TIMER_FILTER_BORDER, "- Filter Border", pc);
		LogLine(ctx, RC_TIMER_FILTER_WALKABLE, "- Filter Walkable", pc);
		LogLine(ctx, RC_TIMER_ERODE_AREA, "- Erode Area", pc);
		LogLine(ctx, RC_TIMER_MEDIAN_AREA, "- Median Area", pc);
		LogLine(ctx, RC_TIMER_MARK_BOX_AREA, "- Mark Box Area", pc);
		LogLine(ctx, RC_TIMER_MARK_CONVEXPOLY_AREA, "- Mark Convex Area", pc);
		LogLine(ctx, RC_TIMER_MARK_CYLINDER_AREA, "- Mark Cylinder Area", pc);
		LogLine(ctx, RC_TIMER_BUILD_DISTANCEFIELD, "- Build Distance Field", pc);
		LogLine(ctx, RC_TIMER_BUILD_DISTANCEFIELD_DIST, "    - Distance", pc);
		LogLine(ctx, RC_TIMER_BUILD_DISTANCEFIELD_BLUR, "    - Blur", pc);
		LogLine(ctx, RC_TIMER_BUILD_REGIONS, "- Build Regions", pc);
		LogLine(ctx, RC_TIMER_BUILD_REGIONS_WATERSHED, "    - Watershed", pc);
		LogLine(ctx, RC_TIMER_BUILD_REGIONS_EXPAND, "      - Expand", pc);
		LogLine(ctx, RC_TIMER_BUILD_REGIONS_FLOOD, "      - Find Basins", pc);
		LogLine(ctx, RC_TIMER_BUILD_REGIONS_FILTER, "    - Filter", pc);
		LogLine(ctx, RC_TIMER_BUILD_LAYERS, "- Build Layers", pc);
		LogLine(ctx, RC_TIMER_BUILD_CONTOURS, "- Build Contours", pc);
		LogLine(ctx, RC_TIMER_BUILD_CONTOURS_TRACE, "    - Trace", pc);
		LogLine(ctx, RC_TIMER_BUILD_CONTOURS_SIMPLIFY, "    - Simplify", pc);
		LogLine(ctx, RC_TIMER_BUILD_POLYMESH, "- Build Polymesh", pc);
		LogLine(ctx, RC_TIMER_BUILD_POLYMESHDETAIL, "- Build Polymesh Detail", pc);
		LogLine(ctx, RC_TIMER_MERGE_POLYMESH, "- Merge Polymeshes", pc);
		LogLine(ctx, RC_TIMER_MERGE_POLYMESHDETAIL, "- Merge Polymesh Details", pc);
		ctx.log(RC_LOG_PROGRESS, "=== TOTAL:\t%.2fms", totalTimeUsec / 1000.0f);		
	}

	InputGeom* NavMeshBuilder::GetInputGeom()
	{
		return s_geom;
	}

	bool NavMeshBuilder::NavMeshIsValid()
	{
		return s_navMesh && s_navQuery;
	}

	dtNavMesh* NavMeshBuilder::GetNavMesh()
	{
		return s_navMesh;
	}

	void NavMeshBuilder::CleanUp()
	{
		delete[] s_triareas;
		s_triareas = 0;
		rcFreeHeightField(s_solid);
		s_solid = 0;
		rcFreeCompactHeightfield(s_chf);
		s_chf = 0;
		rcFreeContourSet(s_cset);
		s_cset = 0;
		rcFreePolyMesh(s_pmesh);
		s_pmesh = 0;
		rcFreePolyMeshDetail(s_dmesh);
		s_dmesh = 0;
	}

	dtNavMeshQuery* NavMeshBuilder::GetNavMeshQuery()
	{
		return s_navQuery;
	}

}