#include "blpch.h"
#include "OpenGLMaterial.h"
#include "BaldLion/Rendering/Renderer.h"

namespace BaldLion
{
	namespace Rendering
	{
		OpenGLMaterial::OpenGLMaterial(const std::string& shaderPath, const glm::vec3& emissive, const glm::vec3& diffuse, const glm::vec3& specular, float shininess, const std::string& diffuseTexPath, const std::string& emissiveTexPath, const std::string& specularTexPath, const std::string& normalTexPath)
		{
			m_shader = std::dynamic_pointer_cast<OpenGLShader>(Renderer::GetShaderLibrary().Load(shaderPath));

			m_emissiveColor = emissive;
			m_diffuseColor = diffuse;
			m_specularColor = specular;
			m_shininess = shininess;

			m_diffuseTexSlot = 0;
			m_diffuseTex = std::dynamic_pointer_cast<OpenGLTexture2D>(Renderer::GetTextureLibrary().Load(diffuseTexPath));
			m_diffuseTex->Bind(m_diffuseTexSlot);

			m_emissiveTexSlot = 1;
			m_emissiveTex = std::dynamic_pointer_cast<OpenGLTexture2D>(Renderer::GetTextureLibrary().Load(emissiveTexPath));
			m_emissiveTex->Bind(m_emissiveTexSlot);

			m_specularTexSlot = 2;
			m_specularTex = std::dynamic_pointer_cast<OpenGLTexture2D>(Renderer::GetTextureLibrary().Load(specularTexPath));
			m_specularTex->Bind(m_specularTexSlot);

			m_normalTexSlot = 3;
			m_normalTex = std::dynamic_pointer_cast<OpenGLTexture2D>(Renderer::GetTextureLibrary().Load(normalTexPath));
			m_normalTex->Bind(m_normalTexSlot);

			m_shader->Bind();

			m_shader->SetUniform("u_material.diffuseColor", ShaderDataType::Float3, &m_diffuseColor);
			m_shader->SetUniform("u_material.emissiveColor", ShaderDataType::Float3, &m_emissiveColor);
			m_shader->SetUniform("u_material.specularColor", ShaderDataType::Float3, &m_specularColor);
			m_shader->SetUniform("u_material.shininess", ShaderDataType::Float, &m_shininess);

			m_shader->SetUniform("u_material.diffuseTex", ShaderDataType::Int, &m_diffuseTexSlot);
			m_shader->SetUniform("u_material.emissiveTex", ShaderDataType::Int, &m_emissiveTexSlot);
			m_shader->SetUniform("u_material.specularTex", ShaderDataType::Int, &m_specularTexSlot);
			m_shader->SetUniform("u_material.normalTex", ShaderDataType::Int, &m_normalTexSlot);
		}

		void OpenGLMaterial::SetEmissiveColor(const glm::vec3 & emissive)
		{
			m_emissiveColor = emissive;
			m_shader->SetUniform("u_material.emissiveColor", ShaderDataType::Float3, &m_emissiveColor);
		}

		void OpenGLMaterial::SetDiffuseColor(const glm::vec3 & diffuse)
		{
			m_diffuseColor = diffuse;
			m_shader->SetUniform("u_material.diffuseColor", ShaderDataType::Float3, &m_diffuseColor);
		}

		void OpenGLMaterial::SetSpecularColor(const glm::vec3 & specular)
		{
			m_specularColor = specular;
			m_shader->SetUniform("u_material.specularColor", ShaderDataType::Float3, &m_specularColor);
		}

		void OpenGLMaterial::SetShininess(float value)
		{
			m_shininess = value;
			m_shader->SetUniform("u_material.shininess", ShaderDataType::Float, &m_shininess);
		}

		void OpenGLMaterial::SetUniform(const std::string& uniformName, ShaderDataType dataType, const void* uniformIndex)
		{
			m_shader->SetUniform(uniformName, dataType, uniformIndex);
		}

		void OpenGLMaterial::Bind() const
		{
			m_diffuseTex->Bind(m_diffuseTexSlot);
			m_emissiveTex->Bind(m_emissiveTexSlot);
			m_specularTex->Bind(m_specularTexSlot);
			m_normalTex->Bind(m_normalTexSlot);
			m_shader->Bind();
		}
	}
}