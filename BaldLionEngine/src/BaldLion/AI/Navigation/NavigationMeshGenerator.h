#pragma once
#include "BaldLion/ECS/Components/ECSMeshComponent.h"
#include "NavigationPolygon.h"

namespace BaldLion::AI::Navigation {

	class NavigationMeshGenerator {

	public:

		static void Init();
		static void Stop();

		static void BakeNavMesh();

		static void AddAllStaticEntities();

		static void AddEntityToBake(ECS::ECSEntityID entity);
		static void RemoveEntityToBake(ECS::ECSEntityID entity);

		static void ClearMeshComponentsList();

		static const DynamicArray<ECS::ECSEntityID>& GetEntitiesToBake();

		static bool FindPath(ui32 initialPolygonIndex, ui32 finalPolygonIndex, DynamicArray<glm::vec3>& path);

		static const NavigationMeshData& GetNavMeshData() { return s_navmeshData; }
		static bool IsNavMeshDataGenerated() { return s_navmeshDataGenerated; }

	private:

		static i32 PolygonsCanBeMerged(const NavigationPolygon& a, const NavigationPolygon& b, float maxDot = 0.01f);
		static bool MergePolygons(NavigationMeshData& polygonContainer, NavigationMeshData& resultContainer, ui32 firstPolygonIndex, ui32 secondPolygonIndex, i32 lastIndexOfAdjacence);
		static void MergePolygonBuffers(NavigationMeshData& bufferA, NavigationMeshData& bufferB);
		static void CopyNavigationMeshData(const NavigationMeshData& origin, NavigationMeshData& destination);
		static void GenerateNavMeshGraph();

	private:
		static DynamicArray<ECS::ECSEntityID> s_entitiesToBake;
		static NavigationMeshData s_navmeshData;
		static bool s_navmeshDataGenerated;
		static float s_maxDot;
	};

}
