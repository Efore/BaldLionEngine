#include "blpch.h"
#include "Renderer.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace BaldLion
{
	Renderer::SceneData* Renderer::m_steneData = new Renderer::SceneData;
	ShaderLibrary Renderer::m_shaderLibrary;

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(ProjectionCamera& camera, const glm::vec3 direction)
	{
		camera.SetDirection(direction);
		m_steneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
		
	}

	void Renderer::Submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform)
	{
		shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniform("u_transform", BaldLion::ShaderDataType::Mat4, &(transform[0][0]));
		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniform("u_viewProjection", BaldLion::ShaderDataType::Mat4, &(m_steneData->ViewProjectionMatrix));

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}