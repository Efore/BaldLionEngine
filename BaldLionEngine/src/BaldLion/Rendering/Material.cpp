#include "blpch.h"
#include "Material.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLMaterial.h"

namespace BaldLion
{
	namespace Rendering
	{
		Material* Material::Create(const std::string& shaderPath, 
			const glm::vec3& ambientColor, 
			const glm::vec3& diffuseColor, 
			const glm::vec3& emissiveColor, 
			const glm::vec3& specularColor, 
			float shininess,
			Texture* ambientTex,
			Texture* diffuseTex,
			Texture* emissiveTex,
			Texture* specularTex,
			Texture* normalTex)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
			case RendererPlatformInterface::RendererPlatform::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererPlatformInterface::RendererPlatform::OpenGL:		
				
				return MemoryManager::New<OpenGLMaterial>(STRING_TO_ID("Material"),AllocationType::FreeList_Renderer,
					shaderPath,
					ambientColor, 
					diffuseColor, 
					emissiveColor, 
					specularColor, 
					shininess, 
					ambientTex,
					diffuseTex,
					emissiveTex,
					specularTex,
					normalTex);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}		

		void Material::Destroy(Material* material)
		{
			MemoryManager::Delete(material);
		}

	}
}
