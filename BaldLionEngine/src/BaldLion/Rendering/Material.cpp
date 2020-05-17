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
			const std::string& ambientTexPath, 
			const std::string& diffuseTexPath, 
			const std::string& emissiveTexPath, 
			const std::string& specularTexPath, 
			const std::string& normalTexPath)
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
					ambientTexPath, 
					diffuseTexPath, 
					emissiveTexPath, 
					specularTexPath, 
					normalTexPath);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}

		Ref<Material> Material::Create(const std::string& shaderPath,
			const glm::vec3& ambientColor,
			const glm::vec3& diffuseColor,
			const glm::vec3& emissiveColor,
			const glm::vec3& specularColor,
			float shininess,
			const std::string& ambientTexPath, const unsigned char* ambientTexData, int ambientTexDataSize,
			const std::string& diffuseTexPath, const unsigned char* diffuseTexData, int diffuseTexDataSize,
			const std::string& emissiveTexPath, const unsigned char* emissiveTexData, int emissiveTexDataSize,
			const std::string& specularTexPath, const unsigned char* specularTexData, int specularTexDataSize,
			const std::string& normalTexPath, const unsigned char* normalTexData, int normalTexDataSize)
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
					ambientTexPath, ambientTexData, ambientTexDataSize,
					diffuseTexPath, diffuseTexData, diffuseTexDataSize,
					emissiveTexPath, emissiveTexData, emissiveTexDataSize,
					specularTexPath, specularTexData, specularTexDataSize,
					normalTexPath, normalTexData, normalTexDataSize);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}
	}
}
