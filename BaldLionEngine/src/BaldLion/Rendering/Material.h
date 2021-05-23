#pragma once
#include "Shader.h"
#include "Texture.h"
#include "BaldLion/Core/Core.h"
#include <glm/glm.hpp>

namespace BaldLion
{
	namespace Rendering
	{

		class Material {
		public:
			enum class BlendMode {
				None,
				Alpha,
				Aditive
			};

			enum class DepthBufferMode {
				NoDepth,
				TestOnly,
				TestAndWrite
			};

			enum class CullingMode {
				Back,
				Front,
				None
			};

			enum class TextureSlots
			{
				AmbientTexture = 0,
				DiffuseTexture = 1,
				EmissiveTexture = 2,
				SpecularTexture = 3,
				NormalTexture = 4,
				ShadowMap = 5
			};
			
			enum class ShadowsSettingsBitMask
			{
				CastShadows = 1 << 0,
				ReceiveShadows = 1 << 1
			};

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

				BlendMode blendMode;
				DepthBufferMode depthBufferMode;
				CullingMode cullingMode;
				ui8 shadowSettings;

				MaterialProperties() {}

				MaterialProperties(StringId sPath,
					const glm::vec3& aColor,
					const glm::vec3& dColor,
					const glm::vec3& eColor,
					const glm::vec3& sColor,
					float shn,
					Texture* aTex,
					Texture* dTex,
					Texture* eTex,
					Texture* sTex,
					Texture* nTex,
					BlendMode bMode,
					DepthBufferMode dbMode,
					CullingMode cMode,
					ui8 shadowSettingsMask) :
					shaderPath(sPath),
					ambientColor(aColor),
					diffuseColor(dColor),
					emissiveColor(eColor),
					specularColor(sColor),
					shininess(shn),
					ambientTex(aTex),
					diffuseTex(dTex),
					emissiveTex(eTex),
					specularTex(sTex),
					normalTex(nTex),
					blendMode(bMode),
					depthBufferMode(dbMode),
					cullingMode(cMode),
					shadowSettings(shadowSettingsMask)
				{}

				MaterialProperties(const MaterialProperties& other) : 
					shaderPath(other.shaderPath),
					ambientColor(other.ambientColor),
					diffuseColor(other.diffuseColor),
					emissiveColor(other.emissiveColor),
					specularColor(other.specularColor),
					shininess(other.shininess),
					ambientTex(other.ambientTex),
					diffuseTex(other.diffuseTex),
					emissiveTex(other.emissiveTex),
					specularTex(other.specularTex),
					normalTex(other.normalTex),
					blendMode(other.blendMode),
					depthBufferMode(other.depthBufferMode),
					cullingMode(other.cullingMode),
					shadowSettings(other.shadowSettings)
				{}
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
			
			StringId GetShaderPath() { return m_materialProperties.shaderPath; }
			StringId GetMaterialName() { return m_materialName; }

			bool GetCastShadows() const { return (m_materialProperties.shadowSettings & (ui8)Material::ShadowsSettingsBitMask::CastShadows) > 0; }
			bool GetReceiveShadows() const { return (m_materialProperties.shadowSettings & (ui8)Material::ShadowsSettingsBitMask::ReceiveShadows) > 0; }

			virtual ~Material() = default;

			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;


			
		protected:

			Material(){}
			Material::Material(const MaterialProperties& materialProperties) : m_materialProperties(materialProperties){}

			StringId m_materialName;
			MaterialProperties m_materialProperties;
		};

		class MaterialLibrary
		{
		public:
			
			static void Init();
			static void Add(Material* shader);
			static Material* Load(const std::string& name, Material::MaterialProperties* materialProperties);
			static void Clear();

		private:
			static HashTable<StringId, Material*> s_materials;
			static std::mutex s_materialLibraryMutex;
		};
		
	}
}