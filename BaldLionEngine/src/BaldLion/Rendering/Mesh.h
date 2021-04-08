#pragma once
#include "Vertex.h"
#include "VertexArray.h"
#include "Material.h"
#include "BaldLion/Utils/GeometryUtils.h"
#include <vector>

namespace BaldLion
{
	using namespace GeometryUtils;

	namespace Rendering
	{
		class Mesh {

		public:
			Mesh(Material* material, AABB aabb, const DynamicArray<Vertex>& vertices, const DynamicArray<ui32>& indices, const glm::mat4& worldTransform);
			~Mesh();

			void SetUpMesh();
			virtual void Draw() const;

			inline const Material* GetMaterial() const { return m_material; }
			inline Material* GetMaterial() { return m_material; }
			inline const AABB& GetAABB() const { return m_aabb; }

			inline const DynamicArray<Vertex>& GetVertices() const { return m_vertices; }
			inline const DynamicArray<ui32>& GetIndices() const { return m_indices; }

		protected:

			AABB RecalculateAABB() const;

		protected:

			DynamicArray<Vertex> m_vertices;
			DynamicArray<ui32> m_indices;
			VertexArray* m_vertexArray;
			Material* m_material;
			glm::mat4 m_worldTransform;
			AABB m_aabb;
		};
	}
}