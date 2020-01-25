#pragma once
#include "Shader.h"
#include "Texture.h"
#include "BaldLion/Core.h"
#include <glm/glm.hpp>

namespace BaldLion{
	namespace Rendering
	{
		class Material {

		public:
			static Ref<Material> Create(const std::string& shaderPath, const glm::vec3& emissive, const glm::vec3& diffuse, const glm::vec3& specular, float shininess, const std::string& diffuseTexPath, const std::string& emissiveTexPath, const std::string& specularTexPath, const std::string& normalTexPath);

			virtual void SetEmissiveColor(const glm::vec3& emissive) = 0;
			virtual void SetDiffuseColor(const glm::vec3& diffuse) = 0;
			virtual void SetSpecularColor(const glm::vec3& specular) = 0;
			virtual void SetShininess(float value) = 0;

			virtual void SetUniform(const std::string& uniformName, ShaderDataType dataType, const void* uniformIndex) = 0;

			virtual Ref<Shader> GetShader() const = 0;

			virtual ~Material() = default;

			virtual void Bind() const = 0;
		};
	}
}