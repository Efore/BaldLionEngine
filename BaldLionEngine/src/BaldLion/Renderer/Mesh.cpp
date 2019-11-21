#include "blpch.h"
#include "Mesh.h"
#include "Buffer.h"
#include "Renderer.h"

#include <fstream>
#include <string_view>

namespace BaldLion
{
	Mesh::Mesh()
	{

	}

	Mesh::Mesh(const std::string& objPath)
	{
		std::vector<glm::vec3> vertexPositions;
		vertexPositions.reserve(10000);

		std::vector<glm::vec3> vertexNormals;
		vertexNormals.reserve(10000);

		std::vector<glm::vec2> vertexTexCoords;
		vertexTexCoords.reserve(10000);

		std::ifstream fs(objPath);
		std::string str;

		while (std::getline(fs, str))
		{

			size_t currentIndex = 0;
			size_t nextSpace = str.find(' ');
			std::string lineData = str.substr(currentIndex, nextSpace);
						
			size_t values_index = 0;

			if (lineData == "v")
			{
				float values[3];
				do
				{
					currentIndex = nextSpace;
					nextSpace = str.find(' ',currentIndex + 1);
					lineData = str.substr(currentIndex, nextSpace);

					values[values_index] = stof(lineData);
					++values_index;
				}
				while (nextSpace != std::string::npos);

				vertexPositions.emplace_back(glm::vec3(values[0], values[1], values[2]));
			}
			else if (lineData == "vn")
			{
				float values[3];
				do
				{
					currentIndex = nextSpace;
					nextSpace = str.find(' ', currentIndex + 1);
					lineData = str.substr(currentIndex, nextSpace);

					values[values_index] = stof(lineData);
					++values_index;
				} 
				while (nextSpace != std::string::npos);

				vertexNormals.emplace_back(glm::vec3(values[0], values[1], values[2]));
			}
			else if (lineData == "vt")
			{
				float values[2];
				do
				{
					currentIndex = nextSpace;
					nextSpace = str.find(' ', currentIndex + 1);
					lineData = str.substr(currentIndex, nextSpace);

					values[values_index] = stof(lineData);
					++values_index;
				} 
				while (nextSpace != std::string::npos);

				vertexTexCoords.emplace_back(glm::vec2(values[0], values[1]));
			}
			else if (lineData == "f")
			{
				do
				{
					currentIndex = nextSpace;
					nextSpace = str.find(' ', currentIndex + 1);
					lineData = str.substr(currentIndex, nextSpace);

					std::string indexValue = lineData.substr(0, lineData.find('/'));					

					m_indices.emplace_back(stoi(indexValue));
					
				} while (nextSpace != std::string::npos);
			}
		}

		m_vertices.reserve(vertexNormals.size());
		for (size_t i = 0; i < vertexPositions.size(); ++i)
		{
			m_vertices.emplace_back( Vertex { vertexPositions[i],vertexNormals[i],vertexTexCoords[i] });
		}
	}


	Mesh::~Mesh()
	{

	}

	void Mesh::SetUpMesh()
	{
		Ref<VertexBuffer> vertexBuffer;
		vertexBuffer = VertexBuffer::Create(&m_vertices[0], (uint32_t)(m_vertices.size() * sizeof(BaldLion::Vertex)));

		vertexBuffer->SetLayout({
			{ BaldLion::ShaderDataType::Float3, "a_position"},
			{ BaldLion::ShaderDataType::Float3, "a_normal"},
			{ BaldLion::ShaderDataType::Float2, "a_texCoord"}
		});

		m_vertexArray = VertexArray::Create();
		m_vertexArray->AddVertexBuffer(vertexBuffer);

		Ref<IndexBuffer> indexBuffer;
		indexBuffer = (BaldLion::IndexBuffer::Create(&m_indices[0], m_indices.size()));
		m_vertexArray->AddIndexBuffer(indexBuffer);

		auto textureShader = BaldLion::Renderer::GetShaderLibrary().Load("assets/shaders/Cube.glsl");
		textureShader->Bind();
	}

	void Mesh::Draw()
	{
		auto textureShader = Renderer::GetShaderLibrary().Get("Cube");		
		textureShader->Bind();

		Renderer::Submit(m_vertexArray, textureShader);		
	}
}
