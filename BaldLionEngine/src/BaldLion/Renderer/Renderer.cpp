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
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(const Ref<ProjectionCamera>& camera, const glm::vec3& lightPosition)
	{
		m_sceneData->ViewProjectionMatrix = camera->GetViewProjectionMatrix();
		m_sceneData->CameraPosition = camera->GetPosition();
		m_sceneData->LightPosition = lightPosition;
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
		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniform("u_lightPos", BaldLion::ShaderDataType::Float3, &(m_sceneData->LightPosition));

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}