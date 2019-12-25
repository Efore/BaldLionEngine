#pragma once
#include "Shader.h"
#include "Texture.h"
#include "BaldLion/Core.h"
#include <glm/glm.hpp>

namespace BaldLion{

	class Material {

	public:
		static Ref<Material> Create(const std::string& shaderPath, const glm::vec3& ambientColor, const glm::vec3& diffuseColor, const glm::vec3& specularColor, const std::string& diffuseTexPath, const std::string& specularTexPath);

		virtual void SetAmbientColor(const glm::vec3& ambient) = 0;
		virtual void SetDiffuseColor(const glm::vec3& diffuse) = 0;
		virtual void SetSpecularColor(const glm::vec3& specular) = 0;

		virtual ~Material() = default;

		virtual void Bind() = 0;
	};
}