#pragma once

#include "BaldLion/Core/Containers/DynamicArray.h"
#include "BaldLion/Rendering/Vertex.h"
#include "BaldLion/Rendering/Buffer.h"

namespace BaldLion {
	namespace Rendering {		

		struct GeometryData {
			
			DynamicArray<Vertex> vertices;
			DynamicArray<ui32> indices;
			ui32 verticesCurrentIndex = 0;
			ui32 indicesCurrentIndex = 0;
			std::mutex geometryDataMutex;
			VertexArray* vertexArray = nullptr;

			GeometryData(){}

			GeometryData(const DynamicArray<Vertex>& vert, const DynamicArray<ui32>& ind) : vertices(vert), indices(ind), verticesCurrentIndex(0), indicesCurrentIndex(0), vertexArray(nullptr){}

			~GeometryData(){}

			void DeleteGeometryData()
			{
				vertices.Delete();
				indices.Delete();
				verticesCurrentIndex = 0;
				indicesCurrentIndex = 0;
				if (vertexArray != nullptr)
				{
					VertexArray::Destroy(vertexArray);
				}
			}

			void ClearGeometryData()
			{
				vertices.Clear();
				indices.Clear();
				verticesCurrentIndex = 0;
				indicesCurrentIndex = 0;
			}
		};

	}
}