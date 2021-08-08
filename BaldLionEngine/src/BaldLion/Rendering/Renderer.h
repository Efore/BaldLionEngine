#pragma once

#include "Shader.h"
#include "LightManager.h"
#include "SkyboxPlatformInterface.h"
#include "RendererPlatformInterface.h"
#include "Model.h"
#include "Framebuffer.h"
#include "Framebuffer.h"

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

			static void Init(ui32 width, ui32 height);
			static void Stop();

			static void OnWindowResize(ui32 width, ui32 height);

			static void BeginScene(const DirectionalLight& directionalLight);
			static void DrawScene();
			static void EndScene();

			static ShaderLibrary& GetShaderLibrary() { return s_shaderLibrary; }
			static TextureLibrary& GetTextureLibrary() { return s_textureLibrary; }
			static SceneData& GetSceneData() { return s_sceneData; }
			static RendererStats& GetRenderStats() { return s_renderStats; }
			static Framebuffer* GetFrameBuffer() { return s_framebuffer; }

			static void Draw(const VertexArray* vertexArray, Shader* shader, bool receiveShadows, const glm::mat4& transform = glm::mat4(1.0f));

			static void RegisterModel(Model* model);
			static void UnregisterModel(Model* model);

			static void RegisterMesh(Mesh* mesh);
			static void UnregisterMesh(Mesh* mesh);

			static void ProcessFrustrumCulling();			

			inline static RendererPlatformInterface::RendererPlatform GetAPI() { return RendererPlatformInterface::GetAPI(); }

		private:

			static void AddToBatch( Mesh* mesh);
			static void ProcessFrustrumCullingParallel(ui32 initialMeshIndex, ui32 finalMeshIndex);
			
			static void CreateShadowMap();
			static void RenderStatictGeometry();
			static void RenderDynamicGeometry();

		private:

			static RendererStats s_renderStats;
			static SceneData s_sceneData;
			static ShaderLibrary s_shaderLibrary;
			static TextureLibrary s_textureLibrary;
			static RendererPlatformInterface* s_rendererPlatformInterface;
			static SkyboxPlatformInterface* s_skyboxPlatformInterface;
						
			static DynamicArray<Mesh*> s_registeredMeshes;
			static DynamicArray<Mesh*> s_dynamicMeshesToRender;	

			static DynamicArray<Mesh*> s_castingShadowMeshes;

			static HashTable<Material*, GeometryData*> s_geometryToBatch;
			static DynamicArray<VertexArray*> s_batchedVertexArrays;

			static Framebuffer* s_framebuffer;
			static Framebuffer* s_shadowFramebuffer;

			static Texture2D* s_shadowMapTex;
			static Shader* s_depthMapShader;
			static Shader* s_depthMapSkinnedShader;

			static glm::mat4 s_lightViewProjection;

			static std::mutex s_geometryToBatchMutex;
			static std::mutex s_dynamicMeshesToRenderMutex;
			static std::mutex s_castingShadowMeshesMutex;
		};
	}
}
