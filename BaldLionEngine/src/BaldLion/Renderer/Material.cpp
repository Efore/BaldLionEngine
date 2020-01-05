#include "blpch.h"
#include "Material.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLMaterial.h"

namespace BaldLion
{
	Ref<Material> Material::Create(const std::string& shaderPath, const glm::vec3& emissive, const glm::vec3& diffuse, const glm::vec3& specular, float shininess, const std::string& diffuseTexPath, const std::string& emissiveTexPath, const std::string& specularTexPath, const std::string& normalTexPath)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLMaterial>(shaderPath, emissive, diffuse, specular, shininess, diffuseTexPath, emissiveTexPath, specularTexPath, normalTexPath);
		}

		BL_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}
