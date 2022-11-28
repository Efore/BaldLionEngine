#pragma once
#include "BaldLion/Core/Containers/DynamicArray.h"
#include <glm/glm.hpp>

namespace BaldLion::AI::Navigation
{

	struct PolygonAdjacency 
	{
		ui32 firstVertexIndex;
		ui32 secondVertexIndex;
		float distanceBetweenCenters;
	};

	struct NavigationPolygon
	{		
		PolygonAdjacency adjacentPolygonsData[3];
		
		glm::vec3 center;
		glm::vec3 normal;

		ui32 vertexIndices[3];
		ui32 adjacentPolygonIndices[3];
		ui32 edgesIndices[3];

	};

	struct NavigationPolygonEdge 
	{
		ui32 vertexIndexA;
		ui32 vertexIndexB;
		bool isNavigationMeshEdge;

		bool operator==(const NavigationPolygonEdge& other)
		{
			return (this->vertexIndexA == other.vertexIndexA && this->vertexIndexB == other.vertexIndexB) ||
				(this->vertexIndexA == other.vertexIndexB && this->vertexIndexB == other.vertexIndexA);
		}
	};

	struct NavigationMeshData {

		DynamicArray<glm::vec3> polygonVertices;
		DynamicArray<NavigationPolygon> polygons;
		DynamicArray<NavigationPolygonEdge> polygonEdges;

		~NavigationMeshData()
		{
			polygonVertices.Delete();
			polygons.Delete();
			polygonEdges.Delete();
		}

	};
}
