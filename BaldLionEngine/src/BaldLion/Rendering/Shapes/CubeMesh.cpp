#include "blpch.h"
#include "CubeMesh.h"
#include "BaldLion/Utils/MathUtils.h"

namespace BaldLion
{
	namespace Rendering
	{
		CubeMesh::CubeMesh(Material* material, float size, const std::string& meshName) :
			Mesh(material,meshName),	m_size(size)
		{

		} 

		void CubeMesh::SetUpCube()
		{
			const float halfSize = m_size * 0.5f;			
			const ui32 numFaces = 6;

			DynamicArray<Vertex> vertices(Memory::AllocationType::FreeList_Renderer, 24);		
			DynamicArray<ui32> indices(Memory::AllocationType::FreeList_Renderer, 36);

			glm::vec3 faceNormals[numFaces] = {
				-MathUtils::Vector3UnitZ,
				MathUtils::Vector3UnitZ,
				MathUtils::Vector3UnitX,
				-MathUtils::Vector3UnitX,
				MathUtils::Vector3UnitY,
				-MathUtils::Vector3UnitY
			};

			glm::vec2 textCoords[4] = {
				glm::vec2(1.0f, 0.0f),
				glm::vec2(1.0f, 1.0f),
				glm::vec2(0.0f, 1.0f),
				glm::vec2(0.0f, 0.0f)
			};

			for (ui32 i = 0; i < numFaces; ++i)
			{
				ui32 currentIndex = vertices.Size();

				indices.EmplaceBack(currentIndex + 0);
				indices.EmplaceBack(currentIndex + 1);
				indices.EmplaceBack(currentIndex + 2);
				indices.EmplaceBack(currentIndex + 0);
				indices.EmplaceBack(currentIndex + 2);
				indices.EmplaceBack(currentIndex + 3);

				const glm::vec3& faceNormal = faceNormals[i];
				const glm::vec3 baseForCrossProd = (i >= 4) ? MathUtils::Vector3UnitZ : MathUtils::Vector3UnitY;
				const glm::vec3 side1Normal = glm::cross(faceNormal, baseForCrossProd);
				const glm::vec3 side2Normal = glm::cross(faceNormal, side1Normal);				
				const glm::vec3 tangent = (i < 4) ? -side1Normal : i == 4 ? baseForCrossProd : -baseForCrossProd;

				const glm::vec3 v0 = (faceNormal - side1Normal + side2Normal) * halfSize;
				const glm::vec3 v1 = (faceNormal - side1Normal - side2Normal) * halfSize;
				const glm::vec3 v2 = (faceNormal + side1Normal - side2Normal) * halfSize;
				const glm::vec3 v3 = (faceNormal + side1Normal + side2Normal) * halfSize;

				vertices.EmplaceBack(
					v0,
					faceNormal,
					textCoords[0],
					tangent
				);

				vertices.EmplaceBack(
					v1,
					faceNormal,
					textCoords[1],
					tangent
				);

				vertices.EmplaceBack(
					v2,
					faceNormal,
					textCoords[2],
					tangent
				);

				vertices.EmplaceBack(
					v3,
					faceNormal,
					textCoords[3],
					tangent
				);
			}

			SetUpMesh(vertices, indices);
		}

	}
}