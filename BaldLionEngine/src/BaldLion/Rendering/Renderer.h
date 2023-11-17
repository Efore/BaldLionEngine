#pragma once

#include "Shader.h"
#include "SkyboxPlatformInterface.h"
#include "RendererPlatformInterface.h"
#include "Model.h"
#include "Framebuffer.h"
#include "BaldLion/Core/Containers/HashMap.h"

#include "BaldLion/ECS/Components/ECSMeshComponent.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/ECS/Components/ECSSkeletonComponent.h"


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

			Debug,
			Count
		};	

		struct RenderMeshData {

			const glm::mat4 transformMatrix;
			const ECS::ECSMeshComponent* meshComponent;
			const ECS::ECSSkeletonComponent* skeletonComponent;
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

			static void BeginScene();
			static void DrawScene();
			static void EndScene();

			static SceneData& GetSceneData() { return s_sceneData; }
			static RendererStats& GetRenderStats() { return s_renderStats; }
			static Framebuffer* GetFrameBuffer() { return s_framebuffer; }

			static void Draw(const VertexArray* vertexArray, Shader* shader, bool receiveShadows, const glm::mat4& transform = glm::mat4(1.0f));	

			inline static RendererPlatformInterface::RendererPlatform GetAPI() { return RendererPlatformInterface::GetAPI(); }
			
			static void AddMeshToDraw(const ECS::ECSMeshComponent* meshComponent, const ECS::ECSTransformComponent* meshTransform, const ECS::ECSSkeletonComponent* skeletonComponent);
			static void AddShadowCastingMesh(const ECS::ECSMeshComponent* meshComponent, const ECS::ECSTransformComponent* meshTransform, const ECS::ECSSkeletonComponent* skeletonComponent);

			static void DrawDebugBox(const glm::vec3& center, const glm::vec3& size, const glm::mat4& transformMatrix, const glm::vec3& color, int durationMs = 0, bool depthEnabled = true);
			static void DrawDebugCircle(const glm::vec3& center, const glm::vec3& normal, float radius, ui32 numSteps, const glm::vec3& color, int durationMs = 0, bool depthEnabled = true);
			static void DrawDebugSphere(const glm::vec3& center, float radius, const glm::vec3& color, int durationMs = 0, bool depthEnabled = true);
			static void DrawDebugCapsule(const glm::vec3& center, const glm::mat4& transformMatrix, float radius, float height, const glm::vec3& color, int durationMs = 0, bool depthEnabled = true);
			static void DrawDebugLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color, float arrowSize = 0.0f, int durationMs = 0, bool depthEnabled = true);
			static void DrawDebugFrustrum(const glm::mat4& invClipMatrix, const glm::vec3& color, int durationMs = 0, bool depthEnabled = true);
			static void DrawDebug3DText(const std::string& text, const glm::vec3& worldPos, const glm::vec3& color, int viewportCoordX, int viewportCoordY, int viewportWidth, int viewportHeight, float scaling = 1.0f, int durationMs = 0);
			static void DrawDebugTriangle(const glm::vec3& vertex1, const glm::vec3& vertex2, const glm::vec3& vertex3, const glm::vec3& color, float alpha = 1.0f, int durationMs = 0, bool depthEnabled = true);
			static void DrawDebugPoint(const glm::vec3& position, const glm::vec3& color, float size = 5.0f, int durationMs = 0, bool depthEnabled = true);

		private:
			
			static void DrawShadowMap();
			static void DrawMeshes();
			static void DrawDebugCommands();		

		private:

			static RendererStats s_renderStats;
			static SceneData s_sceneData;
			static RendererPlatformInterface* s_rendererPlatformInterface;
			static SkyboxPlatformInterface* s_skyboxPlatformInterface;			
			static class DebugDrawRenderInterface* s_debugDrawRender;

			static DynamicArray<RenderMeshData> s_shadowCastingMeshes;
			static DynamicArray<RenderMeshData> s_meshesToDraw;

			static Framebuffer* s_framebuffer;
			static Framebuffer* s_shadowFramebuffer;

			static Texture2D* s_shadowMapTex;
			static Shader* s_depthMapShader;
			static Shader* s_depthMapSkinnedShader;

			static glm::mat4 s_lightViewProjection;

			static std::mutex s_addDynamicMeshMutex;
			static std::mutex s_addShadowCastingMeshMutex;
		};
	}
}
