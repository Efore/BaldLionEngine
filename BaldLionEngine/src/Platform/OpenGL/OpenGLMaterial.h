#pragma once
#include "BaldLion/Renderer/Material.h"
#include "OpenGLShader.h"
#include "OpenGLTexture.h"

namespace BaldLion
{
	class OpenGLMaterial : public Material
	{
	private:
		Ref<OpenGLShader> m_shader;

		glm::vec3 m_ambient;
		glm::vec3 m_diffuse;
		glm::vec3 m_specular;

		Ref<OpenGLTexture2D> m_diffuseTex;
		int m_diffuseTexSlot;

		Ref<OpenGLTexture2D> m_specularTex;
		int m_specularTexSlot;

	public:

		OpenGLMaterial(const std::string& shaderPath, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const std::string& diffuseTexPath, const std::string& specularTexPath);

		virtual void SetAmbientColor(const glm::vec3 & ambient) override;
		virtual void SetDiffuseColor(const glm::vec3 & diffuse) override;
		virtual void SetSpecularColor(const glm::vec3 & specular) override;

		virtual void Bind() override;
	};
}