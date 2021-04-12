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

			OpenGLMaterial(const std::string& matName, const MaterialProperties& materialProperties);

			virtual void AssignShader() override;

			virtual void SetAmbientColor(const glm::vec3 & emissive) override;
			virtual void SetEmissiveColor(const glm::vec3 & emissive) override;
			virtual void SetDiffuseColor(const glm::vec3 & diffuse) override;
			virtual void SetSpecularColor(const glm::vec3 & specular) override;
			virtual void SetShininess(float value) override;

			virtual void SetUniform(StringId uniformName, ShaderDataType dataType, const void* uniformIndex) override;

			virtual Shader* GetShader() const override { return m_shader; }

			virtual void Bind() const override;
			virtual void Unbind() const override;

		private:
			OpenGLShader* m_shader;
			
			int m_ambientTexSlot = 0;
			int m_useAmbientTex = 0;
			
			int m_diffuseTexSlot = 0;
			int m_useDiffuseTex = 0;
			
			int m_emissiveTexSlot = 0;
			int m_useEmissiveTex = 0;
			
			int m_specularTexSlot = 0;
			int m_useSpecularTex = 0;
			
			int m_normalTexSlot = 0;
			int m_useNormalTex = 0;
		};
	}
}