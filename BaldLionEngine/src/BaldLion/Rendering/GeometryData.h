#pragma once

#include "BaldLion/Core/Containers/DynamicArray.h"
#include "BaldLion/Rendering/Vertex.h"

namespace BaldLion {
	namespace Rendering {

		struct GeometryData {
			
			DynamicArray<Vertex> vertices;
			DynamicArray<ui32> indices;

			GeometryData(){}

			GeometryData(const DynamicArray<Vertex>& vert, const DynamicArray<ui32>& ind) : vertices(vert), indices(ind){}

			~GeometryData(){}

			void ClearGeometryData()
			{
				vertices.Delete();
				indices.Delete();
			}
		};

	}
}