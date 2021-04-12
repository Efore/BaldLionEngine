#include "blpch.h"
#include "OpenGLMaterial.h"
#include "BaldLion/Rendering/Renderer.h"

#include <glad/glad.h>

namespace BaldLion
{
	namespace Rendering
	{
		OpenGLMaterial::OpenGLMaterial(const std::string& matName, const MaterialProperties& materialProperties) : Material(materialProperties)			
		{
			BL_PROFILE_FUNCTION();		

			m_materialName = STRING_TO_ID(matName);

			short slotIndex = 0;

			if (materialProperties.ambientTex != nullptr)
			{
				m_ambientTexSlot = slotIndex++;				
				m_useAmbientTex = 1;
			}

			if (materialProperties.diffuseTex != nullptr)
			{
				m_diffuseTexSlot = slotIndex++;
				m_useDiffuseTex = 1;
			}

			if (materialProperties.emissiveTex != nullptr)
			{
				m_emissiveTexSlot = slotIndex++;
				m_useEmissiveTex = 1;
			}

			if (materialProperties.specularTex != nullptr)
			{
				m_specularTexSlot = slotIndex++;
				m_useSpecularTex = 1;
			}

			if (materialProperties.normalTex)
			{
				m_normalTexSlot = slotIndex++;
				m_useNormalTex = 1;
			}
		}

		void OpenGLMaterial::AssignShader()
		{
			m_shader = (OpenGLShader*)(Shader::Create(ID_TO_STRING(m_materialProperties.shaderPath)));
			m_shader->Bind();

			m_shader->SetUniform(MATKEY_AMBIENT_COLOR, ShaderDataType::Float3, &m_materialProperties.ambientColor);
			m_shader->SetUniform(MATKEY_DIFFUSE_COLOR, ShaderDataType::Float3, &m_materialProperties.diffuseColor);
			m_shader->SetUniform(MATKEY_EMISSIVE_COLOR, ShaderDataType::Float3, &m_materialProperties.emissiveColor);
			m_shader->SetUniform(MATKEY_SPECULAR_COLOR, ShaderDataType::Float3, &m_materialProperties.specularColor);
			m_shader->SetUniform(MATKEY_SHININESS, ShaderDataType::Float, &m_materialProperties.shininess);

			if (m_materialProperties.ambientTex != nullptr)
			{
				m_shader->SetUniform(MATKEY_AMBIENT_TEX, ShaderDataType::Int, &m_ambientTexSlot);
				m_useAmbientTex = 1;
			}
			m_shader->SetUniform(MATKEY_USE_AMBIENT_TEX, ShaderDataType::Int, &m_useAmbientTex);

			if (m_materialProperties.diffuseTex != nullptr)
			{
				m_shader->SetUniform(MATKEY_DIFFUSE_TEX, ShaderDataType::Int, &m_diffuseTexSlot);
				m_useDiffuseTex = 1;
			}
			m_shader->SetUniform(MATKEY_USE_DIFFUSE_TEX, ShaderDataType::Int, &m_useDiffuseTex);

			if (m_materialProperties.emissiveTex != nullptr)
			{
				m_shader->SetUniform(MATKEY_EMISSIVE_TEX, ShaderDataType::Int, &m_emissiveTexSlot);
				m_useEmissiveTex = 1;
			}
			m_shader->SetUniform(MATKEY_USE_EMISSIVE_TEX, ShaderDataType::Int, &m_useEmissiveTex);

			if (m_materialProperties.specularTex != nullptr)
			{
				m_shader->SetUniform(MATKEY_SPECULAR_TEX, ShaderDataType::Int, &m_specularTexSlot);
				m_useSpecularTex = 1;
			}
			m_shader->SetUniform(MATKEY_USE_SPECULAR_TEX, ShaderDataType::Int, &m_useSpecularTex);

			if (m_materialProperties.normalTex != nullptr)
			{
				m_shader->SetUniform(MATKEY_NORMAL_TEX, ShaderDataType::Int, &m_normalTexSlot);
				m_useNormalTex = 1;
			}
			m_shader->SetUniform(MATKEY_USE_NORMAL_TEX, ShaderDataType::Int, &m_useNormalTex);
		}

		void OpenGLMaterial::SetAmbientColor(const glm::vec3 & ambient)
		{
			m_materialProperties.emissiveColor = ambient;
			m_shader->SetUniform(MATKEY_AMBIENT_COLOR, ShaderDataType::Float3, &m_materialProperties.ambientColor);
		}

		void OpenGLMaterial::SetEmissiveColor(const glm::vec3 & emissive)
		{
			m_materialProperties.emissiveColor = emissive;
			m_shader->SetUniform(MATKEY_EMISSIVE_COLOR, ShaderDataType::Float3, &m_materialProperties.emissiveColor);
		}

		void OpenGLMaterial::SetDiffuseColor(const glm::vec3 & diffuse)
		{
			m_materialProperties.diffuseColor = diffuse;
			m_shader->SetUniform(MATKEY_DIFFUSE_COLOR, ShaderDataType::Float3, &m_materialProperties.diffuseColor);
		}

		void OpenGLMaterial::SetSpecularColor(const glm::vec3 & specular)
		{
			m_materialProperties.specularColor = specular;
			m_shader->SetUniform(MATKEY_SPECULAR_COLOR, ShaderDataType::Float3, &m_materialProperties.specularColor);
		}

		void OpenGLMaterial::SetShininess(float value)
		{
			m_materialProperties.shininess = value;
			m_shader->SetUniform(MATKEY_SHININESS, ShaderDataType::Float, &m_materialProperties.shininess);
		}

		void OpenGLMaterial::SetUniform(StringId uniformName, ShaderDataType dataType, const void* uniformIndex)
		{
			BL_PROFILE_FUNCTION();

			m_shader->SetUniform(uniformName, dataType, uniformIndex);
		}

		void OpenGLMaterial::Bind() const
		{
			BL_PROFILE_FUNCTION();

			switch (m_materialProperties.blendMode)
			{
				case Material::BlendMode::None:
					glDisable(GL_BLEND);
					break;
				case Material::BlendMode::Alpha:
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glEnable(GL_BLEND);
					break;
				case Material::BlendMode::Aditive:
					glBlendFunc(GL_ONE, GL_ONE);
					glEnable(GL_BLEND);
					break;
				default:
					break;
			}

			switch (m_materialProperties.depthBufferMode)
			{
			case DepthBufferMode::NoDepth:
				glDisable(GL_DEPTH_TEST);
				glDepthMask(GL_FALSE);
				break;

			case DepthBufferMode::TestOnly:
				glEnable(GL_DEPTH_TEST);
				glDepthMask(GL_FALSE);
				glDepthFunc(GL_LEQUAL);
				break;

			case DepthBufferMode::TestAndWrite:
				glEnable(GL_DEPTH_TEST);
				glDepthMask(GL_TRUE);
				glDepthFunc(GL_LESS);
				break;

			default:
				break;
			}

			switch (m_materialProperties.cullingMode)
			{
			case CullingMode::Back:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				break;

			case CullingMode::Front:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				break;

			case CullingMode::None:
				glDisable(GL_CULL_FACE);
				break;

			default:
				break;
			}


			m_shader->Bind();

			if (m_materialProperties.ambientTex != nullptr)
				m_materialProperties.ambientTex->Bind(m_ambientTexSlot);

			if (m_materialProperties.diffuseTex != nullptr)
				m_materialProperties.diffuseTex->Bind(m_diffuseTexSlot);

			if (m_materialProperties.emissiveTex != nullptr)
				m_materialProperties.emissiveTex->Bind(m_emissiveTexSlot);

			if (m_materialProperties.specularTex != nullptr)
				m_materialProperties.specularTex->Bind(m_specularTexSlot);

			if (m_materialProperties.normalTex != nullptr)
				m_materialProperties.normalTex->Bind(m_normalTexSlot);
		}

		void OpenGLMaterial::Unbind() const
		{
			m_shader->Unbind();
		}

	}
}