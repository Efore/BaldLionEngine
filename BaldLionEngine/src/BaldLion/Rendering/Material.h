#pragma once
#include "Shader.h"
#include "Texture.h"
#include "BaldLion/Core/Core.h"
#include "BaldLion/ResourceManagement/Resource.h"
#include <glm/glm.hpp>

namespace BaldLion
{
	namespace Rendering
	{

		class Material : public ResourceManagement::Resource {
		public:
			enum class BlendMode : ui8 {
				None,
				Alpha,
				Aditive
			};

			enum class DepthBufferMode : ui8 {
				NoDepth,
				TestOnly,
				TestAndWrite
			};

			enum class CullingMode : ui8 {
				Back,
				Front,
				None
			};

			enum class TextureSlots : ui8
			{
				AmbientTexture = 0,
				DiffuseTexture = 1,
				EmissiveTexture = 2,
				SpecularTexture = 3,
				NormalTexture = 4,
				ShadowMap = 5
			};
			
			enum class ShadowsSettingsBitMask : ui8
			{
				CastShadows = 1 << 0,
				ReceiveShadows = 1 << 1
			};

			struct MaterialProperties {

				glm::vec3 ambientColor = glm::vec3(0.0f);
				glm::vec3 diffuseColor = glm::vec3(0.0f);
				glm::vec3 emissiveColor = glm::vec3(0.0f);
				glm::vec3 specularColor = glm::vec3(0.0f);
				StringId shaderPath = 0;
				float shininess = 0.0f;
				Texture * ambientTex = nullptr;
				Texture* diffuseTex = nullptr;
				Texture* emissiveTex = nullptr;
				Texture* specularTex = nullptr;
				Texture* normalTex = nullptr;

				BlendMode blendMode = BlendMode::None;
				DepthBufferMode depthBufferMode = DepthBufferMode::NoDepth;
				CullingMode cullingMode = CullingMode::None;
				ui8 shadowSettings = 0;

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

			void SetCastShadows(bool castShadows);
			void SetReceiveShadows(bool receiveShadows);

			virtual ~Material() = default;

			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;


			
		protected:

			Material(){}
			Material(const std::string& matPath, const MaterialProperties& materialProperties) : 
				ResourceManagement::Resource(BL_STRING_TO_STRINGID(matPath), matPath, ResourceManagement::ResourceType::Material),
				m_materialProperties(materialProperties){}

			StringId m_materialName;
			MaterialProperties m_materialProperties;
		};
	}
}