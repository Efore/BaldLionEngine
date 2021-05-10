#include "blpch.h"
#include "SphereMesh.h"
#include "BaldLion/Utils/MathUtils.h"
#include <glm/gtc/constants.hpp>

namespace BaldLion
{
	namespace Rendering
	{
		SphereMesh::SphereMesh(Material* material, AABB aabb, const glm::mat4& worldTransform, bool isStatic, float radius, ui32 rings, ui32 sectors) :
			Mesh(material, aabb, worldTransform, isStatic),
			m_radius(radius),
			m_rings(rings),
			m_sectors(sectors)
		{

		}

		void SphereMesh::SetUpSphere()
		{
			DynamicArray<Vertex> vertices(Memory::AllocationType::FreeList_Renderer, m_rings * m_sectors);
			DynamicArray<ui32> indices(Memory::AllocationType::FreeList_Renderer, m_rings * m_sectors * 6);

			const float ringSize = 1.0f / (m_rings - 1); //in units
			const float sectorSize = 1.0f / (m_sectors - 1); //in radians

			for (ui32 i = 0; i < m_rings; i++)
			{
				for (ui32 j = 0; j < m_sectors; j++)
				{
					const float ringRadius = sinf(glm::pi<float>() * i * ringSize);
					const glm::vec3 vertexPos(cosf(glm::two_pi<float>() * j * sectorSize) * ringRadius, sinf(-glm::half_pi<float>() + glm::pi<float>() * i * ringSize), sinf(glm::pi<float>() * 2 * j * sectorSize) * -ringRadius);

					vertices.EmplaceBack(
						vertexPos * m_radius,
						glm::vec3(1.0f),
						vertexPos,
						glm::vec2(i / m_rings, j / m_sectors),
						glm::vec3(1.0f)
					);
				}
			}

			for (ui32 i = 0; i < (m_rings - 1); ++i)
			{
				for (ui32 j = 0; j < (m_sectors - 1); ++j)
				{
					indices.EmplaceBack((i * m_sectors) + j);
					indices.EmplaceBack((i * m_sectors) + j + 1);					
					indices.EmplaceBack(((i + 1) * m_sectors) + j);

				 	indices.EmplaceBack((i * m_sectors) + j + 1);
					indices.EmplaceBack(((i + 1) * m_sectors) + j + 1);
					indices.EmplaceBack(((i + 1) * m_sectors) + j);
				}
			}

			SetUpMesh(vertices, indices);
		}
	}
}