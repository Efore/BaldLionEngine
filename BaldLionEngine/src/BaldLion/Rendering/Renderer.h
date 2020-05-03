#pragma once

#include "Camera.h"
#include "Shader.h"
#include "LightManager.h"
#include "SkyboxPlatformInterface.h"
#include "RendererPlatformInterface.h"

namespace BaldLion
{
	namespace Rendering
	{
		class Renderer
		{
		public:
			struct SceneData
			{
				glm::mat4 ViewProjectionMatrix;
				glm::vec3 CameraPosition;
			};

			static void Init();
			static void OnWindowResize(uint32_t width, uint32_t height);

			static void BeginScene(const Ref<ProjectionCamera>& camera, const DirectionalLight& directionalLight, const std::vector<PointLight>& pointLights);
			static void EndScene();

			static ShaderLibrary& GetShaderLibrary() { return s_shaderLibrary; }
			static TextureLibrary& GetTextureLibrary() { return s_textureLibrary; }
			static SceneData& GetSceneData() { return s_sceneData; }

			static void Submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform = glm::mat4(1.0f));

			inline static RendererPlatformInterface::RendererPlatform GetAPI() { return RendererPlatformInterface::GetAPI(); }

		private:

			static SceneData s_sceneData;
			static ShaderLibrary s_shaderLibrary;
			static TextureLibrary s_textureLibrary;
			static Ref<RendererPlatformInterface> s_rendererPlatformInterface;
			static Ref<SkyboxPlatformInterface> s_skyboxPlatformInterface;
		};
	}
}
