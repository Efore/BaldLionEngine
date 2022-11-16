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
		glm::vec3 center;
		glm::vec3 normal;

		DynamicArray<ui32> vertexIndices;
		DynamicArray<ui32> adjacentPolygonsIndices;
		DynamicArray<PolygonAdjacency> adjacentPolygonsData;
		
		~NavigationPolygon() 
		{
			vertexIndices.Delete();
			adjacentPolygonsIndices.Delete();
			adjacentPolygonsData.Delete();
		}
	};

	struct NavigationMeshData {

		DynamicArray<glm::vec3> polygonVertices;
		DynamicArray<NavigationPolygon> polygons;

		~NavigationMeshData()
		{
			polygonVertices.Delete();
			polygons.Delete();
		}

	};
}
