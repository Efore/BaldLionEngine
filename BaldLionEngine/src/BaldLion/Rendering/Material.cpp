#include "blpch.h"
#include "Material.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLMaterial.h"

namespace BaldLion
{
	namespace Rendering
	{
		Material* Material::Create(const std::string& matName, const MaterialProperties& materialProperties)
		{
			Material* result = nullptr;
			switch (RendererPlatformInterface::GetAPI())
			{
				case RendererPlatformInterface::RendererPlatform::None:		
					BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); 
					return nullptr;
				case RendererPlatformInterface::RendererPlatform::OpenGL:	
					result = MemoryManager::New<OpenGLMaterial>(std::string("Material " + matName).c_str(),AllocationType::FreeList_Renderer, matName, materialProperties);
					Renderer::RegisterMaterial(result);
					return result;
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}		
	}
}
