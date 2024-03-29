#include "blpch.h"
#include "OpenGLSkybox.h"
#include "OpenGLTextureCubemap.h"
#include "OpenGLShader.h"
#include "BaldLion/Rendering/Renderer.h"
#include "BaldLion/ResourceManagement/ResourceManager.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace BaldLion
{
	namespace Rendering
	{

		void OpenGLSkybox::Init(const std::string& path)
		{
			SkyboxPlatformInterface::Init(path);			
				
			m_cubemapTexture = (OpenGLTextureCubemap*)(TextureCubeMap::Create(path));
			ResourceManagement::ResourceManager::AddResource(m_cubemapTexture);

			m_skyboxShader = (OpenGLShader*)(ResourceManagement::ResourceManager::AddResource<Shader>("assets/editorAssets/shaders/skybox.glsl", ResourceManagement::ResourceType::Shader));
		}

		void OpenGLSkybox::Draw()
		{
			int oldCullFaceMode;
			glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
			int oldDepthFuncMode;
			glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);

			glCullFace(GL_FRONT);
			glDepthFunc(GL_LEQUAL);
			m_skyboxShader->Bind();

			const Renderer::SceneData& sceneData = Renderer::GetSceneData();

			int skybox = 0;
			m_skyboxShader->SetUniform(BL_STRING_TO_STRINGID("skybox"),ShaderDataType::Int, &skybox);
			m_skyboxShader->SetUniform(BL_STRING_TO_STRINGID("u_worldTransformMatrix"), ShaderDataType::Mat4, &(glm::scale(glm::mat4(1.0f), glm::vec3(49000, 49000, 49000))[0][0]));
			m_skyboxShader->SetUniform(BL_STRING_TO_STRINGID("u_viewProjectionMatrix"), ShaderDataType::Mat4, &(sceneData.viewProjectionMatrix));

			m_vertexArray->Bind();
			m_cubemapTexture->Bind();					

			glDrawElements(GL_QUADS, m_vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

			m_vertexArray->Unbind();

			glCullFace(oldCullFaceMode);
			glDepthFunc(oldDepthFuncMode);
		}

	}
}