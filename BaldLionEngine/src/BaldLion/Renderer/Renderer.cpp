#include "blpch.h"
#include "Renderer.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace BaldLion
{
	Renderer::SceneData* Renderer::m_steneData = new Renderer::SceneData;

	void Renderer::BeginScene(const std::shared_ptr<ProjectionCamera> & camera, const glm::vec3 direction)
	{
		camera->SetDirection(direction);
		m_steneData->ViewProjectionMatrix = camera->GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, const glm::mat4& transform)
	{
		shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniform("u_viewProjection", ShaderDataType::Mat4, &(m_steneData->ViewProjectionMatrix));
		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniform("u_transform", ShaderDataType::Mat4, &(transform[0][0]));

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}