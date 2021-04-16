#pragma once

#include "BaldLion/Core/Containers/DynamicArray.h"
#include "BaldLion/Rendering/Vertex.h"

namespace BaldLion {
	namespace Rendering {

		struct GeometryData {

			glm::mat4 worldTransform;
			DynamicArray<Vertex> vertices;
			DynamicArray<ui32> indices;

			GeometryData(){}

			GeometryData(AllocationType allocationType, ui32 numVertices, ui32 numIndices, const glm::mat4& worldT) : worldTransform(worldT)
			{
				vertices = DynamicArray<Vertex>(allocationType, numVertices);
				indices = DynamicArray<ui32>(allocationType, numIndices);
			}

			GeometryData(const DynamicArray<Vertex>& vert, const DynamicArray<ui32>& ind, const glm::mat4& worldT) : vertices(vert), indices(ind), worldTransform(worldT){}

			~GeometryData(){}

			void ClearGeometryData()
			{
				vertices.Clear();
				indices.Clear();
			}
		};

	}
}