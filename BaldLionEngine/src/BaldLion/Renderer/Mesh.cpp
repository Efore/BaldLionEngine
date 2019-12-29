#include "blpch.h"
#include "Mesh.h"
#include "Buffer.h"
#include "Renderer.h"

#include <string_view>

namespace BaldLion
{
	Mesh::Mesh()
	{

	}

	Mesh::Mesh(const std::string& filePath)
	{
		std::string extension = filePath.substr(filePath.find('.'));		

		if (std::strcmp(extension.c_str(), OBJ_EXTENSION) == 0)
			ExtractFromObj(filePath.c_str());
	}

	Mesh::~Mesh()
	{

	}

	void Mesh::SetUpMesh()
	{
		Ref<VertexBuffer> vertexBuffer;
		vertexBuffer = VertexBuffer::Create(&m_vertices[0], (uint32_t)(m_vertices.size() * sizeof(BaldLion::Vertex)));

		vertexBuffer->SetLayout({
			{ BaldLion::ShaderDataType::Float3, "vertex_position"},
			{ BaldLion::ShaderDataType::Float3, "vertex_color"},
			{ BaldLion::ShaderDataType::Float3, "vertex_normal"},
			{ BaldLion::ShaderDataType::Float2, "vertex_texcoord"}
		});

		m_vertexArray = VertexArray::Create();
		m_vertexArray->AddVertexBuffer(vertexBuffer);

		Ref<IndexBuffer> indexBuffer;
		indexBuffer = (IndexBuffer::Create(&m_indices[0], (uint32_t)m_indices.size()));
		m_vertexArray->AddIndexBuffer(indexBuffer);

		m_material = Material::Create("assets/shaders/Diffuse.glsl", glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), "assets/textures/diffuseTex.png", "assets/textures/specularTex.png");
		m_material->Bind();
	}

	void Mesh::Draw()
	{
		m_material->Bind();

		Renderer::Submit(m_vertexArray, m_material->GetShader());
	}

	void Mesh::ExtractFromObj(const char* objPath)
	{
		std::vector<glm::vec3> vertexPositions;
		vertexPositions.reserve(10000);
		
		std::vector<glm::vec3> vertexNormals;
		vertexNormals.reserve(10000);

		std::vector<glm::vec2> vertexTexCoords;
		vertexTexCoords.reserve(10000);

		std::vector< unsigned int > uvIndices, normalIndices;

		FILE * file = fopen(objPath, "r");

		if (file == NULL) {
			printf("Impossible to open the file !\n");
			return;
		}

		while (1) {

			char lineHeader[128];
			// read the first word of the line
			int res = fscanf(file, "%s", lineHeader);

			if (res == EOF)
				break;

			if (strcmp(lineHeader, "v") == 0) {
				glm::vec3 vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				vertexPositions.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) {
				glm::vec2 uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y);
				vertexTexCoords.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0) {
				glm::vec3 normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				vertexNormals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) {

				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					printf("File can't be read by our simple parser : ( Try exporting with other options\n");
					return;
				}

				m_indices.push_back(vertexIndex[0] - 1);
				m_indices.push_back(vertexIndex[1] - 1);
				m_indices.push_back(vertexIndex[2] - 1);

				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);

				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
		}

		m_vertices.reserve(vertexNormals.size());

		for (size_t i = 0; i < vertexPositions.size(); ++i)
		{
			m_vertices.emplace_back(Vertex{ vertexPositions[i], glm::vec3(1.0f), vertexNormals[i], vertexTexCoords[i] });
		} 
	}

}
