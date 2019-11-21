#pragma once
#include "Vertex.h"
#include "VertexArray.h"
#include <vector>

namespace BaldLion
{
	class Mesh {
		
	public:
		Mesh();
		Mesh(const std::string& path);
		~Mesh();

		void SetUpMesh();
		void Draw();

	private:

		std::vector<Vertex> m_vertices;		
		std::vector<uint32_t> m_indices;
		Ref<VertexArray> m_vertexArray;
	};
}