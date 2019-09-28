#pragma once

#include "RendererAPI.h"
#include "VertexArray.h"
#include "Camera.h"
#include "Shader.h"

namespace BaldLion
{
	class Renderer
	{	
	public:
		static void Init();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(ProjectionCamera& camera, const glm::vec3 direction);
		static void EndScene();

		static ShaderLibrary& GetShaderLibrary() { return m_shaderLibrary; }

		static void Submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform = glm::mat4(1.0f));

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData 
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* m_steneData;
		static ShaderLibrary m_shaderLibrary;
	};
}
