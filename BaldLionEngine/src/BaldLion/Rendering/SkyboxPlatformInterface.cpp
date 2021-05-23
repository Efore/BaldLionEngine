#include "blpch.h"
#include <string_view>
#include "SkyboxPlatformInterface.h"

#include "Platform/OpenGL/OpenGLSkybox.h"
#include "RendererPlatformInterface.h"

namespace BaldLion
{
	namespace Rendering
	{
		SkyboxPlatformInterface* SkyboxPlatformInterface::Create()
		{
			switch (RendererPlatformInterface::GetAPI())
			{
			case RendererPlatformInterface::RendererPlatform::None:			BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererPlatformInterface::RendererPlatform::OpenGL:		return MemoryManager::New<OpenGLSkybox>("OpenGLRenderer", AllocationType::FreeList_Renderer);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}

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

			ui32 cube_indices[] = {
			  0, 1, 2, 3,
			  3, 2, 6, 7,
			  7, 6, 5, 4,
			  4, 5, 1, 0,
			  0, 3, 7, 4,
			  5, 6, 2, 1
			};

			VertexBuffer* vertexBuffer = VertexBuffer::Create(&cube_vertices[0], (ui32)(sizeof(cube_vertices)));

			vertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "vertex_position"}				
			});

			IndexBuffer* indexBuffer = (IndexBuffer::Create(&cube_indices[0], (ui32)(sizeof(cube_indices))));

			m_vertexArray->AddIndexBuffer(indexBuffer);
			m_vertexArray->AddVertexBuffer(vertexBuffer);
		}

		void SkyboxPlatformInterface::Stop()
		{
			VertexArray::Destroy(m_vertexArray);
		}

	}
}