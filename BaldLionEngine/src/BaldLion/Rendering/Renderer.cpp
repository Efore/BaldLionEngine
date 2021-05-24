#include "blpch.h"
#include "Renderer.h"

#include "BaldLion/Core/JobManagement/JobManager.h"
#include "BaldLion/Utils/MathUtils.h"

const ui32 maxMeshesToProcess = 5u;

namespace BaldLion
{
	namespace Rendering
	{
		Renderer::RendererStats Renderer::s_renderStats;
		Renderer::SceneData Renderer::s_sceneData;
		ShaderLibrary Renderer::s_shaderLibrary;
		TextureLibrary Renderer::s_textureLibrary;
		RendererPlatformInterface* Renderer::s_rendererPlatformInterface;
		SkyboxPlatformInterface* Renderer::s_skyboxPlatformInterface;

		DynamicArray<Mesh*> Renderer::s_registeredMeshes;
		DynamicArray<Mesh*> Renderer::s_dynamicMeshesToRender;	
		DynamicArray<Mesh*> Renderer::s_castingShadowMeshes;

		HashTable<Material*, GeometryData*> Renderer::s_geometryToBatch;
		DynamicArray<VertexArray*> Renderer::s_batchedVertexArrays;

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
		std::mutex Renderer::s_castingShadowMeshesMutex;

		void Renderer::Init(ui32 width, ui32 height)
		{
			BL_PROFILE_FUNCTION();

			MaterialLibrary::Init();
			s_textureLibrary.Init();
			s_shaderLibrary.Init();

			s_rendererPlatformInterface = RendererPlatformInterface::Create();
			s_rendererPlatformInterface->Init();

			s_skyboxPlatformInterface = SkyboxPlatformInterface::Create();
			s_skyboxPlatformInterface->Init("assets/textures/skybox/skybox.jpg");

			s_registeredMeshes = DynamicArray<Mesh*>(AllocationType::FreeList_Renderer, 500);

			s_framebuffer = Framebuffer::Create({
				width,
				height,
				1,
				(ui8)FramebufferUsage::ColorBuffer | (ui8)FramebufferUsage::DepthBuffer | (ui8)FramebufferUsage::StencilBuffer
			});

			s_shadowMapTex = Texture2D::Create("ShadowMapTex",true);

			s_shadowFramebuffer = Framebuffer::Create({
				1024,
				1024,
				1,
				(ui8)FramebufferUsage::DepthBuffer,
				false,
				s_shadowMapTex
			});

			s_depthMapShader = Shader::Create("assets/shaders/depthMap.glsl");
			s_depthMapSkinnedShader = Shader::Create("assets/shaders/depthMapSkinned.glsl");

			LightManager::Init();
		}

		void Renderer::Stop()
		{
			MemoryManager::Delete(s_rendererPlatformInterface);
			MemoryManager::Delete(s_skyboxPlatformInterface);

			MaterialLibrary::Clear();
			s_shaderLibrary.Clear();
			s_textureLibrary.Clear();
			s_registeredMeshes.Clear();

			Framebuffer::Destroy(s_framebuffer);
			Framebuffer::Destroy(s_shadowFramebuffer);
		}

		void Renderer::OnWindowResize(ui32 width, ui32 height)
		{
			s_rendererPlatformInterface->SetViewport(0, 0, width, height);			
		}

		void Renderer::BeginScene(const Camera* camera, const DirectionalLight& directionalLight)
		{
			BL_PROFILE_FUNCTION();
			
			s_sceneData.viewProjectionMatrix = camera->GetViewProjectionMatrix();
			s_sceneData.cameraPosition = camera->GetPosition();
			
			LightManager::BeginScene(directionalLight);

			s_renderStats.drawCalls = 0;
			s_renderStats.vertices = 0;
		}

		void Renderer::DrawScene(const Camera* camera)
		{
			BL_PROFILE_FUNCTION();

			CreateShadowMap();

			s_framebuffer->Bind();
			s_rendererPlatformInterface->SetClearColor({ 0.3f, 0.3f, 0.8f, 1.0f });

			RenderStatictGeometry();

			RenderDynamicGeometry();
		}

		void Renderer::EndScene()
		{	
			for (ui32 i = 0; i < s_batchedVertexArrays.Size(); ++i)
			{
				VertexArray::Destroy(s_batchedVertexArrays[i]);
			}
			s_batchedVertexArrays.ClearNoDestructor();

			s_skyboxPlatformInterface->Draw();

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

			shader->SetUniform(UNIFORM_DIR_LIGHT_DIRECTION, ShaderDataType::Float3, &(LightManager::GetDirectionalLight().direction));
			shader->SetUniform(UNIFORM_DIR_LIGHT_AMBIENT,ShaderDataType::Float3, &(LightManager::GetDirectionalLight().ambientColor));
			shader->SetUniform(UNIFORM_DIR_LIGHT_DIFUSSE, ShaderDataType::Float3, &(LightManager::GetDirectionalLight().diffuseColor));
			shader->SetUniform(UNIFORM_DIR_LIGHT_SPECULAR, ShaderDataType::Float3, &(LightManager::GetDirectionalLight().specularColor));

			s_rendererPlatformInterface->DrawVertexArray(vertexArray);

			s_renderStats.drawCalls++;
			s_renderStats.vertices += vertexArray->GetIndexBuffer()->GetCount();
		}

		void Renderer::ProcessFrustrumCulling(const Camera* camera)
		{
			BL_PROFILE_FUNCTION();

			s_dynamicMeshesToRender = DynamicArray<Mesh*>(AllocationType::Linear_Frame, s_registeredMeshes.Size());
			s_castingShadowMeshes = DynamicArray<Mesh*>(AllocationType::Linear_Frame, s_registeredMeshes.Size());
			s_geometryToBatch = HashTable<Material*, GeometryData*>(AllocationType::Linear_Frame, s_registeredMeshes.Size());

			const ui32 numOfTasks = (ui32)(glm::ceil( (float)s_registeredMeshes.Size() / (float)maxMeshesToProcess));
			ui32 currentMeshIndex = 0;

			for (ui32 iTask = 0; iTask < numOfTasks; ++iTask)
			{
				JobManagement::Job processFrustrumCulling(("ProcessFrustrumCulling" + std::to_string(iTask)).c_str());

				const ui32 nextTaskIndex = ((currentMeshIndex + maxMeshesToProcess) > s_registeredMeshes.Size()) ? s_registeredMeshes.Size() : currentMeshIndex + maxMeshesToProcess;

				processFrustrumCulling.Task = [currentMeshIndex, nextTaskIndex, camera]
				{
					ProcessFrustrumCullingParallel(currentMeshIndex, nextTaskIndex, camera);
				};

				currentMeshIndex = nextTaskIndex;

				JobManagement::JobManager::QueueJob(processFrustrumCulling);
			}
		}

		void Renderer::ProcessFrustrumCullingParallel(ui32 initialMeshIndex, ui32 finalMeshIndex, const Camera* camera)
		{
			BL_PROFILE_FUNCTION();
			for (ui32 i = initialMeshIndex; i < finalMeshIndex; ++i)
			{
				if (camera->IsAABBVisible(s_registeredMeshes[i]->GetAABB()))
				{
					if (s_registeredMeshes[i]->GetIsStatic())
					{
						std::lock_guard<std::mutex> frustrumCullingGuard(s_geometryToBatchMutex);
						AddToBatch(s_registeredMeshes[i]);
					}
					else
					{
						std::lock_guard<std::mutex> frustrumCullingGuard(s_dynamicMeshesToRenderMutex);
						s_dynamicMeshesToRender.PushBack(s_registeredMeshes[i]);
					}					
				}

				if (s_registeredMeshes[i]->GetMaterial()->GetCastShadows())
				{
					std::lock_guard<std::mutex> frustrumCullingGuard(s_castingShadowMeshesMutex);
					s_castingShadowMeshes.PushBack(s_registeredMeshes[i]);
				}
			}
		}

		void Renderer::CreateShadowMap()
		{
			if (s_castingShadowMeshes.Size() == 0)
				return;

			float shadowDistance = 200.0f;			
			glm::mat4 lightView = glm::lookAt(LightManager::GetDirectionalLight().direction * (-shadowDistance * 0.5f), glm::vec3(0.0f), MathUtils::Vector3UnitY);
			glm::mat4 lightProjection = glm::ortho(-shadowDistance, shadowDistance, -shadowDistance, shadowDistance, 0.0f, shadowDistance * 2.0f);

			s_lightViewProjection = lightProjection * lightView;	

			s_shadowFramebuffer->Bind();
			s_rendererPlatformInterface->SetClearColor(glm::vec4(1.0f));
			s_rendererPlatformInterface->SetFaceCulling(RendererPlatformInterface::FaceCulling::Front);

			for (ui32 i = 0; i < s_castingShadowMeshes.Size(); ++i)
			{
				if (s_castingShadowMeshes[i]->GetIsStatic())
				{
					s_depthMapShader->Bind();
					s_depthMapShader->SetUniform(UNIFORM_LIGHT_SPACE_TRANSFORM, ShaderDataType::Mat4, &(s_lightViewProjection));
					s_depthMapShader->SetUniform(UNIFORM_MODEL_SPACE_TRANSFORM, ShaderDataType::Mat4, &(s_castingShadowMeshes[i]->GetWorldTransform()[0][0]));
					s_rendererPlatformInterface->DrawVertexArray(s_castingShadowMeshes[i]->GetVertexArray());

					s_depthMapShader->Unbind();
				}
				else
				{
					s_depthMapSkinnedShader->Bind();
					s_depthMapSkinnedShader->SetUniform(UNIFORM_LIGHT_SPACE_TRANSFORM, ShaderDataType::Mat4, &(s_lightViewProjection));
					s_depthMapSkinnedShader->SetUniform(UNIFORM_MODEL_SPACE_TRANSFORM, ShaderDataType::Mat4, &(s_castingShadowMeshes[i]->GetWorldTransform()[0][0]));

					DynamicArray<Animation::Joint>* joints = &(static_cast<SkinnedMesh*>(s_castingShadowMeshes[i])->GetJoints());
					
					for (ui32 i = 0; i < joints->Size(); ++i)
					{
						s_depthMapSkinnedShader->SetUniform(STRING_TO_ID(("u_joints[" + std::to_string(i) + "]")), ShaderDataType::Mat4, &((*joints)[i].jointAnimationTransform));
					}

					s_rendererPlatformInterface->DrawVertexArray(s_castingShadowMeshes[i]->GetVertexArray());
					s_depthMapSkinnedShader->Unbind();
				}
				s_renderStats.drawCalls++;
			}

			s_rendererPlatformInterface->SetFaceCulling(RendererPlatformInterface::FaceCulling::Back);
			s_shadowFramebuffer->Unbind();		
		}

		void Renderer::RenderStatictGeometry()
		{
			if (s_geometryToBatch.Size() > 0)
			{
				s_batchedVertexArrays = DynamicArray<VertexArray*>(AllocationType::FreeList_Renderer, s_geometryToBatch.Size());

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

					s_batchedVertexArrays.PushBack(vertexArray);
				}
			}
		}

		void Renderer::RenderDynamicGeometry()
		{
			for (ui32 i = 0; i < s_dynamicMeshesToRender.Size(); ++i)
			{
				s_dynamicMeshesToRender[i]->Draw();
			}
		}

		void Renderer::AddToBatch(Mesh* mesh)
		{
			BL_PROFILE_FUNCTION();		

			GeometryData* batch = nullptr;
			
			if (!s_geometryToBatch.Contains(mesh->GetMaterial()))
			{
				BL_PROFILE_SCOPE("Emplace material for batch", Optick::Category::Rendering);

				batch = MemoryManager::New<GeometryData>("Batch", AllocationType::Linear_Frame);
				batch->vertices = DynamicArray<Vertex>(AllocationType::Linear_Frame, mesh->GetVertices().Size() * 100);
				batch->indices = DynamicArray<ui32>(AllocationType::Linear_Frame, batch->vertices.Capacity() * 2);

				s_geometryToBatch.Emplace(mesh->GetMaterial(), std::move(batch));				
			}
			else
			{	
				batch = s_geometryToBatch.Get(mesh->GetMaterial());
			}

			{
				BL_PROFILE_SCOPE("Add vertices and indices", Optick::Category::Rendering);

				const ui32 verticesInBatch = batch->vertices.Size();
				const ui32 numVertices = mesh->GetVertices().Size(); 

				for (ui32 i = 0; i < numVertices; ++i)
				{
					Vertex batchedVertex = mesh->GetVertices()[i];						
					batchedVertex.position = glm::vec3(mesh->GetWorldTransform() * glm::vec4(batchedVertex.position, 1.0f));					
					batch->vertices.EmplaceBack(batchedVertex);
				}
				
				const ui32 numIndices = mesh->GetIndices().Size();
				for (ui32 i = 0; i < numIndices; ++i)
				{
					batch->indices.EmplaceBack(mesh->GetIndices()[i] + verticesInBatch);
				}
			}
		}

		void Renderer::RegisterModel(Model* model)
		{
			for (ui32 i = 0; i < model->GetSubMeshes().Size(); ++i)
			{
				if (!s_registeredMeshes.Contains(model->GetSubMeshes()[i]))
				{
					s_registeredMeshes.PushBack(model->GetSubMeshes()[i]);
				}
			}
		}

		void Renderer::UnregisterModel(Model* model)
		{
			for (ui32 i = 0; i < model->GetSubMeshes().Size(); ++i)
			{
				if (s_registeredMeshes.Contains(model->GetSubMeshes()[i]))
				{
					s_registeredMeshes.RemoveFast(model->GetSubMeshes()[i]);
				}
			}			
		}

		void Renderer::RegisterMesh(Mesh* mesh)
		{
			if (!s_registeredMeshes.Contains(mesh))
			{
				s_registeredMeshes.PushBack(mesh);
			}
		}

		void Renderer::UnregisterMesh(Mesh* mesh)
		{
			if (s_registeredMeshes.Contains(mesh))
			{
				s_registeredMeshes.RemoveFast(mesh);
			}
		}

	}
}