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

#define MATKEY_USE_AMBIENT_TEX		STRING_TO_ID("u_material.useAmbientTex" )
#define MATKEY_AMBIENT_TEX			STRING_TO_ID("u_material.ambientTex" )
#define MATKEY_USE_DIFFUSE_TEX		STRING_TO_ID("u_material.useDiffuseTex" )
#define MATKEY_DIFFUSE_TEX			STRING_TO_ID("u_material.diffuseTex")
#define MATKEY_USE_EMISSIVE_TEX		STRING_TO_ID("u_material.useEmissiveTex" )
#define MATKEY_EMISSIVE_TEX			STRING_TO_ID("u_material.emissiveTex")
#define MATKEY_USE_SPECULAR_TEX		STRING_TO_ID("u_material.useSpecularTex" )
#define MATKEY_SPECULAR_TEX			STRING_TO_ID("u_material.specularTex")
#define MATKEY_USE_NORMAL_TEX		STRING_TO_ID("u_material.useNormalTex" )
#define MATKEY_NORMAL_TEX			STRING_TO_ID("u_material.normalTex")

namespace BaldLion
{
	namespace Rendering
	{

		class Material {

		public:

			struct MaterialProperties {

				StringId shaderPath;
				glm::vec3 ambientColor;
				glm::vec3 diffuseColor;
				glm::vec3 emissiveColor;
				glm::vec3 specularColor;
				float shininess;
				Texture* ambientTex;
				Texture* diffuseTex;
				Texture* emissiveTex;
				Texture* specularTex;
				Texture* normalTex;
			};

		public:
			static Material* Create(const std::string& shaderPath, const MaterialProperties& materialProperties);

			virtual void AssignShader() = 0;

			virtual void SetAmbientColor(const glm::vec3& ambient) = 0;
			virtual void SetEmissiveColor(const glm::vec3& emissive) = 0;
			virtual void SetDiffuseColor(const glm::vec3& diffuse) = 0;
			virtual void SetSpecularColor(const glm::vec3& specular) = 0;
			virtual void SetShininess(float value) = 0;

			virtual void SetUniform(StringId uniformName, ShaderDataType dataType, const void* uniformIndex) = 0;

			virtual Shader* GetShader() const = 0;
			
			StringId GetShaderPath() { return m_shaderPath; }
			StringId GetMaterialName() { return m_materialName; }

			virtual ~Material() = default;

			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;
			
		protected:

			StringId m_shaderPath;
			StringId m_materialName;
		};

		class MaterialLibrary
		{
		public:
			
			static void Init();
			static void Add(Material* shader);
			static Material* Load(const std::string& name, const Material::MaterialProperties& materialProperties);
			static void Clear();

		private:
			static HashTable<StringId, Material*> s_materials;
			static std::mutex s_materialLibraryMutex;
		};
		
	}
}