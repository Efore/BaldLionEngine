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