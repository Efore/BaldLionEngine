#pragma once
#include "Vertex.h"
#include "VertexArray.h"
#include "Material.h"
#include <vector>

namespace BaldLion
{
	namespace Rendering
	{
		class Mesh {

		public:
			Mesh(Material* material);
			~Mesh();

			void SetUpMesh(const DynamicArray<Vertex>& vertices, const DynamicArray<ui32>& indices);
			virtual void Draw(const glm::mat4& worldTransform) const;

			const Material* GetMaterial() const { return m_material; }

		protected:

			VertexArray* m_vertexArray;
			Material* m_material;
		};
	}
}