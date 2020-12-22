#include "blpch.h"
#include "OpenGLMaterial.h"
#include "BaldLion/Rendering/Renderer.h"

namespace BaldLion
{
	namespace Rendering
	{
		OpenGLMaterial::OpenGLMaterial(const std::string& shaderPath, 
			const glm::vec3& ambientColor, 
			const glm::vec3& diffuseColor, 
			const glm::vec3& emissiveColor, 
			const glm::vec3& specularColor, 
			float shininess,
			Texture* ambientTex,
			Texture* diffuseTex,
			Texture* emissiveTex,
			Texture* specularTex,
			Texture* normalTex) :
			m_ambientColor(ambientColor),
			m_diffuseColor(diffuseColor),
			m_emissiveColor(emissiveColor),
			m_specularColor(specularColor),
			m_shininess(shininess)
		{
			BL_PROFILE_FUNCTION();

			m_shader = (OpenGLShader*)(Renderer::GetShaderLibrary().Load(shaderPath));

			m_ambientColor = ambientColor;
			m_diffuseColor = diffuseColor;
			m_emissiveColor = emissiveColor;
			m_specularColor = specularColor;
			m_shininess = shininess;

			short slotIndex = 0;

			if (ambientTex != nullptr)
			{
				m_ambientTexSlot = slotIndex++;
				m_ambientTex = (OpenGLTexture2D*)ambientTex;
				m_ambientTex->Bind(m_ambientTexSlot);
			}

			if (diffuseTex != nullptr)
			{
				m_diffuseTexSlot = slotIndex++;
				m_diffuseTex = (OpenGLTexture2D*)(diffuseTex);
				m_diffuseTex->Bind(m_diffuseTexSlot);
			}

			if (emissiveTex != nullptr)
			{
				m_emissiveTexSlot = slotIndex++;
				m_emissiveTex = (OpenGLTexture2D*)(emissiveTex);
				m_emissiveTex->Bind(m_emissiveTexSlot);
			}

			if (specularTex != nullptr)
			{
				m_specularTexSlot = slotIndex++;
				m_specularTex = (OpenGLTexture2D*)(specularTex);
				m_specularTex->Bind(m_specularTexSlot);
			}

			if (normalTex)
			{
				m_normalTexSlot = slotIndex++;
				m_normalTex = (OpenGLTexture2D*)(normalTex);
				m_normalTex->Bind(m_normalTexSlot);
			}

			m_shader->Bind();

			m_shader->SetUniform(MATKEY_AMBIENT_COLOR, ShaderDataType::Float3, &m_ambientColor);
			m_shader->SetUniform(MATKEY_DIFFUSE_COLOR, ShaderDataType::Float3, &m_diffuseColor);
			m_shader->SetUniform(MATKEY_EMISSIVE_COLOR, ShaderDataType::Float3, &m_emissiveColor);
			m_shader->SetUniform(MATKEY_SPECULAR_COLOR, ShaderDataType::Float3, &m_specularColor);
			m_shader->SetUniform(MATKEY_SHININESS, ShaderDataType::Float, &m_shininess);

			if (m_ambientTex != nullptr)
				m_shader->SetUniform(MATKEY_AMBIENT_TEX, ShaderDataType::Int, &m_ambientTexSlot);

			if (m_diffuseTex != nullptr)
				m_shader->SetUniform(MATKEY_DIFFUSE_TEX, ShaderDataType::Int, &m_diffuseTexSlot);

			if (m_emissiveTex != nullptr)
				m_shader->SetUniform(MATKEY_EMISSIVE_TEX, ShaderDataType::Int, &m_emissiveTexSlot);

			if (m_specularTex != nullptr)
				m_shader->SetUniform(MATKEY_SPECULAR_TEX, ShaderDataType::Int, &m_specularTexSlot);

			if (m_normalTex != nullptr)
				m_shader->SetUniform(MATKEY_NORMAL_TEX, ShaderDataType::Int, &m_normalTexSlot);
		}

		

		void OpenGLMaterial::SetAmbientColor(const glm::vec3 & ambient)
		{
			m_emissiveColor = ambient;
			m_shader->SetUniform(MATKEY_AMBIENT_COLOR, ShaderDataType::Float3, &m_ambientColor);
		}

		void OpenGLMaterial::SetEmissiveColor(const glm::vec3 & emissive)
		{
			m_emissiveColor = emissive;
			m_shader->SetUniform(MATKEY_EMISSIVE_COLOR, ShaderDataType::Float3, &m_emissiveColor);
		}

		void OpenGLMaterial::SetDiffuseColor(const glm::vec3 & diffuse)
		{
			m_diffuseColor = diffuse;
			m_shader->SetUniform(MATKEY_DIFFUSE_COLOR, ShaderDataType::Float3, &m_diffuseColor);
		}

		void OpenGLMaterial::SetSpecularColor(const glm::vec3 & specular)
		{
			m_specularColor = specular;
			m_shader->SetUniform(MATKEY_SPECULAR_COLOR, ShaderDataType::Float3, &m_specularColor);
		}

		void OpenGLMaterial::SetShininess(float value)
		{
			m_shininess = value;
			m_shader->SetUniform(MATKEY_SHININESS, ShaderDataType::Float, &m_shininess);
		}

		void OpenGLMaterial::SetUniform(const char* uniformName, ShaderDataType dataType, const void* uniformIndex)
		{
			BL_PROFILE_FUNCTION();

			m_shader->SetUniform(uniformName, dataType, uniformIndex);
		}

		void OpenGLMaterial::Bind() const
		{
			BL_PROFILE_FUNCTION();

			if (m_ambientTex != nullptr)
				m_ambientTex->Bind(m_ambientTexSlot);

			if(m_diffuseTex != nullptr)
				m_diffuseTex->Bind(m_diffuseTexSlot);

			if (m_emissiveTex != nullptr)
				m_emissiveTex->Bind(m_emissiveTexSlot);

			if (m_specularTex != nullptr)
				m_specularTex->Bind(m_specularTexSlot);

			if (m_normalTex != nullptr)
				m_normalTex->Bind(m_normalTexSlot);			
		}
	}
}