#pragma once
#include "Shader.h"
#include "Texture.h"
#include "BaldLion/Core/Core.h"
#include <glm/glm.hpp>

#define MATKEY_AMBIENT_COLOR	"u_material.ambientColor"
#define MATKEY_DIFFUSE_COLOR	"u_material.diffuseColor" 
#define MATKEY_EMISSIVE_COLOR	"u_material.emissiveColor"
#define MATKEY_SPECULAR_COLOR	"u_material.specularColor"
#define MATKEY_SHININESS		"u_material.shininess"

#define MATKEY_AMBIENT_TEX		"u_material.ambientTex" 
#define MATKEY_DIFFUSE_TEX		"u_material.diffuseTex"
#define MATKEY_EMISSIVE_TEX		"u_material.emissiveTex"
#define MATKEY_SPECULAR_TEX		"u_material.specularTex"
#define MATKEY_NORMAL_TEX		"u_material.normalTex"

namespace BaldLion
{
	namespace Rendering
	{
		class Material {

		public:
			static Ref<Material> Create(const std::string& shaderPath, 
				const glm::vec3& ambientColor, 
				const glm::vec3& diffuseColor, 
				const glm::vec3& emissiveColor, 
				const glm::vec3& specularColor, 
				float shininess,
				const std::string& ambientTexPath, 
				const std::string& diffuseTexPath, 
				const std::string& emissiveTexPath, 
				const std::string& specularTexPath, 
				const std::string& normalTexPath);

			static Ref<Material> Create(const std::string& shaderPath,
				const glm::vec3& ambientColor,
				const glm::vec3& diffuseColor,
				const glm::vec3& emissiveColor,
				const glm::vec3& specularColor,
				float shininess,
				const std::string& ambientTexPath, const unsigned char* ambientTexData, int ambientTexDataSize,
				const std::string& diffuseTexPath, const unsigned char* diffuseTexData, int diffuseTexDataSize,
				const std::string& emissiveTexPath, const unsigned char* emissiveTexData, int emissiveTexDataSize,
				const std::string& specularTexPath, const unsigned char* specularTexData, int specularTexDataSize,
				const std::string& normalTexPath, const unsigned char* normalTexData,int normalTexDataSize);

			virtual void SetAmbientColor(const glm::vec3& ambient) = 0;
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