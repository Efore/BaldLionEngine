#include "blpch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLRenderer.h"
#include "Platform/OpenGL/OpenGLSkybox.h"
#include "BaldLion/Core/JobManagement/JobManager.h"

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
		DynamicArray<Mesh*> Renderer::s_meshesToRender;

		HashTable<Material*, GeometryData*> Renderer::s_geometryToBatch;		
		DynamicArray<VertexArray*> Renderer::s_batchedVertexArrays;

		std::mutex Renderer::s_geometryToBatchMutex;
		std::mutex Renderer::s_meshesToRenderMutex;

		void Renderer::Init()
		{
			BL_PROFILE_FUNCTION();

			MaterialLibrary::Init();
			s_textureLibrary.Init();
			s_shaderLibrary.Init();

			s_rendererPlatformInterface = MemoryManager::New<OpenGLRenderer>("Renderer Platform interface", AllocationType::FreeList_Renderer);
			s_rendererPlatformInterface->Init();

			s_skyboxPlatformInterface = MemoryManager::New<OpenGLSkybox>("Skybox Platform interface", AllocationType::FreeList_Renderer);
			s_skyboxPlatformInterface->Init("assets/textures/skybox/skybox.jpg");

			s_registeredMeshes = DynamicArray<Mesh*>(AllocationType::FreeList_Renderer, 500);

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
		}

		void Renderer::OnWindowResize(ui32 width, ui32 height)
		{
			s_rendererPlatformInterface->SetViewport(0, 0, width, height);			
		}

		void Renderer::BeginScene(const Camera*  camera, const DirectionalLight& directionalLight)
		{
			BL_PROFILE_FUNCTION();
			
			s_rendererPlatformInterface->SetClearColor({ 0.3f, 0.3f, 0.8f, 1.0f });
			s_rendererPlatformInterface->Clear();

			s_sceneData.viewProjectionMatrix = camera->GetViewProjectionMatrix();
			s_sceneData.cameraPosition = camera->GetPosition();
			
			LightManager::BeginScene(directionalLight);

			s_renderStats.drawCalls = 0;
			s_renderStats.vertices = 0;
		}

		void Renderer::DrawScene(const Camera* camera)
		{
			BL_PROFILE_FUNCTION();

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
					Draw(vertexArray, mat->GetShader());
					mat->Unbind();

					s_batchedVertexArrays.PushBack(vertexArray);
				}
			}

			for (ui32 i = 0; i < s_meshesToRender.Size(); ++i)
			{
				s_meshesToRender[i]->Draw();
			}
		}

		void Renderer::EndScene()
		{	
			for (ui32 i = 0; i < s_batchedVertexArrays.Size(); ++i)
			{
				VertexArray::Destroy(s_batchedVertexArrays[i]);
			}
			s_batchedVertexArrays.ClearNoDestructor();

			s_skyboxPlatformInterface->Draw();
		}

		void Renderer::Draw(const VertexArray* vertexArray, Shader* shader, const glm::mat4& transform)
		{
			BL_PROFILE_FUNCTION();

			shader->SetUniform(STRING_TO_ID("u_worldTransformMatrix"), ShaderDataType::Mat4, &(transform[0][0]));
			shader->SetUniform(STRING_TO_ID("u_viewProjectionMatrix"), ShaderDataType::Mat4, &(s_sceneData.viewProjectionMatrix));
			shader->SetUniform(STRING_TO_ID("u_cameraPos"), ShaderDataType::Float3, &(s_sceneData.cameraPosition));

			shader->SetUniform(STRING_TO_ID("u_directionalLight.direction"), ShaderDataType::Float3, &(LightManager::GetDirectionalLight().direction));
			shader->SetUniform(STRING_TO_ID("u_directionalLight.ambientColor"),ShaderDataType::Float3, &(LightManager::GetDirectionalLight().ambientColor));
			shader->SetUniform(STRING_TO_ID("u_directionalLight.diffuseColor"), ShaderDataType::Float3, &(LightManager::GetDirectionalLight().diffuseColor));
			shader->SetUniform(STRING_TO_ID("u_directionalLight.specularColor"), ShaderDataType::Float3, &(LightManager::GetDirectionalLight().specularColor));

			vertexArray->Bind();
			s_rendererPlatformInterface->DrawIndexed(vertexArray);
			vertexArray->Unbind();			

			s_renderStats.drawCalls++;
			s_renderStats.vertices += vertexArray->GetIndexBuffer()->GetCount();
		}

		void Renderer::ProcessFrustrumCulling(const Camera* camera)
		{
			BL_PROFILE_FUNCTION();

			s_meshesToRender = DynamicArray<Mesh*>(AllocationType::Linear_Frame, s_registeredMeshes.Size());
			s_geometryToBatch = HashTable<Material*, GeometryData*>(AllocationType::Linear_Frame, s_registeredMeshes.Size());

			const ui32 numOfTasks = (ui32)(glm::ceil( (float)s_registeredMeshes.Size() / (float)maxMeshesToProcess));
			ui32 currentMeshIndex = 0;

			for (ui32 iTask = 0; iTask < numOfTasks; ++iTask)
			{
				JobManagement::Job processFrustrumCulling(("ProcessFrustrumCulling" + std::to_string(iTask)).c_str());

				const ui32 nextTaskIndex = ((currentMeshIndex + maxMeshesToProcess) > s_registeredMeshes.Size()) ? s_registeredMeshes.Size() : currentMeshIndex + maxMeshesToProcess;

				processFrustrumCulling.Task = [currentMeshIndex, nextTaskIndex, camera]
				{
					BL_PROFILE_SCOPE("ProcessFrustrumCulling", Optick::Category::Rendering);
					for (ui32 i = currentMeshIndex; i < nextTaskIndex; ++i)
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
								std::lock_guard<std::mutex> frustrumCullingGuard(s_meshesToRenderMutex);
								s_meshesToRender.PushBack(s_registeredMeshes[i]);
							}
						}						
					}
				};

				currentMeshIndex = nextTaskIndex;

				JobManagement::JobManager::QueueJob(processFrustrumCulling);
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