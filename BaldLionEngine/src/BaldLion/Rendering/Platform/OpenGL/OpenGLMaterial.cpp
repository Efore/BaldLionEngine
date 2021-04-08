#include "blpch.h"
#include "OpenGLMaterial.h"
#include "BaldLion/Rendering/Renderer.h"

namespace BaldLion
{
	namespace Rendering
	{
		OpenGLMaterial::OpenGLMaterial(const std::string& matName, const MaterialProperties& materialProperties) :
			m_ambientColor(materialProperties.ambientColor),
			m_diffuseColor(materialProperties.diffuseColor),
			m_emissiveColor(materialProperties.emissiveColor),
			m_specularColor(materialProperties.specularColor),
			m_shininess(materialProperties.shininess)
		{
			BL_PROFILE_FUNCTION();		

			m_shaderPath = materialProperties.shaderPath;
			m_materialName = STRING_TO_ID(matName);

			short slotIndex = 0;

			if (materialProperties.ambientTex != nullptr)
			{
				m_ambientTexSlot = slotIndex++;
				m_ambientTex = (OpenGLTexture2D*)materialProperties.ambientTex;
				m_useAmbientTex = 1;
			}

			if (materialProperties.diffuseTex != nullptr)
			{
				m_diffuseTexSlot = slotIndex++;
				m_diffuseTex = (OpenGLTexture2D*)(materialProperties.diffuseTex);
				m_useDiffuseTex = 1;
			}

			if (materialProperties.emissiveTex != nullptr)
			{
				m_emissiveTexSlot = slotIndex++;
				m_emissiveTex = (OpenGLTexture2D*)(materialProperties.emissiveTex);
				m_useEmissiveTex = 1;
			}

			if (materialProperties.specularTex != nullptr)
			{
				m_specularTexSlot = slotIndex++;
				m_specularTex = (OpenGLTexture2D*)(materialProperties.specularTex);
				m_useSpecularTex = 1;
			}

			if (materialProperties.normalTex)
			{
				m_normalTexSlot = slotIndex++;
				m_normalTex = (OpenGLTexture2D*)(materialProperties.normalTex);
				m_useNormalTex = 1;
			}
		}

		void OpenGLMaterial::AssignShader()
		{
			m_shader = (OpenGLShader*)(Shader::Create(ID_TO_STRING(m_shaderPath)));
			m_shader->Bind();

			m_shader->SetUniform(MATKEY_AMBIENT_COLOR, ShaderDataType::Float3, &m_ambientColor);
			m_shader->SetUniform(MATKEY_DIFFUSE_COLOR, ShaderDataType::Float3, &m_diffuseColor);
			m_shader->SetUniform(MATKEY_EMISSIVE_COLOR, ShaderDataType::Float3, &m_emissiveColor);
			m_shader->SetUniform(MATKEY_SPECULAR_COLOR, ShaderDataType::Float3, &m_specularColor);
			m_shader->SetUniform(MATKEY_SHININESS, ShaderDataType::Float, &m_shininess);

			if (m_ambientTex != nullptr)
			{
				m_shader->SetUniform(MATKEY_AMBIENT_TEX, ShaderDataType::Int, &m_ambientTexSlot);
				m_useAmbientTex = 1;
			}
			m_shader->SetUniform(MATKEY_USE_AMBIENT_TEX, ShaderDataType::Int, &m_useAmbientTex);

			if (m_diffuseTex != nullptr)
			{
				m_shader->SetUniform(MATKEY_DIFFUSE_TEX, ShaderDataType::Int, &m_diffuseTexSlot);
				m_useDiffuseTex = 1;
			}
			m_shader->SetUniform(MATKEY_USE_DIFFUSE_TEX, ShaderDataType::Int, &m_useDiffuseTex);

			if (m_emissiveTex != nullptr)
			{
				m_shader->SetUniform(MATKEY_EMISSIVE_TEX, ShaderDataType::Int, &m_emissiveTexSlot);
				m_useEmissiveTex = 1;
			}
			m_shader->SetUniform(MATKEY_USE_EMISSIVE_TEX, ShaderDataType::Int, &m_useEmissiveTex);

			if (m_specularTex != nullptr)
			{
				m_shader->SetUniform(MATKEY_SPECULAR_TEX, ShaderDataType::Int, &m_specularTexSlot);
				m_useSpecularTex = 1;
			}
			m_shader->SetUniform(MATKEY_USE_SPECULAR_TEX, ShaderDataType::Int, &m_useSpecularTex);

			if (m_normalTex != nullptr)
			{
				m_shader->SetUniform(MATKEY_NORMAL_TEX, ShaderDataType::Int, &m_normalTexSlot);
				m_useNormalTex = 1;
			}
			m_shader->SetUniform(MATKEY_USE_NORMAL_TEX, ShaderDataType::Int, &m_useNormalTex);
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

		void OpenGLMaterial::SetUniform(StringId uniformName, ShaderDataType dataType, const void* uniformIndex)
		{
			BL_PROFILE_FUNCTION();

			m_shader->SetUniform(uniformName, dataType, uniformIndex);
		}

		void OpenGLMaterial::Bind() const
		{
			BL_PROFILE_FUNCTION();
			m_shader->Bind();

			if (m_ambientTex != nullptr)
				m_ambientTex->Bind(m_ambientTexSlot);

			if (m_diffuseTex != nullptr)
				m_diffuseTex->Bind(m_diffuseTexSlot);

			if (m_emissiveTex != nullptr)
				m_emissiveTex->Bind(m_emissiveTexSlot);

			if (m_specularTex != nullptr)
				m_specularTex->Bind(m_specularTexSlot);

			if (m_normalTex != nullptr)
				m_normalTex->Bind(m_normalTexSlot);
		}

		void OpenGLMaterial::Unbind() const
		{
			m_shader->Unbind();
		}

	}
}