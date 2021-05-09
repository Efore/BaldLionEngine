#include "blpch.h"
#include "PlaneMesh.h"

namespace BaldLion
{
	namespace Rendering
	{

		PlaneMesh::PlaneMesh(Material* material, AABB aabb, const glm::mat4& worldTransform, bool isStatic, float size) : 
			Mesh(material, aabb, worldTransform, isStatic), 
			m_size(size)			
		{
		}		

		void PlaneMesh::SetUpPlane()
		{
			const ui32 rows = (m_size / m_gridSize) + 1;
			const ui32 columns = (m_size / m_gridSize) + 1;
			const ui32 numVertices = rows * columns;
			const glm::vec3 initialPoint(-m_size * 0.5f, 0.0f, -m_size * 0.5f);

			DynamicArray<Vertex> vertices(Memory::AllocationType::FreeList_Renderer, rows * columns);

			for (ui32 i = 0; i < numVertices; ++i)
			{
				const glm::vec3 position = initialPoint + glm::vec3(m_gridSize * (i % columns), 0.0f, m_gridSize * (ui32)(i / columns));				
				vertices.EmplaceBack(
					position,
					glm::vec3(1.0f),
					glm::vec3(0.0f, 1.0f, 0.0f),
					glm::vec2((i % columns)/(columns - 1) , (i / columns)/(rows - 1)),
					glm::vec3(0.0f, 0.0f, 1.0f),
					glm::vec3(1.0f, 0.0f, 0.0f)
				);
			}

			const ui32 numIndices = (rows - 1) * (columns - 1) * 6;

			DynamicArray<ui32> indices(Memory::AllocationType::FreeList_Renderer, numIndices);

			for (ui32 i = 0; i < rows - 1; ++i)
			{
				for (ui32 j = 0; j < columns - 1; ++j)
				{
					indices.EmplaceBack((i * columns) + j);
					indices.EmplaceBack(((i + 1) * columns) + j);
					indices.EmplaceBack((i * columns) + j + 1);

					indices.EmplaceBack(((i + 1) * columns) + j);
					indices.EmplaceBack(((i + 1) * columns) + j + 1);
					indices.EmplaceBack((i * columns) + j + 1);
				}
			}

			SetUpMesh(vertices, indices);
		}
	}
}
