#pragma once
#include "Vertex.h"
#include "VertexArray.h"
#include "Material.h"
#include <vector>

namespace BaldLion
{
	class Mesh {
		
	public:
		Mesh();
		Mesh(const std::string& filePath);
		~Mesh();

		void SetUpMesh();
		void Draw();

		Ref<Material> GetMaterial() { return m_material; }

	private:
		void ExtractFromObj(const char* filePath);

		std::vector<Vertex> m_vertices;		
		std::vector<uint32_t> m_indices;

		Ref<VertexArray> m_vertexArray;
		Ref<Material> m_material;
	};
}