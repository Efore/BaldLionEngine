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
			Mesh(Material* material, AABB aabb);
			~Mesh();

			void SetUpMesh(const DynamicArray<Vertex>& vertices, const DynamicArray<ui32>& indices);
			virtual void Draw(const glm::mat4& worldTransform) const;

			inline const Material* GetMaterial() const { return m_material; }
			inline const AABB& GetAABB() const { return m_aabb; }

		protected:

			VertexArray* m_vertexArray;
			Material* m_material;
			AABB m_aabb;
		};
	}
}