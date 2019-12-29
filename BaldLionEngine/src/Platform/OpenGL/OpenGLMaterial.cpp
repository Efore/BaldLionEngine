#include "blpch.h"
#include "OpenGLMaterial.h"
#include "BaldLion/Renderer/Renderer.h"

namespace BaldLion
{
	OpenGLMaterial::OpenGLMaterial(const std::string & shaderPath, const glm::vec3 & ambient, const glm::vec3 & diffuse, const glm::vec3 & specular, const std::string & diffuseTexPath, const std::string & specularTexPath)
	{
		m_shader = std::dynamic_pointer_cast<OpenGLShader>(Renderer::GetShaderLibrary().Load(shaderPath));		

		m_ambient = ambient;
		m_diffuse = diffuse;
		m_specular = specular;

		m_diffuseTexSlot = 0;
		m_diffuseTex = std::dynamic_pointer_cast<OpenGLTexture2D>(Renderer::GetTextureLibrary().Load(diffuseTexPath));
		m_diffuseTex->Bind(m_diffuseTexSlot);

		m_specularTexSlot = 1;
		m_specularTex = std::dynamic_pointer_cast<OpenGLTexture2D>(Renderer::GetTextureLibrary().Load(specularTexPath));
		m_specularTex->Bind(m_specularTexSlot);

		m_shader->SetUniform("material.ambient", BaldLion::ShaderDataType::Float3, &m_ambient);
		m_shader->SetUniform("material.diffuse", BaldLion::ShaderDataType::Float3, &m_diffuse);
		m_shader->SetUniform("material.specular", BaldLion::ShaderDataType::Float3, &m_specular);

		m_shader->SetUniform("material.diffuseTex", BaldLion::ShaderDataType::Int, &m_diffuseTexSlot);
		m_shader->SetUniform("material.specularTex", BaldLion::ShaderDataType::Int, &m_specularTexSlot);

		m_shader->Bind();
	}

	void OpenGLMaterial::SetAmbientColor(const glm::vec3 & ambient)
	{
		m_ambient = ambient;
		m_shader->SetUniform("material.ambient", BaldLion::ShaderDataType::Float3, &m_ambient);
	}

	void OpenGLMaterial::SetDiffuseColor(const glm::vec3 & diffuse)
	{
		m_diffuse = diffuse;
		m_shader->SetUniform("material.diffuse", BaldLion::ShaderDataType::Float3, &m_diffuse);
	}

	void OpenGLMaterial::SetSpecularColor(const glm::vec3 & specular)
	{
		m_specular = specular;
		m_shader->SetUniform("material.specular", BaldLion::ShaderDataType::Float3, &m_specular);
	}

	void OpenGLMaterial::SetUniform(const std::string& uniformName, ShaderDataType dataType, const void* uniformIndex)
	{
		m_shader->SetUniform(uniformName, dataType, uniformIndex);
	}

	void OpenGLMaterial::Bind()
	{		
		m_diffuseTex->Bind(m_diffuseTexSlot);
		m_specularTex->Bind(m_specularTexSlot);
		m_shader->Bind();		
	}
}