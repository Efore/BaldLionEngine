#include "blpch.h"
#include <string_view>
#include "SkyboxPlatformInterface.h"

namespace BaldLion
{
	namespace Rendering
	{
		SkyboxPlatformInterface::SkyboxPlatform SkyboxPlatformInterface::s_skyboxPlatform = SkyboxPlatformInterface::SkyboxPlatform::OpenGL;

		void SkyboxPlatformInterface::Init(const std::string& path)
		{
			BL_PROFILE_FUNCTION();

			m_vertexArray = VertexArray::Create();

			float cube_vertices[] = {
			  -1.0,  1.0,  1.0,
			  -1.0, -1.0,  1.0,
			   1.0, -1.0,  1.0,
			   1.0,  1.0,  1.0,
			  -1.0,  1.0, -1.0,
			  -1.0, -1.0, -1.0,
			   1.0, -1.0, -1.0,
			   1.0,  1.0, -1.0
			};

			uint32_t cube_indices[] = {
			  0, 1, 2, 3,
			  3, 2, 6, 7,
			  7, 6, 5, 4,
			  4, 5, 1, 0,
			  0, 3, 7, 4,
			  5, 6, 2, 1
			};

			Ref<VertexBuffer> vertexBuffer;
			vertexBuffer = VertexBuffer::Create(&cube_vertices[0], (uint32_t)(sizeof(cube_vertices)));

			vertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "vertex_position"}				
			});

			Ref<IndexBuffer> indexBuffer;
			indexBuffer = (IndexBuffer::Create(&cube_indices[0], (uint32_t)(sizeof(cube_indices))));

			m_vertexArray->AddIndexBuffer(indexBuffer);
			m_vertexArray->AddVertexBuffer(vertexBuffer);
		}
	}
}