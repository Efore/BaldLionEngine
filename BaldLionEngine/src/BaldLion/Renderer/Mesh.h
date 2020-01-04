#pragma once
#include "Vertex.h"
#include "VertexArray.h"
#include "Material.h"
#include <vector>

namespace BaldLion
{
	class Mesh {
		
	public:
		Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, const Ref<Material>& material);
		~Mesh();

		void SetUpMesh();
		void Draw() const;

		const Ref<Material>& GetMaterial() const { return m_material; }

	private:
		std::vector<Vertex> m_vertices;		
		std::vector<uint32_t> m_indices;

		Ref<VertexArray> m_vertexArray;
		Ref<Material> m_material;
	};
}