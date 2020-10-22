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
			Mesh(BLVector<Vertex>& vertices, BLVector<uint32_t>& indices, Material* material);
			~Mesh();

			void SetUpMesh(const BLVector<Vertex>& vertices, const BLVector<uint32_t>& indices);
			void Draw() const;

			const Material* GetMaterial() const { return m_material; }

		protected:

			VertexArray* m_vertexArray;
			Material* m_material;
		};
	}
}