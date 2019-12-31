#include "blpch.h"
#include "Material.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLMaterial.h"

namespace BaldLion
{
	Ref<Material> Material::Create(const std::string & shaderPath, const glm::vec3 & ambientColor, const glm::vec3 & diffuseColor, const glm::vec3 & specularColor, float shininess, const std::string & diffuseTexPath, const std::string & specularTexPath)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLMaterial>(shaderPath, ambientColor, diffuseColor, specularColor, shininess, diffuseTexPath, specularTexPath);
		}

		BL_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}
