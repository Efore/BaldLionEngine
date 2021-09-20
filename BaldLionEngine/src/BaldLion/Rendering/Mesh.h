#pragma once
#include "Vertex.h"
#include "VertexArray.h"
#include "Material.h"
#include "BaldLion/Utils/GeometryUtils.h"
#include "GeometryData.h"
#include "BaldLion/ECS/Components/ECSMeshComponent.h"
#include "BaldLion/ECS/ECSManager.h"

namespace BaldLion
{
	using namespace GeometryUtils;

	namespace Rendering
	{
		class Mesh {

		public:
			Mesh(Material* material);
			~Mesh();

			void SetUpMesh(const DynamicArray<Vertex>& vertices, const DynamicArray<ui32>& indices);
			void Draw() const;

			inline const Material* GetMaterial() const { return m_material; }
			inline Material* GetMaterial() { return m_material; }

			inline const DynamicArray<Vertex>& GetVertices() const { return m_geometryData->vertices; }
			inline DynamicArray<Vertex>& GetVertices() { return m_geometryData->vertices; }

			inline const DynamicArray<ui32>& GetIndices() const { return m_geometryData->indices; }
			inline DynamicArray<ui32>& GetIndices() { return m_geometryData->indices; }

			inline const VertexArray* GetVertexArray() const { return m_vertexArray; }
			inline VertexArray* GetVertexArray() { return m_vertexArray; }

			ECS::ECSMeshComponent* GenerateMeshComponent(ECS::ECSManager* ecsManager, bool isStatic);

		protected:

			GeometryData* m_geometryData;
			VertexArray* m_vertexArray;
			Material* m_material;
		};
	}
}