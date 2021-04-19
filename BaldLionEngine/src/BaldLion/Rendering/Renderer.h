#pragma once

#include "Camera.h"
#include "Shader.h"
#include "LightManager.h"
#include "SkyboxPlatformInterface.h"
#include "RendererPlatformInterface.h"
#include "AnimatedModel.h"

namespace BaldLion
{
	namespace Rendering
	{
		enum class RenderingGroup
		{
			StaticMesh,
			DynamicMesh,
			SkinnedMesh,
			UI,
			Particles,

			Count
		};

		class Renderer
		{
		public:
			struct SceneData
			{
				glm::mat4 viewProjectionMatrix;
				glm::vec3 cameraPosition;
			};

			struct RendererStats {

				ui32 drawCalls;
				ui32 vertices;				
				float avgFramesPerSecond;
			};

			static void Init();
			static void Stop();

			static void OnWindowResize(ui32 width, ui32 height);

			static void BeginScene(const Camera* camera, const DirectionalLight& directionalLight);
			static void DrawScene(const Camera* camera);
			static void EndScene();

			static ShaderLibrary& GetShaderLibrary() { return s_shaderLibrary; }
			static TextureLibrary& GetTextureLibrary() { return s_textureLibrary; }
			static SceneData& GetSceneData() { return s_sceneData; }
			static RendererStats& GetRenderStats() { return s_renderStats; }

			static void Draw(const VertexArray* vertexArray, Shader* shader, const glm::mat4& transform = glm::mat4(1.0f));

			static void RegisterModel(Model* model);
			static void UnregisterModel(Model* model);

			static void ProcessFrustrumCulling(const Camera* camera);

			inline static RendererPlatformInterface::RendererPlatform GetAPI() { return RendererPlatformInterface::GetAPI(); }

		private:

			static void AddToBatch( Mesh* mesh);

		private:

			static RendererStats s_renderStats;
			static SceneData s_sceneData;
			static ShaderLibrary s_shaderLibrary;
			static TextureLibrary s_textureLibrary;
			static RendererPlatformInterface* s_rendererPlatformInterface;
			static SkyboxPlatformInterface* s_skyboxPlatformInterface;

			static DynamicArray<Model*> s_registeredModels;
			static DynamicArray<Mesh*> s_meshesToRender;	

			static HashTable<Material*, GeometryData*> s_geometryToBatch;
			static DynamicArray<VertexArray*> s_batchedVertexArrays;

			static std::mutex s_geometryToBatchMutex;
			static std::mutex s_meshesToRenderMutex;
		};
	}
}
