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
				const std::string& ambientTexPath, 
				const std::string& diffuseTexPath, 
				const std::string& emissiveTexPath, 
				const std::string& specularTexPath, 
				const std::string& normalTexPath);

			virtual void SetAmbientColor(const glm::vec3 & emissive) override;
			virtual void SetEmissiveColor(const glm::vec3 & emissive) override;
			virtual void SetDiffuseColor(const glm::vec3 & diffuse) override;
			virtual void SetSpecularColor(const glm::vec3 & specular) override;
			virtual void SetShininess(float value) override;

			virtual void SetUniform(const std::string& uniformName, ShaderDataType dataType, const void* uniformIndex) override;

			virtual Ref<Shader> GetShader() const override { return m_shader; }

			virtual void Bind() const override;

		private:
			Ref<OpenGLShader> m_shader;

			glm::vec3 m_ambientColor;
			glm::vec3 m_emissiveColor;
			glm::vec3 m_diffuseColor;
			glm::vec3 m_specularColor;
			float m_shininess;

			Ref<OpenGLTexture2D> m_ambientTex;
			int m_ambientTexSlot;

			Ref<OpenGLTexture2D> m_diffuseTex;
			int m_diffuseTexSlot;

			Ref<OpenGLTexture2D> m_emissiveTex;
			int m_emissiveTexSlot;

			Ref<OpenGLTexture2D> m_specularTex;
			int m_specularTexSlot;

			Ref<OpenGLTexture2D> m_normalTex;
			int m_normalTexSlot;
		};
	}
}