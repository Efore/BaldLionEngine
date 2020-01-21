#pragma once

#include "RendererAPI.h"
#include "VertexArray.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "LightManager.h"

namespace BaldLion
{
	class Renderer
	{	
	public:
		static void Init();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(const Ref<ProjectionCamera>& camera, const DirectionalLight& directionalLight, const std::vector<PointLight>& pointLights);
		static void EndScene();

		static ShaderLibrary& GetShaderLibrary() { return m_shaderLibrary; }
		static TextureLibrary& GetTextureLibrary() { return m_textureLibrary; }

		static void Submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform = glm::mat4(1.0f));

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData 
		{
			glm::mat4 ViewProjectionMatrix;
			glm::vec3 CameraPosition;
		};

		static SceneData* m_sceneData;
		static ShaderLibrary m_shaderLibrary;
		static TextureLibrary m_textureLibrary;
	};
}
