#include "blpch.h"
#include "Renderer.h"
#include "RenderCommand.h"

namespace BaldLion
{
	Renderer::SceneData* Renderer::m_steneData = new Renderer::SceneData;

	void Renderer::BeginScene(const std::shared_ptr<ProjectionCamera> & camera, const glm::vec3& position, const glm::vec3 direction)
	{
		camera->SetPosition(position);
		camera->SetDirection(direction);

		m_steneData->ViewProjectionMatrix = camera->GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader)
	{
		shader->Bind();
		shader->SetUniform("MVP", ShaderDataType::Mat4, &(m_steneData->ViewProjectionMatrix * glm::mat4(1.0f)[0][0]));

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}