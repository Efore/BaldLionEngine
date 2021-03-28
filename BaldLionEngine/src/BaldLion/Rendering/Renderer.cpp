#include "blpch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLRenderer.h"
#include "Platform/OpenGL/OpenGLSkybox.h"

namespace BaldLion
{
	namespace Rendering
	{
		Renderer::RendererStats Renderer::s_renderStats;
		Renderer::SceneData Renderer::s_sceneData;
		ShaderLibrary Renderer::s_shaderLibrary;
		TextureLibrary Renderer::s_textureLibrary;
		RendererPlatformInterface* Renderer::s_rendererPlatformInterface;
		SkyboxPlatformInterface* Renderer::s_skyboxPlatformInterface;
		DynamicArray<Model*> Renderer::s_modelsToRender; 

		void Renderer::Init()
		{
			BL_PROFILE_FUNCTION();

			s_textureLibrary.Init();
			s_shaderLibrary.Init();

			s_rendererPlatformInterface = MemoryManager::New<OpenGLRenderer>(STRING_TO_ID("Renderer Platform interface"), AllocationType::FreeList_Renderer);
			s_rendererPlatformInterface->Init();

			s_skyboxPlatformInterface = MemoryManager::New<OpenGLSkybox>(STRING_TO_ID("Skybox Platform interface"), AllocationType::FreeList_Renderer);
			s_skyboxPlatformInterface->Init("assets/textures/skybox/skybox.jpg");

			s_modelsToRender = DynamicArray<Model*>(AllocationType::FreeList_Renderer, 500);			

			LightManager::Init();
		}

		void Renderer::Stop()
		{
			MemoryManager::Delete(s_rendererPlatformInterface);
			MemoryManager::Delete(s_skyboxPlatformInterface);
			s_shaderLibrary.Clear();
			s_textureLibrary.Clear();
			s_modelsToRender.Clear();
		}

		void Renderer::OnWindowResize(ui32 width, ui32 height)
		{
			s_rendererPlatformInterface->SetViewport(0, 0, width, height);			
		}

		void Renderer::BeginScene(const Camera*  camera, const DirectionalLight& directionalLight)
		{
			BL_PROFILE_FUNCTION();
			
			s_rendererPlatformInterface->SetClearColor({ 0.3f, 0.3f, 0.8f, 1.0f });
			s_rendererPlatformInterface->Clear();

			s_sceneData.viewProjectionMatrix = camera->GetViewProjectionMatrix();
			s_sceneData.cameraPosition = camera->GetPosition();
			
			LightManager::BeginScene(directionalLight);
		}

		void Renderer::DrawScene(const Camera* camera)
		{
			for (ui32 i = 0; i < s_modelsToRender.Size(); ++i)
			{
				for (ui32 j = 0; j < s_modelsToRender[i]->GetSubMeshes().Size(); ++j)
				{
					if (camera->IsAABBVisible(s_modelsToRender[i]->GetSubMeshes()[j]->GetAABB(), s_modelsToRender[i]->GetWorldTransform()))
					{
						s_modelsToRender[i]->GetSubMeshes()[j]->Draw(s_modelsToRender[i]->GetWorldTransform());
					}
				}
			}
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

			s_renderStats.drawCalls++;
			s_renderStats.vertices += vertexArray->GetIndexBuffer()->GetCount();
		}

		void Renderer::SubscribeModel(Model* model)
		{
			if (!s_modelsToRender.Contains(model))
			{
				s_modelsToRender.PushBack(model);
			}
		}

		void Renderer::UnsubscribeModel(Model* model)
		{
			if (s_modelsToRender.Contains(model))
			{
				s_modelsToRender.RemoveFast(model);
			}
		}

	}
}