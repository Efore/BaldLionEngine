#pragma once

#include "BaldLion/Core/Containers/DynamicArray.h"
#include "BaldLion/Rendering/Vertex.h"

namespace BaldLion {
	namespace Rendering {

		struct GeometryData {
			
			DynamicArray<Vertex> vertices;
			DynamicArray<ui32> indices;
			ui32 verticesCurrentIndex;
			ui32 indicesCurrentIndex;
			std::mutex geometryDataMutex;


			GeometryData(){}

			GeometryData(const DynamicArray<Vertex>& vert, const DynamicArray<ui32>& ind) : vertices(vert), indices(ind), verticesCurrentIndex(0), indicesCurrentIndex(0){}

			~GeometryData(){}

			void DeleteGeometryData()
			{
				vertices.Delete();
				indices.Delete();
				verticesCurrentIndex = 0;
				indicesCurrentIndex = 0;
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