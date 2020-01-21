#include "blpch.h"
#include "Renderer.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace BaldLion
{
	Renderer::SceneData* Renderer::m_sceneData = new Renderer::SceneData;
	ShaderLibrary Renderer::m_shaderLibrary;
	TextureLibrary Renderer::m_textureLibrary;

	void Renderer::Init()
	{
		RenderCommand::Init();
		LightManager::Init();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(const Ref<ProjectionCamera>& camera, const DirectionalLight& directionalLight, const std::vector<PointLight>& pointLights)
	{
		m_sceneData->ViewProjectionMatrix = camera->GetViewProjectionMatrix();
		m_sceneData->CameraPosition = camera->GetPosition();

		LightManager::BeginScene(directionalLight, pointLights);
	}

	void Renderer::EndScene()
	{
		
	}

	void Renderer::Submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform)
	{
		shader->Bind();

		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniform("u_transform", BaldLion::ShaderDataType::Mat4, &(transform[0][0]));
		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniform("u_viewProjection", BaldLion::ShaderDataType::Mat4, &(m_sceneData->ViewProjectionMatrix));
		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniform("u_cameraPos", BaldLion::ShaderDataType::Float3, &(m_sceneData->CameraPosition));

		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniform("u_directionalLight.direction", BaldLion::ShaderDataType::Float3, &(LightManager::GetDirectionalLight().direction));
		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniform("u_directionalLight.ambientColor", BaldLion::ShaderDataType::Float3, &(LightManager::GetDirectionalLight().ambientColor));
		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniform("u_directionalLight.diffuseColor", BaldLion::ShaderDataType::Float3, &(LightManager::GetDirectionalLight().diffuseColor));
		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniform("u_directionalLight.specularColor", BaldLion::ShaderDataType::Float3, &(LightManager::GetDirectionalLight().specularColor));

		size_t numPointLights = LightManager::GetScenePointLights().size();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniform("u_numPointLights", BaldLion::ShaderDataType::Int, &(numPointLights));

		for (size_t i = 0; i < numPointLights; ++i)
		{	
			std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniform("u_pointLights[" + std::to_string(i) + "].position", BaldLion::ShaderDataType::Float3, &(LightManager::GetScenePointLights()[i].position));
			std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniform("u_pointLights[" + std::to_string(i) + "].constant", BaldLion::ShaderDataType::Float, &(LightManager::GetScenePointLights()[i].constant));
			std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniform("u_pointLights[" + std::to_string(i) + "].linear", BaldLion::ShaderDataType::Float, &(LightManager::GetScenePointLights()[i].linear));
			std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniform("u_pointLights[" + std::to_string(i) + "].quadratic", BaldLion::ShaderDataType::Float, &(LightManager::GetScenePointLights()[i].quadratic));
			std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniform("u_pointLights[" + std::to_string(i) + "].ambientColor", BaldLion::ShaderDataType::Float3, &(LightManager::GetScenePointLights()[i].ambientColor));
			std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniform("u_pointLights[" + std::to_string(i) + "].diffuseColor", BaldLion::ShaderDataType::Float3, &(LightManager::GetScenePointLights()[i].diffuseColor));
			std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniform("u_pointLights[" + std::to_string(i) + "].specularColor", BaldLion::ShaderDataType::Float3, &(LightManager::GetScenePointLights()[i].specularColor));

		}
		
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}