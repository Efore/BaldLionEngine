#include "blpch.h"
#include "Renderer.h"

#include "BaldLion/Core/JobManagement/JobManager.h"
#include "BaldLion/Utils/MathUtils.h"

#include "BaldLion/ECS/ComponentsSingleton/ECSProjectionCameraSingleton.h"
#include "BaldLion/ECS/ComponentsSingleton/ECSLightSingleton.h"

#include "DebugDrawRenderInterface.h"
#include "DebugDrawRenderProvider.h"

const ui32 maxMeshesToProcess = 5u;

namespace BaldLion
{
	namespace Rendering
	{
		Renderer::RendererStats Renderer::s_renderStats;
		Renderer::SceneData Renderer::s_sceneData;
		RendererPlatformInterface* Renderer::s_rendererPlatformInterface;
		SkyboxPlatformInterface* Renderer::s_skyboxPlatformInterface;
		
		DebugDrawRenderInterface* Renderer::s_debugDrawRender;

		HashTable<Material*, GeometryData*> Renderer::s_geometryToBatch;

		DynamicArray<RenderMeshData> Renderer::s_shadowCastingMeshes;
		DynamicArray<RenderMeshData> Renderer::s_dynamicMeshes;
		DynamicArray<VertexArray*> Renderer::s_disposableVertexArrays;
		DynamicArray<std::function<void()>> Renderer::s_scheduledDebugDrawCommands;

		Framebuffer* Renderer::s_framebuffer;

		//Shadows
		Framebuffer* Renderer::s_shadowFramebuffer;
		Texture2D* Renderer::s_shadowMapTex;

		Shader* Renderer::s_depthMapShader;
		Shader* Renderer::s_depthMapSkinnedShader;

		glm::mat4 Renderer::s_lightViewProjection;

		//Mutexes
		std::mutex Renderer::s_geometryToBatchMutex;
		std::mutex Renderer::s_dynamicMeshesToRenderMutex;		
		std::mutex Renderer::s_shadowCastingMeshesMutex;

		void Renderer::Init(ui32 width, ui32 height)
		{
			BL_PROFILE_FUNCTION();

			s_rendererPlatformInterface = RendererPlatformInterface::Create();
			s_rendererPlatformInterface->Init();

			s_skyboxPlatformInterface = SkyboxPlatformInterface::Create();
			s_skyboxPlatformInterface->Init("assets/textures/skybox/skybox.jpg");

			s_framebuffer = Framebuffer::Create({
				width,
				height,
				1,
				(ui8)FramebufferUsage::ColorBuffer | (ui8)FramebufferUsage::DepthBuffer | (ui8)FramebufferUsage::StencilBuffer
			});

			s_shadowMapTex = Texture2D::Create("ShadowMapTex",true);
			ResourceManagement::ResourceManager::AddResource(s_shadowMapTex);

			s_shadowFramebuffer = Framebuffer::Create({
				1024,
				1024,
				1,
				(ui8)FramebufferUsage::DepthBuffer,
				false,
				s_shadowMapTex
			});

			s_depthMapShader = ResourceManagement::ResourceManager::AddResource<Shader>("assets/shaders/depthMap.glsl",ResourceManagement::ResourceType::Shader);

			s_depthMapSkinnedShader = ResourceManagement::ResourceManager::AddResource<Shader>("assets/shaders/depthMapSkinned.glsl", ResourceManagement::ResourceType::Shader);

			s_dynamicMeshes = DynamicArray<RenderMeshData>(AllocationType::FreeList_Renderer, 100);
			s_shadowCastingMeshes = DynamicArray<RenderMeshData>(AllocationType::FreeList_Renderer, 100);
			s_disposableVertexArrays = DynamicArray<VertexArray*>(AllocationType::FreeList_Renderer, 100);
			s_geometryToBatch = HashTable<Material*, GeometryData*>(AllocationType::FreeList_Renderer, 10);
			s_scheduledDebugDrawCommands = DynamicArray<std::function<void()>>(AllocationType::FreeList_Renderer, 100);			

			s_debugDrawRender = DebugDrawRenderProvider::Create();
			s_debugDrawRender->Init();
			s_debugDrawRender->SetScreenDimensions((float)width, (float)height);

			dd::initialize(s_debugDrawRender);
		}

		void Renderer::Stop()
		{
			MemoryManager::Delete(s_rendererPlatformInterface);
			MemoryManager::Delete(s_skyboxPlatformInterface);

			Framebuffer::Destroy(s_framebuffer);
			Framebuffer::Destroy(s_shadowFramebuffer);

			s_debugDrawRender->Stop();
			DebugDrawRenderProvider::Destroy(s_debugDrawRender);
		}

		void Renderer::OnWindowResize(ui32 width, ui32 height)
		{
			s_rendererPlatformInterface->SetViewport(0, 0, width, height);			
		}

		void Renderer::BeginScene()
		{
			BL_PROFILE_FUNCTION();
			
			s_sceneData.viewProjectionMatrix = ECS::SingletonComponents::ECSProjectionCameraSingleton::GetMainCameraViewProjectionMatrix();
			s_sceneData.cameraPosition = ECS::SingletonComponents::ECSProjectionCameraSingleton::GetMainCameraPosition();			

			s_renderStats.drawCalls = 0;
			s_renderStats.vertices = 0;		

		}

		void Renderer::DrawScene()
		{
			BL_PROFILE_FUNCTION();

			DrawShadowMap();

			s_framebuffer->Bind();
			s_rendererPlatformInterface->SetClearColor({ 0.3f, 0.3f, 0.8f, 1.0f });

			DrawBatchedMeshes();
			DrawDynamicMeshes();
			s_skyboxPlatformInterface->Draw();

			DrawDebugCommands();
		}

		void Renderer::EndScene()
		{
			BL_PROFILE_FUNCTION();
			for (ui32 i = 0; i < s_disposableVertexArrays.Size(); ++i)
			{
				VertexArray::Destroy(s_disposableVertexArrays[i]);
			}

			s_disposableVertexArrays.ClearNoDestructor();

			s_dynamicMeshes.Clear();
			s_shadowCastingMeshes.Clear();
			s_geometryToBatch.Clear();
			s_framebuffer->Unbind();
		}

		void Renderer::Draw(const VertexArray* vertexArray, Shader* shader, bool receiveShadows, const glm::mat4& transform)
		{
			BL_PROFILE_FUNCTION();

			shader->Bind();

			ui32 useShadowMapTex = 0;			
			if (receiveShadows)
			{
				useShadowMapTex = 1;
				ui32 shadowMapSlot = (ui32)Material::TextureSlots::ShadowMap;
				shader->SetUniform(UNIFORM_SHADOWMAP_TEX, ShaderDataType::Int, &shadowMapSlot);
				s_shadowMapTex->Bind(shadowMapSlot);
			}

			shader->SetUniform(UNIFORM_LIGHT_SPACE_TRANSFORM, ShaderDataType::Mat4, &(s_lightViewProjection));
			shader->SetUniform(UNIFORM_USE_SHADOWMAP_TEX, ShaderDataType::Int, &useShadowMapTex);
			shader->SetUniform(UNIFORM_MODEL_SPACE_TRANSFORM, ShaderDataType::Mat4, &(transform[0][0]));
			shader->SetUniform(UNIFORM_VIEW_PROJECTION, ShaderDataType::Mat4, &(s_sceneData.viewProjectionMatrix));
			shader->SetUniform(UNIFORM_CAMERA_POS, ShaderDataType::Float3, &(s_sceneData.cameraPosition));

			shader->SetUniform(UNIFORM_DIR_LIGHT_DIRECTION, ShaderDataType::Float3, &(ECS::SingletonComponents::ECSLightSingleton::GetDirectionaLightDirection()));
			shader->SetUniform(UNIFORM_DIR_LIGHT_AMBIENT,ShaderDataType::Float3, &(ECS::SingletonComponents::ECSLightSingleton::GetDirectionaLightAmbientColor()));
			shader->SetUniform(UNIFORM_DIR_LIGHT_DIFFUSE, ShaderDataType::Float3, &(ECS::SingletonComponents::ECSLightSingleton::GetDirectionaLightDiffuseColor()));
			shader->SetUniform(UNIFORM_DIR_LIGHT_SPECULAR, ShaderDataType::Float3, &(ECS::SingletonComponents::ECSLightSingleton::GetDirectionaLightSpecularColor()));

			s_rendererPlatformInterface->DrawVertexArray(vertexArray);

			s_renderStats.drawCalls++;
			s_renderStats.vertices += vertexArray->GetIndexBuffer()->GetCount();
		}

		void Renderer::DrawShadowMap()
		{

			BL_PROFILE_FUNCTION();

			if (s_shadowCastingMeshes.Size() == 0)
				return;

			const float shadowDistance = 200.0f;	

			glm::vec3 lookAtEye = s_sceneData.cameraPosition;
			lookAtEye.y = shadowDistance * 0.5f;

			const glm::vec3 lookAtCenter = lookAtEye + (ECS::SingletonComponents::ECSLightSingleton::GetDirectionaLightDirection() * glm::length(lookAtEye));

			const glm::mat4 lightView = glm::lookAt(lookAtEye, lookAtCenter, MathUtils::Vector3UnitY);
			const glm::mat4 lightProjection = glm::ortho(-shadowDistance, shadowDistance, -shadowDistance, shadowDistance, 0.0f, shadowDistance * 2.0f);

			s_lightViewProjection = lightProjection * lightView;	

			s_shadowFramebuffer->Bind();
			s_rendererPlatformInterface->SetClearColor(glm::vec4(1.0f));
			s_rendererPlatformInterface->SetFaceCulling(RendererPlatformInterface::FaceCulling::Front);

			for (ui32 i = 0; i < s_shadowCastingMeshes.Size(); ++i)
			{
				VertexArray* vertexArray = VertexArray::Create();

				IndexBuffer* indexBuffer = IndexBuffer::Create(&s_shadowCastingMeshes[i].meshComponent->indices[0], s_shadowCastingMeshes[i].meshComponent->indices.Size());
				VertexBuffer* vertexBuffer = VertexBuffer::Create(s_shadowCastingMeshes[i].meshComponent->vertices[0].GetFirstElement(), (ui32)(s_shadowCastingMeshes[i].meshComponent->vertices.Size() * sizeof(Vertex)));

				vertexBuffer->SetLayout({
					{ ShaderDataType::Float3, "vertex_position"},
					{ ShaderDataType::Float3, "vertex_color"},
					{ ShaderDataType::Float3, "vertex_normal"},
					{ ShaderDataType::Float3, "vertex_tangent"},
					{ ShaderDataType::Float2, "vertex_texcoord"}
					});

				vertexArray->AddIndexBuffer(indexBuffer);
				vertexArray->AddVertexBuffer(vertexBuffer);

				if (s_shadowCastingMeshes[i].skeletonComponent != nullptr)
				{
					VertexBuffer* boneDataVertexBuffer = VertexBuffer::Create(s_shadowCastingMeshes[i].skeletonComponent->boneData[0].GetFirstElement(), 
						(ui32)(s_shadowCastingMeshes[i].skeletonComponent->boneData.Size() * sizeof(VertexBone)));

					boneDataVertexBuffer->SetLayout({
						{ ShaderDataType::Int3,		"vertex_joint_ids" },
						{ ShaderDataType::Float3,	"vertex_joint_weights" }
						});

					vertexArray->AddVertexBuffer(boneDataVertexBuffer);

					s_depthMapSkinnedShader->Bind();
					s_depthMapSkinnedShader->SetUniform(UNIFORM_LIGHT_SPACE_TRANSFORM, ShaderDataType::Mat4, &(s_lightViewProjection));
					s_depthMapSkinnedShader->SetUniform(UNIFORM_MODEL_SPACE_TRANSFORM, ShaderDataType::Mat4, &(s_shadowCastingMeshes[i].transformMatrix));

					const DynamicArray<Animation::Joint>* joints = &(s_shadowCastingMeshes[i].skeletonComponent->joints);
					
					for (ui32 j = 0; j < joints->Size(); ++j)
					{
						s_depthMapSkinnedShader->SetUniform(STRING_TO_STRINGID(("u_joints[" + std::to_string(j) + "]")), ShaderDataType::Mat4, &((*joints)[j].jointAnimationTransform));
					}

					s_rendererPlatformInterface->DrawVertexArray(vertexArray);
					s_depthMapSkinnedShader->Unbind();
				}
				else {

					s_depthMapShader->Bind();
					s_depthMapShader->SetUniform(UNIFORM_LIGHT_SPACE_TRANSFORM, ShaderDataType::Mat4, &(s_lightViewProjection));
					s_depthMapShader->SetUniform(UNIFORM_MODEL_SPACE_TRANSFORM, ShaderDataType::Mat4, &(s_shadowCastingMeshes[i].transformMatrix));

					s_rendererPlatformInterface->DrawVertexArray(vertexArray);

					s_depthMapShader->Unbind();
				}

				s_renderStats.drawCalls++;
				s_disposableVertexArrays.PushBack(vertexArray);
			}

			s_rendererPlatformInterface->SetFaceCulling(RendererPlatformInterface::FaceCulling::Back);
			s_shadowFramebuffer->Unbind();		
		}

		void Renderer::DrawDynamicMeshes()
		{
			BL_PROFILE_FUNCTION();

			for (ui32 i = 0; i < s_dynamicMeshes.Size(); ++i)
			{
				VertexArray* vertexArray = VertexArray::Create();

				IndexBuffer* indexBuffer = IndexBuffer::Create(&s_dynamicMeshes[i].meshComponent->indices[0], s_dynamicMeshes[i].meshComponent->indices.Size());
				VertexBuffer* vertexBuffer = VertexBuffer::Create(s_dynamicMeshes[i].meshComponent->vertices[0].GetFirstElement(), (ui32)(s_dynamicMeshes[i].meshComponent->vertices.Size() * sizeof(Vertex)));

				vertexBuffer->SetLayout({
					{ ShaderDataType::Float3, "vertex_position"},
					{ ShaderDataType::Float3, "vertex_color"},
					{ ShaderDataType::Float3, "vertex_normal"},
					{ ShaderDataType::Float3, "vertex_tangent"},
					{ ShaderDataType::Float2, "vertex_texcoord"}
					});

				vertexArray->AddIndexBuffer(indexBuffer);
				vertexArray->AddVertexBuffer(vertexBuffer);

				if (s_dynamicMeshes[i].skeletonComponent != nullptr)
				{
					VertexBuffer* boneDataVertexBuffer = VertexBuffer::Create(s_dynamicMeshes[i].skeletonComponent->boneData[0].GetFirstElement(),
						(ui32)(s_dynamicMeshes[i].skeletonComponent->boneData.Size() * sizeof(VertexBone)));

					boneDataVertexBuffer->SetLayout({
						{ ShaderDataType::Int3,		"vertex_joint_ids" },
						{ ShaderDataType::Float3,	"vertex_joint_weights" }
						});

					vertexArray->AddVertexBuffer(boneDataVertexBuffer);
				}

				s_dynamicMeshes[i].meshComponent->material->Bind();

				if (s_dynamicMeshes[i].skeletonComponent != nullptr) {

					const DynamicArray<Animation::Joint>* joints = &(s_dynamicMeshes[i].skeletonComponent->joints);

					for (ui32 j = 0; j < joints->Size(); ++j)
					{
						s_dynamicMeshes[i].meshComponent->material->GetShader()->SetUniform(STRING_TO_STRINGID(("u_joints[" + std::to_string(j) + "]")), ShaderDataType::Mat4, &((*joints)[j].jointAnimationTransform));
					}
				}					
				
				Draw(vertexArray, s_dynamicMeshes[i].meshComponent->material->GetShader(), s_dynamicMeshes[i].meshComponent->material->GetReceiveShadows(), s_dynamicMeshes[i].transformMatrix);

				s_dynamicMeshes[i].meshComponent->material->Unbind();

				s_disposableVertexArrays.PushBack(vertexArray);
			}
		}

		void Renderer::DrawBatchedMeshes()
		{
			BL_PROFILE_FUNCTION();

			if (s_geometryToBatch.Size() > 0)
			{
				//Draw batches
				for (HashTable<Material*, GeometryData*>::Iterator iterator = s_geometryToBatch.Begin(); iterator != s_geometryToBatch.End(); ++iterator)
				{
					VertexArray* vertexArray = VertexArray::Create();

					IndexBuffer* indexBuffer = IndexBuffer::Create(&iterator.GetValue()->indices[0], (ui32)iterator.GetValue()->indices.Size());
					VertexBuffer* vertexBuffer = VertexBuffer::Create(iterator.GetValue()->vertices[0].GetFirstElement(), (ui32)(iterator.GetValue()->vertices.Size() * sizeof(Vertex)));

					vertexBuffer->SetLayout({
						{ ShaderDataType::Float3, "vertex_position"},
						{ ShaderDataType::Float3, "vertex_color"},
						{ ShaderDataType::Float3, "vertex_normal"},
						{ ShaderDataType::Float3, "vertex_tangent"},
						{ ShaderDataType::Float2, "vertex_texcoord"}
					});

					vertexArray->AddIndexBuffer(indexBuffer);
					vertexArray->AddVertexBuffer(vertexBuffer);

					const Material* mat = iterator.GetKey();

					mat->Bind();
					Draw(vertexArray, mat->GetShader(), mat->GetReceiveShadows());
					mat->Unbind();

					s_disposableVertexArrays.PushBack(vertexArray);
				}
			}
		}

		void Renderer::DrawDebugCommands()
		{
			s_debugDrawRender->SetMvpMatrix(s_sceneData.viewProjectionMatrix);

			for (ui32 i = 0; i < s_scheduledDebugDrawCommands.Size(); ++i) 
			{
				s_scheduledDebugDrawCommands[i]();
			}

			dd::flush((int)Time::GetCurrentTimeInMilliseconds());
			s_scheduledDebugDrawCommands.ClearNoDestructor();
		}

		void Renderer::AddStaticMeshToBatch(const ECS::ECSMeshComponent* staticMeshComponent, const ECS::ECSTransformComponent* staticMeshTransform)
		{
			BL_PROFILE_FUNCTION();	

			std::lock_guard<std::mutex> frustrumCullingGuard(s_geometryToBatchMutex);

			GeometryData* batch = nullptr;		

			const ui32 numVertices = staticMeshComponent->vertices.Size();
			const glm::mat4 staticMeshTransformMatrix = staticMeshTransform->GetTransformMatrix();
			const ui32 numIndices = staticMeshComponent->indices.Size();

			if (!s_geometryToBatch.TryGet(staticMeshComponent->material, batch))
			{	
				batch = MemoryManager::New<GeometryData>("Batch", AllocationType::Linear_Frame);
				batch->vertices = DynamicArray<Vertex>(AllocationType::Linear_Frame, staticMeshComponent->vertices.Size() * 100);
				batch->indices = DynamicArray<ui32>(AllocationType::Linear_Frame, batch->vertices.Capacity() * 2);

				s_geometryToBatch.Emplace(staticMeshComponent->material, std::move(batch));
			}	

			const ui32 verticesInBatch = batch->vertices.Size();

			for (ui32 i = 0; i < numVertices; ++i)
			{
				batch->vertices.EmplaceBack(staticMeshComponent->vertices[i] * staticMeshTransformMatrix);
			}

			for (ui32 i = 0; i < numIndices; ++i)
			{
				batch->indices.EmplaceBack(staticMeshComponent->indices[i] + verticesInBatch);
			}			
		}

		void Renderer::AddShadowCastingMesh(const ECS::ECSMeshComponent* meshComponent, const ECS::ECSTransformComponent* meshTransform, const ECS::ECSSkeletonComponent* skeletonComponent)
		{
			BL_PROFILE_FUNCTION();
			std::lock_guard<std::mutex> frustrumCullingGuard(s_shadowCastingMeshesMutex);
			s_shadowCastingMeshes.EmplaceBack(RenderMeshData{ meshTransform->GetTransformMatrix(), meshComponent, skeletonComponent });
		}		

		void Renderer::DrawDebugBox(const glm::vec3& center, const glm::vec3& size, const glm::vec3& color, int durationMs, bool depthEnabled /*= true*/)
		{
			ScheduleDebugDrawCommand([center, color, size, durationMs, depthEnabled]
			{
				dd::box((float*)&center, (float*)&color, size[0], size[1], size[2], durationMs, depthEnabled);
			});
		}

		void Renderer::DrawDebugSphere(const glm::vec3& center, float radius, const glm::vec3& color, int durationMs, bool depthEnabled /*= true*/)
		{
			ScheduleDebugDrawCommand([center, radius, color, durationMs, depthEnabled]
			{
				dd::sphere((float*)&center, (float*)&color, radius, durationMs, depthEnabled);
			});
		}

		void Renderer::DrawDebugLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color, bool arrow, int durationMs, bool depthEnabled /*= true*/)
		{
			ScheduleDebugDrawCommand([from, to, color, arrow, durationMs, depthEnabled]
			{
				if (arrow) 
				{
					dd::line((float*)&from, (float*)&to, (float*)&color, durationMs, depthEnabled);
				}
				else 
				{
					dd::arrow((float*)&from, (float*)&to, (float*)&color, 1.0f, durationMs, depthEnabled);
				}				
			});
		}

		void Renderer::DrawDebugFrustrum(const glm::mat4& invClipMatrix, const glm::vec3& color, int durationMs, bool depthEnabled /*= true*/)
		{
			ScheduleDebugDrawCommand([invClipMatrix, color, durationMs, depthEnabled]
			{
				dd::frustum((float*)&invClipMatrix, (float*)&color, durationMs, depthEnabled);
			});
		}

		void Renderer::ScheduleDebugDrawCommand(std::function<void()> debugDrawCommand)
		{
			s_scheduledDebugDrawCommands.EmplaceBack(debugDrawCommand);
		}

		void Renderer::AddDynamicMesh(const ECS::ECSMeshComponent* meshComponent, const ECS::ECSTransformComponent* meshTransform, const ECS::ECSSkeletonComponent* skeletonComponent)
		{
			BL_PROFILE_FUNCTION();
			std::lock_guard<std::mutex> frustrumCullingGuard(s_dynamicMeshesToRenderMutex);
			s_dynamicMeshes.EmplaceBack(RenderMeshData{ meshTransform->GetTransformMatrix(), meshComponent, skeletonComponent });
		}
	}
}