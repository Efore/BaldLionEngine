#pragma once
#include "BaldLion/Rendering/Material.h"
#include "OpenGLShader.h"
#include "OpenGLTexture2D.h"

namespace BaldLion
{
	namespace Rendering
	{
		class OpenGLMaterial : public Material
		{

		public:

			OpenGLMaterial(const std::string& shaderPath,
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

			virtual void SetAmbientColor(const glm::vec3 & emissive) override;
			virtual void SetEmissiveColor(const glm::vec3 & emissive) override;
			virtual void SetDiffuseColor(const glm::vec3 & diffuse) override;
			virtual void SetSpecularColor(const glm::vec3 & specular) override;
			virtual void SetShininess(float value) override;

			virtual void SetUniform(StringId uniformName, ShaderDataType dataType, const void* uniformIndex) override;

			virtual Shader* GetShader() const override { return m_shader; }

			virtual void Bind() const override;

		private:
			OpenGLShader* m_shader;

			glm::vec3 m_ambientColor;
			glm::vec3 m_emissiveColor;
			glm::vec3 m_diffuseColor;
			glm::vec3 m_specularColor;
			float m_shininess;

			OpenGLTexture2D* m_ambientTex = nullptr;
			int m_ambientTexSlot;

			OpenGLTexture2D* m_diffuseTex = nullptr;
			int m_diffuseTexSlot;

			OpenGLTexture2D* m_emissiveTex = nullptr;
			int m_emissiveTexSlot;

			OpenGLTexture2D* m_specularTex = nullptr;
			int m_specularTexSlot;

			OpenGLTexture2D* m_normalTex = nullptr;
			int m_normalTexSlot;
		};
	}
}