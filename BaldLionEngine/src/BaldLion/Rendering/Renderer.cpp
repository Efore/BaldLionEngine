#include "blpch.h"
#include "Renderer.h"

#include "BaldLion/Core/JobManagement/JobManager.h"
#include "BaldLion/Utils/MathUtils.h"

#include "BaldLion/ECS/ComponentsSingleton/ECSProjectionCameraSingleton.h"
#include "BaldLion/ECS/ComponentsSingleton/ECSLightSingleton.h"

#include "BaldLion/ResourceManagement/ResourceManager.h"

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

		Framebuffer* Renderer::s_framebuffer;

		//Shadows
		Framebuffer* Renderer::s_shadowFramebuffer;
		Texture2D* Renderer::s_shadowMapTex;

		Shader* Renderer::s_depthMapShader;
		Shader* Renderer::s_depthMapSkinnedShader;

		glm::mat4 Renderer::s_lightViewProjection;

		void Renderer::Init(ui32 width, ui32 height)
		{
			BL_PROFILE_FUNCTION();

			s_rendererPlatformInterface = RendererPlatformInterface::Create();
			s_rendererPlatformInterface->Init();

			s_skyboxPlatformInterface = SkyboxPlatformInterface::Create();
			s_skyboxPlatformInterface->Init("assets/editorAssets/textures/skybox/skybox.jpg");

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

			s_depthMapShader = ResourceManagement::ResourceManager::AddResource<Shader>("assets/editorAssets/shaders/depthMap.glsl",ResourceManagement::ResourceType::Shader);

			s_depthMapSkinnedShader = ResourceManagement::ResourceManager::AddResource<Shader>("assets/editorAssets/shaders/depthMapSkinned.glsl", ResourceManagement::ResourceType::Shader);

			s_dynamicMeshes = DynamicArray<RenderMeshData>(AllocationType::FreeList_Renderer, 100);
			s_shadowCastingMeshes = DynamicArray<RenderMeshData>(AllocationType::FreeList_Renderer, 100);
			s_disposableVertexArrays = DynamicArray<VertexArray*>(AllocationType::FreeList_Renderer, 100);
			s_geometryToBatch = HashTable<Material*, GeometryData*>(AllocationType::FreeList_Renderer, 100);		

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
			BL_DYNAMICARRAY_FOR(i, s_disposableVertexArrays, 0)			
			{
				VertexArray::Destroy(s_disposableVertexArrays[i]);
			}

			s_disposableVertexArrays.Clear();

			s_dynamicMeshes.Clear();
			s_shadowCastingMeshes.Clear();			

			BL_HASHTABLE_FOR(s_geometryToBatch, it)
			{
				it.GetValue()->ClearGeometryData();
			}

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

			BL_DYNAMICARRAY_FOR(i, s_shadowCastingMeshes, 0)			
			{
				VertexArray* vertexArray = VertexArray::Create();

				IndexBuffer* indexBuffer = IndexBuffer::Create(&s_shadowCastingMeshes[i].meshComponent->indices[0], s_shadowCastingMeshes[i].meshComponent->indices.Size());
				VertexBuffer* vertexBuffer = VertexBuffer::Create(s_shadowCastingMeshes[i].meshComponent->vertices[0].GetFirstElement(), (ui32)(s_shadowCastingMeshes[i].meshComponent->vertices.Size() * sizeof(Vertex)));

				vertexBuffer->SetLayout({
					{ ShaderDataType::Float3, "vertex_position"},					
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

					const Animation::AnimationJoint* joints = s_shadowCastingMeshes[i].skeletonComponent->joints;
					
					for (ui32 j = 0; j < (ui32)JointType::Count; ++j)					
					{
						s_depthMapSkinnedShader->SetUniform(BL_STRING_TO_STRINGID(("u_joints[" + std::to_string(j) + "]")), ShaderDataType::Mat4, &(joints[j].jointModelSpaceTransform));
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

			BL_DYNAMICARRAY_FOR(i, s_dynamicMeshes, 0)			
			{
				VertexArray* vertexArray = VertexArray::Create();

				IndexBuffer* indexBuffer = IndexBuffer::Create(&s_dynamicMeshes[i].meshComponent->indices[0], s_dynamicMeshes[i].meshComponent->indices.Size());
				VertexBuffer* vertexBuffer = VertexBuffer::Create(s_dynamicMeshes[i].meshComponent->vertices[0].GetFirstElement(), (ui32)(s_dynamicMeshes[i].meshComponent->vertices.Size() * sizeof(Vertex)));

				vertexBuffer->SetLayout({
					{ ShaderDataType::Float3, "vertex_position"},					
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

					const Animation::AnimationJoint* joints = (s_dynamicMeshes[i].skeletonComponent->joints);

					for (ui32 j = 0; j < (ui32)JointType::Count; ++j)
					{
						s_dynamicMeshes[i].meshComponent->material->GetShader()->SetUniform(BL_STRING_TO_STRINGID(("u_joints[" + std::to_string(j) + "]")), ShaderDataType::Mat4, &(joints[j].jointModelSpaceTransform));
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
				BL_HASHTABLE_FOR(s_geometryToBatch, iterator)
				{
					if(iterator.GetValue()->vertices.Size() == 0)
						continue;

					VertexArray* vertexArray = VertexArray::Create();

					IndexBuffer* indexBuffer = IndexBuffer::Create(&iterator.GetValue()->indices[0], (ui32)iterator.GetValue()->indices.Size());
					VertexBuffer* vertexBuffer = VertexBuffer::Create(iterator.GetValue()->vertices[0].GetFirstElement(), (ui32)(iterator.GetValue()->vertices.Size() * sizeof(Vertex)));

					vertexBuffer->SetLayout({
						{ ShaderDataType::Float3, "vertex_position"},
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
			dd::flush((int)Time::GetCurrentTimeInMilliseconds());
		}

		void Renderer::AddDynamicMesh(const ECS::ECSMeshComponent* meshComponent, const ECS::ECSTransformComponent* meshTransform, const ECS::ECSSkeletonComponent* skeletonComponent)
		{
			BL_PROFILE_FUNCTION();
			s_dynamicMeshes.EmplaceBack(RenderMeshData{ meshTransform->GetTransformMatrix(), meshComponent, skeletonComponent });
		}

		void Renderer::AddStaticMeshToBatch(Material* material, const DynamicArray<Vertex>& vertices, const DynamicArray<ui32>& indices)
		{
			BL_PROFILE_FUNCTION();	

			const ui32 numVertices = vertices.Size();

			if (numVertices == 0)
				return;

			const ui32 numIndices = indices.Size();

			GeometryData* batch = nullptr;			

			if (s_geometryToBatch.TryGet(material, batch))
			{
				BL_PROFILE_SCOPE("Emplace vertices and indices", Optick::Category::Rendering);
				const ui32 verticesInBatch = batch->vertices.Size();

				batch->vertices.PushBackRange(vertices);				

				for (ui32 i = 0; i < numIndices; ++i)
				{
					batch->indices.EmplaceBack(indices[i] + verticesInBatch);
				}
			}
		}

		void Renderer::AddShadowCastingMesh(const ECS::ECSMeshComponent* meshComponent, const ECS::ECSTransformComponent* meshTransform, const ECS::ECSSkeletonComponent* skeletonComponent)
		{
			if (meshComponent->vertices.Size() == 0)
				return;

			BL_PROFILE_FUNCTION();
			s_shadowCastingMeshes.EmplaceBack(RenderMeshData{ meshTransform->GetTransformMatrix(), meshComponent, skeletonComponent });
		}	

		void Renderer::RegisterMaterial(Material* material)
		{
			GeometryData* batch = nullptr;

			if (!s_geometryToBatch.TryGet(material, batch))
			{
				BL_PROFILE_SCOPE("Try get batch", Optick::Category::Rendering);
				batch = MemoryManager::New<GeometryData>("Batch", AllocationType::FreeList_Renderer);
				batch->vertices = DynamicArray<Vertex>(AllocationType::FreeList_Renderer, 100);
				batch->indices = DynamicArray<ui32>(AllocationType::FreeList_Renderer, 150);

				s_geometryToBatch.Emplace(material, std::move(batch));
			}
		}
		
		void Renderer::DrawDebugBox(const glm::vec3& center, const glm::vec3& size, const glm::mat4& transformMatrix, const glm::vec3& color, int durationMs, bool depthEnabled /*= true*/)
		{			
			dd::box((float*)&center, (float*)&transformMatrix, (float*)&color, size[0], size[1], size[2], durationMs, depthEnabled);			
		}

		void Renderer::DrawDebugSphere(const glm::vec3& center, float radius, const glm::vec3& color, int durationMs, bool depthEnabled /*= true*/)
		{			
			dd::sphere((float*)&center, (float*)&color, radius, durationMs, depthEnabled);			
		}

		void Renderer::DrawDebugCapsule(const glm::vec3& center, const glm::mat4& transformMatrix, float radius, float height, const glm::vec3& color, int durationMs /*= 0*/, bool depthEnabled /*= true*/)
		{
			const glm::vec3 transformedCenter = transformMatrix * glm::vec4(center, 1.0f);
			const glm::vec3 circleNormal = transformMatrix * glm::vec4(MathUtils::Vector3UnitY, 1.0f);			
			glm::vec3 position = transformedCenter + circleNormal * ((height * 0.5f) - radius);

			dd::sphere((float*)&position, (float*)&color, radius, durationMs, depthEnabled);			

			const ui32 numCircles = (ui32)(height / 0.5f);

			glm::vec3 circlesStartPosition = transformedCenter - circleNormal * ((height * 0.5f) - radius);

			for (ui32 i = 0; i < numCircles; ++i)
			{
				circlesStartPosition += circleNormal * (i * 0.5f);								
				dd::circle((float*)&transformedCenter, (float*)&circleNormal, (float*)&color, radius, 32, durationMs, depthEnabled);				
			}
			
			position = transformedCenter - circleNormal * ((height * 0.5f) - radius);
			dd::sphere((float*)&position, (float*)&color, radius, durationMs, depthEnabled);	
		}

		void Renderer::DrawDebugLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color, float arrowSize, int durationMs, bool depthEnabled /*= true*/)
		{			
			if (arrowSize > 0.0f)
			{
				dd::arrow((float*)&from, (float*)&to, (float*)&color, arrowSize, durationMs, depthEnabled);
			}
			else
			{
				dd::line((float*)&from, (float*)&to, (float*)&color, durationMs, depthEnabled);
			}			
		}

		void Renderer::DrawDebugFrustrum(const glm::mat4& invClipMatrix, const glm::vec3& color, int durationMs, bool depthEnabled /*= true*/)
		{			
			dd::frustum((float*)&invClipMatrix, (float*)&color, durationMs, depthEnabled);			
		}

		void Renderer::DrawDebug3DText(const std::string& text, const glm::vec3& worldPos, const glm::vec3& color, int viewportCoordX, int viewportCoordY, int viewportWidth, int viewportHeight, float scaling, int durationMs /*= 0*/)
		{	
			dd::projectedText(text.c_str(), (float*)&worldPos, (float*)&color, (float*)&s_sceneData.viewProjectionMatrix, viewportCoordX, viewportCoordY, viewportWidth, viewportHeight, scaling, durationMs);
		}

		void Renderer::DrawDebugTriangle(const glm::vec3& vertex1, const glm::vec3& vertex2, const glm::vec3& vertex3, const glm::vec3& color, float alpha, int durationMs /*= 0*/, bool depthEnabled /*= true*/)
		{
			dd::triangle((float*)&vertex1, (float*)&vertex2, (float*)&vertex3, (float*)&color, alpha, durationMs, depthEnabled);
		}

		void Renderer::DrawDebugPoint(const glm::vec3& position, const glm::vec3& color, float size /*= 15.0f*/, int durationMs /*= 0*/, bool depthEnabled /*= true*/)
		{
			dd::point((float*)&position, (float*)&color, size, durationMs, depthEnabled);
		}

	}
}