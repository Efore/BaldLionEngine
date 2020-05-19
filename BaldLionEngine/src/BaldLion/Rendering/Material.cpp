#include "blpch.h"
#include "Material.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLMaterial.h"

namespace BaldLion
{
	namespace Rendering
	{
		Ref<Material> Material::Create(const std::string& shaderPath, 
			const glm::vec3& ambientColor, 
			const glm::vec3& diffuseColor, 
			const glm::vec3& emissiveColor, 
			const glm::vec3& specularColor, 
			float shininess,
			const Ref<Texture> ambientTex,
			const Ref<Texture> diffuseTex,
			const Ref<Texture> emissiveTex,
			const Ref<Texture> specularTex,
			const Ref<Texture> normalTex)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
			case RendererPlatformInterface::RendererPlatform::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererPlatformInterface::RendererPlatform::OpenGL:		
				
				return CreateRef<OpenGLMaterial>(shaderPath, 
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
	}
}
