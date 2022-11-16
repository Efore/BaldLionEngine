#pragma once
#include "BaldLion/Core/Containers/HashTable.h"
#include "BaldLion/Core/Containers/Heap.h"
#include "NavigationPolygon.h"

#include <glm/glm.hpp>
#include <set>

namespace BaldLion::AI::Navigation
{
	struct AStarNode
	{
		ui32 nodeID;
		float hScore;
		float gScore;

		float FScore() const { return hScore + gScore; }
		bool operator<(const AStarNode& other) const { return this->FScore() < other.FScore(); }
		bool operator==(const AStarNode& other) const { return nodeID == other.nodeID; }

		AStarNode& operator= (const AStarNode& other)
		{
			nodeID = other.nodeID;
			hScore = other.hScore;
			gScore = other.gScore;
			return *this;
		}

		void Print()
		{
			BL_LOG_CORE_INFO("Node ID: {0}, FScore: {1}", nodeID, FScore());
		}
	};

	struct FindAStarNode {

		FindAStarNode(ui32 node) : nodeID(node) {}
		bool operator()(const AStarNode& other) {
			return other.nodeID == nodeID;
		}

		ui32 nodeID;
	};

	struct AStarResult {

		i32 bestPreviousNodeID;
		float cost;
	};

	class AStarPathfinding
	{

	public:

		static bool FindPath(const NavigationMeshData& navMeshData, ui32 initialNavigationPolygonIdx, ui32 finalNavigationPoylgonIdx, DynamicArray<glm::vec3>& pathResult)
		{

			DynamicArray<ui32> closedNodes(Memory::AllocationType::Linear_Frame, navMeshData.polygons.Size());
			Heap<AStarNode> openNodes(Memory::AllocationType::Linear_Frame, navMeshData.polygons.Size());

			DynamicArray<AStarResult> polygonsBestCosts(Memory::AllocationType::Linear_Frame, navMeshData.polygons.Size());
			polygonsBestCosts.Populate({ -1, FLT_MAX });

			const NavigationPolygon& lastPolygon = navMeshData.polygons[finalNavigationPoylgonIdx];
			float heuristic = glm::length(navMeshData.polygons[initialNavigationPolygonIdx].center - lastPolygon.center);

			AStarNode initialNode{
				initialNavigationPolygonIdx,
				heuristic,
				0.0f
			};

			openNodes.Emplace(initialNode);

			while (openNodes.Size() > 0)
			{
				const AStarNode currentNode = openNodes.Pop();

				if (currentNode.nodeID == finalNavigationPoylgonIdx)
				{
					AStarPathfinding::FillPathResult(navMeshData, initialNavigationPolygonIdx, finalNavigationPoylgonIdx, polygonsBestCosts, pathResult);
					return true;
				}

				const float currentGScore = currentNode.gScore;

				const NavigationPolygon& currentNavigationPolygon = navMeshData.polygons[currentNode.nodeID];

				BL_DYNAMICARRAY_FOREACH(currentNavigationPolygon.adjacentPolygonsIndices)
				{
					const ui32 adjacentPolygonIdx = currentNavigationPolygon.adjacentPolygonsIndices[i];

					if (closedNodes.Contains(adjacentPolygonIdx))
						continue;

					const NavigationPolygon& adjacentPolygon = navMeshData.polygons[adjacentPolygonIdx];
					float newGScore = currentGScore + currentNavigationPolygon.adjacentPolygonsData[i].distanceBetweenCenters;
					heuristic = glm::length(navMeshData.polygons[adjacentPolygonIdx].center - lastPolygon.center);

					AStarNode adjacentNode{
						adjacentPolygonIdx,
						heuristic,
						newGScore
					};

					i32 indexInOpenNodes = openNodes.FindIndex(adjacentNode);
					if (indexInOpenNodes < 0 || openNodes.GetAtIndex(indexInOpenNodes)->gScore > newGScore)
					{
						openNodes.Emplace(adjacentNode);
					}

					if (currentGScore < polygonsBestCosts[adjacentPolygonIdx].cost)
					{
						polygonsBestCosts[adjacentPolygonIdx].bestPreviousNodeID = currentNode.nodeID;
						polygonsBestCosts[adjacentPolygonIdx].cost = currentGScore;
					}
				}

				closedNodes.EmplaceBack(currentNode.nodeID);
			}

			return false;
		}

	private:

		static void FillPathResult(const NavigationMeshData& navMeshData, ui32 initialNavigationPolygonIdx, ui32 finalNavigationPoylgonIdx, DynamicArray<AStarResult>& polygonsBestCosts, DynamicArray<glm::vec3>& pathResult)
		{
			pathResult.EmplaceBack(navMeshData.polygons[finalNavigationPoylgonIdx].center);

			const glm::vec3 goalPosition = navMeshData.polygons[initialNavigationPolygonIdx].center;

			ui32 nextIndex = finalNavigationPoylgonIdx;
			while (nextIndex != initialNavigationPolygonIdx)
			{
				const ui32 bestPrevious = polygonsBestCosts[nextIndex].bestPreviousNodeID;
				const ui32 indexOfBestAdjacent = navMeshData.polygons[nextIndex].adjacentPolygonsIndices.FindIndex(bestPrevious);

				const ui32 vertexIndex1 = navMeshData.polygons[nextIndex].adjacentPolygonsData[indexOfBestAdjacent].firstVertexIndex;
				const ui32 vertexIndex2 = navMeshData.polygons[nextIndex].adjacentPolygonsData[indexOfBestAdjacent].secondVertexIndex;

				const glm::vec3 vertex1 = navMeshData.polygonVertices[vertexIndex1];
				const glm::vec3 vertex2 = navMeshData.polygonVertices[vertexIndex2];

				const glm::vec3 prevPosition = pathResult[pathResult.Size() - 1];

				if ((glm::length(vertex1 - prevPosition) + glm::length(vertex1 - goalPosition)) < (glm::length(vertex2 - prevPosition) + glm::length(vertex2 - goalPosition)))
				{
					pathResult.EmplaceBack(vertex1);
				}
				else 
				{
					pathResult.EmplaceBack(vertex2);
				}				

				nextIndex = bestPrevious;
			}

			pathResult.EmplaceBack(goalPosition);
		}

	};
}
