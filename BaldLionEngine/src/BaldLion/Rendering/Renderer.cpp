#include "blpch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLRenderer.h"
#include "Platform/OpenGL/OpenGLSkybox.h"

namespace BaldLion
{
	namespace Rendering
	{
		Renderer::SceneData Renderer::s_sceneData;
		ShaderLibrary Renderer::s_shaderLibrary;
		TextureLibrary Renderer::s_textureLibrary;
		RendererPlatformInterface* Renderer::s_rendererPlatformInterface;
		SkyboxPlatformInterface* Renderer::s_skyboxPlatformInterface;

		void Renderer::Init()
		{
			BL_PROFILE_FUNCTION();

			s_textureLibrary.Init();
			s_shaderLibrary.Init();

			s_rendererPlatformInterface = MemoryManager::New<OpenGLRenderer>(STRING_TO_ID("Renderer Platform interface"), AllocationType::FreeList_Renderer);
			s_rendererPlatformInterface->Init();

			s_skyboxPlatformInterface = MemoryManager::New<OpenGLSkybox>(STRING_TO_ID("Skybox Platform interface"), AllocationType::FreeList_Renderer);
			s_skyboxPlatformInterface->Init("assets/textures/skybox/skybox.jpg");

			LightManager::Init();
		}

		void Renderer::Stop()
		{
			MemoryManager::Delete(s_rendererPlatformInterface);
			MemoryManager::Delete(s_skyboxPlatformInterface);
			s_shaderLibrary.Clear();
			s_textureLibrary.Clear();
		}

		void Renderer::OnWindowResize(ui32 width, ui32 height)
		{
			s_rendererPlatformInterface->SetViewport(0, 0, width, height);			
		}

		void Renderer::BeginScene(const ProjectionCamera*  camera, const DirectionalLight& directionalLight)
		{
			BL_PROFILE_FUNCTION();
			
			s_rendererPlatformInterface->SetClearColor({ 0.3f, 0.3f, 0.8f, 1.0f });
			s_rendererPlatformInterface->Clear();

			s_sceneData.viewProjectionMatrix = camera->GetViewProjectionMatrix();
			s_sceneData.cameraPosition = camera->GetPosition();

			LightManager::BeginScene(directionalLight);
		}

		void Renderer::EndScene()
		{
			s_skyboxPlatformInterface->Draw();
		}

		void Renderer::Submit(const VertexArray* vertexArray, Shader* shader, const glm::mat4& transform)
		{
			BL_PROFILE_FUNCTION();

			shader->SetUniform(STRING_TO_ID("u_worldTransformMatrix"), ShaderDataType::Mat4, &(transform[0][0]));
			shader->SetUniform(STRING_TO_ID("u_viewProjectionMatrix"), ShaderDataType::Mat4, &(s_sceneData.viewProjectionMatrix));
			shader->SetUniform(STRING_TO_ID("u_cameraPos"), ShaderDataType::Float3, &(s_sceneData.cameraPosition));

			shader->SetUniform(STRING_TO_ID("u_directionalLight.direction"), ShaderDataType::Float3, &(LightManager::GetDirectionalLight().direction));
			shader->SetUniform(STRING_TO_ID("u_directionalLight.ambientColor"),ShaderDataType::Float3, &(LightManager::GetDirectionalLight().ambientColor));
			shader->SetUniform(STRING_TO_ID("u_directionalLight.diffuseColor"), ShaderDataType::Float3, &(LightManager::GetDirectionalLight().diffuseColor));
			shader->SetUniform(STRING_TO_ID("u_directionalLight.specularColor"), ShaderDataType::Float3, &(LightManager::GetDirectionalLight().specularColor));

			vertexArray->Bind();
			s_rendererPlatformInterface->DrawIndexed(vertexArray);
			vertexArray->Unbind();			
		}
	}
}