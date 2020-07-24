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
			Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const Ref<Material>& material);
			~Mesh();

			void SetUpMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
			void Draw() const;

			const Ref<Material>& GetMaterial() const { return m_material; }

		protected:

			Ref<VertexArray> m_vertexArray;
			Ref<Material> m_material;
		};
	}
}