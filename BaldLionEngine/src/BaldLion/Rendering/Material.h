#pragma once
#include "Shader.h"
#include "Texture.h"
#include "BaldLion/Core/Core.h"
#include <glm/glm.hpp>

#define MATKEY_AMBIENT_COLOR	STRING_TO_ID("u_material.ambientColor")
#define MATKEY_DIFFUSE_COLOR	STRING_TO_ID("u_material.diffuseColor" )
#define MATKEY_EMISSIVE_COLOR	STRING_TO_ID("u_material.emissiveColor")
#define MATKEY_SPECULAR_COLOR	STRING_TO_ID("u_material.specularColor")
#define MATKEY_SHININESS		STRING_TO_ID("u_material.shininess")

#define MATKEY_AMBIENT_TEX		STRING_TO_ID("u_material.ambientTex" )
#define MATKEY_DIFFUSE_TEX		STRING_TO_ID("u_material.diffuseTex")
#define MATKEY_EMISSIVE_TEX		STRING_TO_ID("u_material.emissiveTex")
#define MATKEY_SPECULAR_TEX		STRING_TO_ID("u_material.specularTex")
#define MATKEY_NORMAL_TEX		STRING_TO_ID("u_material.normalTex")

namespace BaldLion
{
	namespace Rendering
	{
		class Material {

		public:
			static Material* Create(
				const glm::vec3& ambientColor, 
				const glm::vec3& diffuseColor, 
				const glm::vec3& emissiveColor, 
				const glm::vec3& specularColor, 
				float shininess,
				Texture* ambientTex, 
				Texture* diffuseTex,
				Texture* emissiveTex,
				Texture* specularTex,
				Texture* normalTex);

			static void Destroy(Material* material);

			virtual void AssignShader(const std::string& shaderPath) = 0;

			virtual void SetAmbientColor(const glm::vec3& ambient) = 0;
			virtual void SetEmissiveColor(const glm::vec3& emissive) = 0;
			virtual void SetDiffuseColor(const glm::vec3& diffuse) = 0;
			virtual void SetSpecularColor(const glm::vec3& specular) = 0;
			virtual void SetShininess(float value) = 0;

			virtual void SetUniform(StringId uniformName, ShaderDataType dataType, const void* uniformIndex) = 0;

			virtual Shader* GetShader() const = 0;

			virtual ~Material() = default;

			virtual void Bind() const = 0;
			
		protected:

			StringId m_shaderPath;
		};

		
	}
}