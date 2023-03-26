#include "blpch.h"
#include "NavMeshBuilder.h"
#include "RecastClasses/InputGeom.h"
#include "DetourNavMeshBuilder.h"
#include "BaldLion/Core/JobManagement/JobManager.h"


namespace BaldLion::AI::Navigation
{
	rcPolyMeshDetail* NavMeshBuilder::m_dmesh = nullptr;
	rcPolyMesh* NavMeshBuilder::m_pmesh = nullptr;
	rcContourSet* NavMeshBuilder::m_cset = nullptr;
	rcCompactHeightfield* NavMeshBuilder::m_chf = nullptr;
	rcHeightfield* NavMeshBuilder::m_solid = nullptr;
	unsigned char* NavMeshBuilder::m_triareas = nullptr;
	
	dtNavMeshQuery* NavMeshBuilder::m_navQuery = nullptr;
	dtNavMesh* NavMeshBuilder::m_navMesh = nullptr;
	InputGeom* NavMeshBuilder::m_geom = nullptr;

	BuildContext NavMeshBuilder::m_ctx;
	rcConfig NavMeshBuilder::m_cfg;
	NavMeshBuildSettings NavMeshBuilder::navMeshConfig;

	float NavMeshBuilder::m_lastBuiltTileBmax[3];
	float NavMeshBuilder::m_lastBuiltTileBmin[3];

	float NavMeshBuilder::m_tileMemUsage;
	float NavMeshBuilder::m_tileBuildTime;
	float NavMeshBuilder::m_totalBuildTimeMs;
	int NavMeshBuilder::m_tileTriCount;

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

		m_navQuery = dtAllocNavMeshQuery();

		m_totalBuildTimeMs = 0;
		m_triareas = 0;
		m_solid = 0;
		m_chf = 0;
		m_cset = 0;
		m_pmesh = 0;
		m_dmesh = 0;		
		m_tileBuildTime = 0;
		m_tileMemUsage = 0;
		m_tileTriCount = 0;

		memset(m_lastBuiltTileBmin, 0, sizeof(m_lastBuiltTileBmin));
		memset(m_lastBuiltTileBmax, 0, sizeof(m_lastBuiltTileBmax));
	}

	void NavMeshBuilder::Stop()
	{
		CleanUp();

		if (m_navQuery != nullptr)
		{
			dtFreeNavMeshQuery(m_navQuery);
			m_navQuery = nullptr;
		}

		if (m_navMesh != nullptr)
		{
			dtFreeNavMesh(m_navMesh);
			m_navMesh = nullptr;
		}
	}

	bool NavMeshBuilder::LoadGeom(const char* filepath)
	{
		if (m_ctx.GetInitialized())
		{
			m_ctx.Stop();
		}
		m_ctx.Init();

		if (m_geom != nullptr)
		{
			MemoryManager::Delete(m_geom);
		}

		m_geom = MemoryManager::New<InputGeom>("NavMesh Geom", AllocationType::FreeList_ECS);

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

		m_geom->saveGeomSet(&buildSettings);

		if (!m_geom->load(&m_ctx, filepath))
		{
			MemoryManager::Delete(m_geom);
			m_geom = nullptr;
			return false;
		}
	}

	bool NavMeshBuilder::BuildNavMesh()
	{	
		if (m_geom == nullptr)
		{
			return false;
		}

		InternalBuildNavMesh(true);

		return true;
	}	
	
	bool NavMeshBuilder::InternalBuildNavMesh(bool parallelize)
	{
		if (!m_geom || !m_geom->getMesh())
		{
			m_ctx.log(RC_LOG_ERROR, "buildTiledNavigation: No vertices and triangles.");
			return false;
		}

		dtFreeNavMesh(m_navMesh);

		m_navMesh = dtAllocNavMesh();
		if (!m_navMesh)
		{
			m_ctx.log(RC_LOG_ERROR, "buildTiledNavigation: Could not allocate navmesh.");
			return false;
		}

		dtNavMeshParams params;
		rcVcopy(params.orig, m_geom->getNavMeshBoundsMin());
		params.tileWidth = navMeshConfig.tileSize *  navMeshConfig.cellSize;
		params.tileHeight = navMeshConfig.tileSize *  navMeshConfig.cellSize;
		params.maxTiles = navMeshConfig.maxTiles;
		params.maxPolys = navMeshConfig.maxPolys;

		dtStatus status;

		status = m_navMesh->init(&params);
		if (dtStatusFailed(status))
		{
			m_ctx.log(RC_LOG_ERROR, "buildTiledNavigation: Could not init navmesh.");
			return false;
		}

		status = m_navQuery->init(m_navMesh, 2048);
		if (dtStatusFailed(status))
		{
			m_ctx.log(RC_LOG_ERROR, "buildTiledNavigation: Could not init Detour navmesh query");
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
		if (!m_geom) return;
		if (!m_navMesh) return;

		const float* bmin = m_geom->getNavMeshBoundsMin();
		const float* bmax = m_geom->getNavMeshBoundsMax();
		int gw = 0, gh = 0;
		rcCalcGridSize(bmin, bmax, navMeshConfig.cellSize, &gw, &gh);
		const int ts = (int)navMeshConfig.tileSize;
		const int tw = (gw + ts - 1) / ts;
		const int th = (gh + ts - 1) / ts;
		const float tcs = navMeshConfig.tileSize * navMeshConfig.cellSize;

		// Start the build process.
		m_ctx.startTimer(RC_TIMER_TEMP);

		for (int y = 0; y < th; ++y)
		{
			for (int x = 0; x < tw; ++x)
			{
				m_lastBuiltTileBmin[0] = bmin[0] + x * tcs;
				m_lastBuiltTileBmin[1] = bmin[1];
				m_lastBuiltTileBmin[2] = bmin[2] + y * tcs;

				m_lastBuiltTileBmax[0] = bmin[0] + (x + 1)*tcs;
				m_lastBuiltTileBmax[1] = bmax[1];
				m_lastBuiltTileBmax[2] = bmin[2] + (y + 1)*tcs;

				int dataSize = 0;
				unsigned char* data = BuildTileMesh(x, y, m_lastBuiltTileBmin, m_lastBuiltTileBmax, dataSize);
				if (data)
				{
					// Remove any previous data (navmesh owns and deletes the data).
					m_navMesh->removeTile(m_navMesh->getTileRefAt(x, y, 0), 0, 0);
					// Let the navmesh own the data.
					dtStatus status = m_navMesh->addTile(data, dataSize, DT_TILE_FREE_DATA, 0, 0);
					if (dtStatusFailed(status))
						dtFree(data);
				}
			}
		}

		// Start the build process.	
		m_ctx.stopTimer(RC_TIMER_TEMP);

		m_totalBuildTimeMs = m_ctx.getAccumulatedTime(RC_TIMER_TEMP) / 1000.0f;
	}

	unsigned char* NavMeshBuilder::BuildTileMesh(const int tx, const int ty, const float* bmin, const float* bmax, int& dataSize)
	{
		if (!m_geom || !m_geom->getMesh() || !m_geom->getChunkyMesh())
		{
			m_ctx.log(RC_LOG_ERROR, "buildNavigation: Input mesh is not specified.");
			return 0;
		}

		m_tileMemUsage = 0;
		m_tileBuildTime = 0;

		CleanUp();

		const float* verts = m_geom->getMesh()->getVerts();
		const int nverts = m_geom->getMesh()->getVertCount();
		const int ntris = m_geom->getMesh()->getTriCount();
		const rcChunkyTriMesh* chunkyMesh = m_geom->getChunkyMesh();

		// Init build configuration from GUI
		memset(&m_cfg, 0, sizeof(m_cfg));
		m_cfg.cs = navMeshConfig.cellSize;
		m_cfg.ch = navMeshConfig.cellHeight;
		m_cfg.walkableSlopeAngle = navMeshConfig.agentMaxSlope;
		m_cfg.walkableHeight = (int)ceilf(navMeshConfig.agentHeight / m_cfg.ch);
		m_cfg.walkableClimb = (int)floorf(navMeshConfig.agentMaxClimb / m_cfg.ch);
		m_cfg.walkableRadius = (int)ceilf(navMeshConfig.agentRadius / m_cfg.cs);
		m_cfg.maxEdgeLen = (int)(navMeshConfig.edgeMaxLen / navMeshConfig.cellSize);
		m_cfg.maxSimplificationError = navMeshConfig.edgeMaxError;
		m_cfg.minRegionArea = (int)rcSqr(navMeshConfig.regionMinSize);		// Note: area = size*size
		m_cfg.mergeRegionArea = (int)rcSqr(navMeshConfig.regionMergeSize);	// Note: area = size*size
		m_cfg.maxVertsPerPoly = (int)navMeshConfig.vertsPerPoly;
		m_cfg.tileSize = (int)navMeshConfig.tileSize;
		m_cfg.borderSize = m_cfg.walkableRadius + 3; // Reserve enough padding.
		m_cfg.width = m_cfg.tileSize + m_cfg.borderSize * 2;
		m_cfg.height = m_cfg.tileSize + m_cfg.borderSize * 2;
		m_cfg.detailSampleDist = navMeshConfig.detailSampleDist < 0.9f ? 0 : navMeshConfig.cellSize * navMeshConfig.detailSampleDist;
		m_cfg.detailSampleMaxError = navMeshConfig.cellHeight * navMeshConfig.detailSampleDist;

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
		rcVcopy(m_cfg.bmin, bmin);
		rcVcopy(m_cfg.bmax, bmax);
		m_cfg.bmin[0] -= m_cfg.borderSize*m_cfg.cs;
		m_cfg.bmin[2] -= m_cfg.borderSize*m_cfg.cs;
		m_cfg.bmax[0] += m_cfg.borderSize*m_cfg.cs;
		m_cfg.bmax[2] += m_cfg.borderSize*m_cfg.cs;

		// Reset build times gathering.
		m_ctx.resetTimers();

		// Start the build process.
		m_ctx.startTimer(RC_TIMER_TOTAL);

		m_ctx.log(RC_LOG_PROGRESS, "Building navigation:");
		m_ctx.log(RC_LOG_PROGRESS, " - %d x %d cells", m_cfg.width, m_cfg.height);
		m_ctx.log(RC_LOG_PROGRESS, " - %.1fK verts, %.1fK tris", nverts / 1000.0f, ntris / 1000.0f);

		// Allocate voxel heightfield where we rasterize our input data to.
		m_solid = rcAllocHeightfield();
		if (!m_solid)
		{
			m_ctx.log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
			return 0;
		}
		if (!rcCreateHeightfield(&m_ctx, *m_solid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch))
		{
			m_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
			return 0;
		}

		// Allocate array that can hold triangle flags.
		// If you have multiple meshes you need to process, allocate
		// and array which can hold the max number of triangles you need to process.
		m_triareas = new unsigned char[chunkyMesh->maxTrisPerChunk];
		if (!m_triareas)
		{
			m_ctx.log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", chunkyMesh->maxTrisPerChunk);
			return 0;
		}

		float tbmin[2], tbmax[2];
		tbmin[0] = m_cfg.bmin[0];
		tbmin[1] = m_cfg.bmin[2];
		tbmax[0] = m_cfg.bmax[0];
		tbmax[1] = m_cfg.bmax[2];
		int cid[512];// TODO: Make grow when returning too many items.
		const int ncid = rcGetChunksOverlappingRect(chunkyMesh, tbmin, tbmax, cid, 512);
		if (!ncid)
			return 0;

		m_tileTriCount = 0;

		for (int i = 0; i < ncid; ++i)
		{
			const rcChunkyTriMeshNode& node = chunkyMesh->nodes[cid[i]];
			const int* ctris = &chunkyMesh->tris[node.i * 3];
			const int nctris = node.n;

			m_tileTriCount += nctris;

			memset(m_triareas, 0, nctris * sizeof(unsigned char));
			rcMarkWalkableTriangles(&m_ctx, m_cfg.walkableSlopeAngle,
				verts, nverts, ctris, nctris, m_triareas);

			if (!rcRasterizeTriangles(&m_ctx, verts, nverts, ctris, m_triareas, nctris, *m_solid, m_cfg.walkableClimb))
				return 0;
		}

		delete[] m_triareas;
		m_triareas = 0;
		

		// Once all geometry is rasterized, we do initial pass of filtering to
		// remove unwanted overhangs caused by the conservative rasterization
		// as well as filter spans where the character cannot possibly stand.
		rcFilterLowHangingWalkableObstacles(&m_ctx, m_cfg.walkableClimb, *m_solid);
		rcFilterLedgeSpans(&m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid);
		rcFilterWalkableLowHeightSpans(&m_ctx, m_cfg.walkableHeight, *m_solid);

		// Compact the heightfield so that it is faster to handle from now on.
		// This will result more cache coherent data as well as the neighbours
		// between walkable cells will be calculated.
		m_chf = rcAllocCompactHeightfield();
		if (!m_chf)
		{
			m_ctx.log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
			return 0;
		}
		if (!rcBuildCompactHeightfield(&m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid, *m_chf))
		{
			m_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
			return 0;
		}

		
		rcFreeHeightField(m_solid);
		m_solid = 0;
		

		// Erode the walkable area by agent radius.
		if (!rcErodeWalkableArea(&m_ctx, m_cfg.walkableRadius, *m_chf))
		{
			m_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
			return 0;
		}

		// (Optional) Mark areas.
		const ConvexVolume* vols = m_geom->getConvexVolumes();
		for (int i = 0; i < m_geom->getConvexVolumeCount(); ++i)
			rcMarkConvexPolyArea(&m_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *m_chf);


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
			if (!rcBuildDistanceField(&m_ctx, *m_chf))
			{
				m_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not build distance field.");
				return 0;
			}

			// Partition the walkable surface into simple regions without holes.
			if (!rcBuildRegions(&m_ctx, *m_chf, m_cfg.borderSize, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
			{
				m_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not build watershed regions.");
				return 0;
			}
		}
		else if (navMeshConfig.partitionType == SAMPLE_PARTITION_MONOTONE)
		{
			// Partition the walkable surface into simple regions without holes.
			// Monotone partitioning does not need distancefield.
			if (!rcBuildRegionsMonotone(&m_ctx, *m_chf, m_cfg.borderSize, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
			{
				m_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not build monotone regions.");
				return 0;
			}
		}
		else // SAMPLE_PARTITION_LAYERS
		{
			// Partition the walkable surface into simple regions without holes.
			if (!rcBuildLayerRegions(&m_ctx, *m_chf, m_cfg.borderSize, m_cfg.minRegionArea))
			{
				m_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not build layer regions.");
				return 0;
			}
		}

		// Create contours.
		m_cset = rcAllocContourSet();
		if (!m_cset)
		{
			m_ctx.log(RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
			return 0;
		}
		if (!rcBuildContours(&m_ctx, *m_chf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *m_cset))
		{
			m_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
			return 0;
		}

		if (m_cset->nconts == 0)
		{
			return 0;
		}

		// Build polygon navmesh from the contours.
		m_pmesh = rcAllocPolyMesh();
		if (!m_pmesh)
		{
			m_ctx.log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
			return 0;
		}
		if (!rcBuildPolyMesh(&m_ctx, *m_cset, m_cfg.maxVertsPerPoly, *m_pmesh))
		{
			m_ctx.log(RC_LOG_ERROR, "buildNavigation: Could not triangulate contours.");
			return 0;
		}

		// Build detail mesh.
		m_dmesh = rcAllocPolyMeshDetail();
		if (!m_dmesh)
		{
			m_ctx.log(RC_LOG_ERROR, "buildNavigation: Out of memory 'dmesh'.");
			return 0;
		}

		if (!rcBuildPolyMeshDetail(&m_ctx, *m_pmesh, *m_chf,
			m_cfg.detailSampleDist, m_cfg.detailSampleMaxError,
			*m_dmesh))
		{
			m_ctx.log(RC_LOG_ERROR, "buildNavigation: Could build polymesh detail.");
			return 0;
		}
		
		rcFreeCompactHeightfield(m_chf);
		m_chf = 0;
		rcFreeContourSet(m_cset);
		m_cset = 0;		

		unsigned char* navData = 0;
		int navDataSize = 0;
		if (m_cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
		{
			if (m_pmesh->nverts >= 0xffff)
			{
				// The vertex indices are ushorts, and cannot point to more than 0xffff vertices.
				m_ctx.log(RC_LOG_ERROR, "Too many vertices per tile %d (max: %d).", m_pmesh->nverts, 0xffff);
				return 0;
			}

			// Update poly flags from areas.
			for (int i = 0; i < m_pmesh->npolys; ++i)
			{
				if (m_pmesh->areas[i] == RC_WALKABLE_AREA)
					m_pmesh->areas[i] = SAMPLE_POLYAREA_GROUND;

				if (m_pmesh->areas[i] == SAMPLE_POLYAREA_GROUND ||
					m_pmesh->areas[i] == SAMPLE_POLYAREA_GRASS ||
					m_pmesh->areas[i] == SAMPLE_POLYAREA_ROAD)
				{
					m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK;
				}
				else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_WATER)
				{
					m_pmesh->flags[i] = SAMPLE_POLYFLAGS_SWIM;
				}
				else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_DOOR)
				{
					m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
				}
			}

			dtNavMeshCreateParams params;
			memset(&params, 0, sizeof(params));
			params.verts = m_pmesh->verts;
			params.vertCount = m_pmesh->nverts;
			params.polys = m_pmesh->polys;
			params.polyAreas = m_pmesh->areas;
			params.polyFlags = m_pmesh->flags;
			params.polyCount = m_pmesh->npolys;
			params.nvp = m_pmesh->nvp;
			params.detailMeshes = m_dmesh->meshes;
			params.detailVerts = m_dmesh->verts;
			params.detailVertsCount = m_dmesh->nverts;
			params.detailTris = m_dmesh->tris;
			params.detailTriCount = m_dmesh->ntris;
			params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
			params.offMeshConRad = m_geom->getOffMeshConnectionRads();
			params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
			params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
			params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
			params.offMeshConUserID = m_geom->getOffMeshConnectionId();
			params.offMeshConCount = m_geom->getOffMeshConnectionCount();
			params.walkableHeight = navMeshConfig.agentHeight;
			params.walkableRadius = navMeshConfig.agentRadius;
			params.walkableClimb = navMeshConfig.agentMaxClimb;
			params.tileX = tx;
			params.tileY = ty;
			params.tileLayer = 0;
			rcVcopy(params.bmin, m_pmesh->bmin);
			rcVcopy(params.bmax, m_pmesh->bmax);
			params.cs = m_cfg.cs;
			params.ch = m_cfg.ch;
			params.buildBvTree = true;

			if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
			{
				m_ctx.log(RC_LOG_ERROR, "Could not build Detour navmesh.");
				return 0;
			}
		}
		m_tileMemUsage = navDataSize / 1024.0f;

		m_ctx.stopTimer(RC_TIMER_TOTAL);

		// Show performance stats.
		//duLogBuildTimes(m_ctx, m_ctx.getAccumulatedTime(RC_TIMER_TOTAL));
		m_ctx.log(RC_LOG_PROGRESS, ">> Polymesh: %d vertices  %d polygons", m_pmesh->nverts, m_pmesh->npolys);

		m_tileBuildTime = m_ctx.getAccumulatedTime(RC_TIMER_TOTAL) / 1000.0f;

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

	const InputGeom* NavMeshBuilder::GetInputGeom()
	{
		return m_geom;
	}

	bool NavMeshBuilder::NavMeshIsValid()
	{
		return m_navMesh && m_navQuery;
	}

	const dtNavMesh* NavMeshBuilder::GetNavMesh()
	{
		return m_navMesh;
	}

	void NavMeshBuilder::CleanUp()
	{
		delete[] m_triareas;
		m_triareas = 0;
		rcFreeHeightField(m_solid);
		m_solid = 0;
		rcFreeCompactHeightfield(m_chf);
		m_chf = 0;
		rcFreeContourSet(m_cset);
		m_cset = 0;
		rcFreePolyMesh(m_pmesh);
		m_pmesh = 0;
		rcFreePolyMeshDetail(m_dmesh);
		m_dmesh = 0;
	}


}