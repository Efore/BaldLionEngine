#include "blpch.h"
#include "Renderer.h"

#include "BaldLion/Utils/MathUtils.h"

#include "BaldLion/ECS/SingletonSystems/CameraSystem.h"
#include "BaldLion/ECS/SingletonSystems/LightningSystem.h"
#include "BaldLion/ECS/ECSComponentLookUp.h"
#include "BaldLion/SceneManagement/SceneManager.h"

#include "BaldLion/ResourceManagement/ResourceManager.h"


#include "DebugDrawRenderInterface.h"
#include "DebugDrawRenderProvider.h"

namespace BaldLion
{
	namespace Rendering
	{
		Renderer::RendererStats Renderer::s_renderStats;
		Renderer::SceneData Renderer::s_sceneData;
		RendererPlatformInterface* Renderer::s_rendererPlatformInterface;
		SkyboxPlatformInterface* Renderer::s_skyboxPlatformInterface;
		
		DebugDrawRenderInterface* Renderer::s_debugDrawRender;

		Framebuffer* Renderer::s_framebuffer;

		//Shadows
		Framebuffer* Renderer::s_shadowFramebuffer;
		Texture2D* Renderer::s_shadowMapTex;

		Shader* Renderer::s_depthMapShader;
		Shader* Renderer::s_depthMapSkinnedShader;

		glm::mat4 Renderer::s_lightViewProjection;

		DynamicArray<ECS::ECSComponentLookUp*> Renderer::s_renderingComponentLookUps;

		bool Renderer::s_refreshComponentLookUps = true;

		ECS::ECSSignature Renderer::s_renderingEcsSignature;

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

			s_shadowMapTex = Texture2D::Create("ShadowMapTex", true);
			s_shadowMapTex->SetWrapMode(WrapMode::ClampToEdge, WrapMode::ClampToEdge);

			ResourceManagement::ResourceManager::AddResource(s_shadowMapTex);

			s_shadowFramebuffer = Framebuffer::Create({
				4096,
				4096,
				1,
				(ui8)FramebufferUsage::DepthBuffer,
				false,
				s_shadowMapTex
			});

			s_renderingComponentLookUps = DynamicArray<ECS::ECSComponentLookUp*>(AllocationType::FreeList_Renderer, 1024);
			s_renderingEcsSignature = ECS::GenerateSignature(2, ECS::ECSComponentType::Mesh, ECS::ECSComponentType::Transform);

			s_depthMapShader = ResourceManagement::ResourceManager::AddResource<Shader>("assets/editorAssets/shaders/depthMap.glsl",ResourceManagement::ResourceType::Shader);
			s_depthMapSkinnedShader = ResourceManagement::ResourceManager::AddResource<Shader>("assets/editorAssets/shaders/depthMapSkinned.glsl", ResourceManagement::ResourceType::Shader);

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

		void Renderer::OnWindowResized(ui32 width, ui32 height)
		{
			s_rendererPlatformInterface->SetViewport(0, 0, width, height);			
			s_framebuffer->Resize(width, height);
			s_shadowFramebuffer->Resize(width, height);
		}

		void Renderer::BeginScene()
		{
			BL_PROFILE_FUNCTION();
			
			s_sceneData.viewProjectionMatrix = ECS::SingletonSystems::CameraSystem::GetMainCameraViewProjectionMatrix();
			s_sceneData.cameraPosition = ECS::SingletonSystems::CameraSystem::GetMainCameraPosition();			

			s_renderStats.drawCalls = 0;
			s_renderStats.vertices = 0;		

			if (s_refreshComponentLookUps)
			{
				RefreshComponentLookUps();
				s_refreshComponentLookUps = false;
			}
		}

		void Renderer::DrawScene()
		{
			BL_PROFILE_FUNCTION();

			DrawShadowMap();

			s_framebuffer->Bind();
			s_rendererPlatformInterface->SetClearColor({ 0.3f, 0.3f, 0.8f, 1.0f });

			DrawMeshes();
			s_skyboxPlatformInterface->Draw();

			DrawDebugCommands();
		}

		void Renderer::EndScene()
		{
			BL_PROFILE_FUNCTION();			
			s_framebuffer->Unbind();
		}

		void Renderer::ForceRefreshComponentLookUps()
		{
			s_refreshComponentLookUps = true;
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

			const glm::vec3 lightDirection = MathUtils::GetTransformForwardDirection(ECS::SingletonSystems::LightningSystem::GetDirectionaLightTransform()->GetTransformMatrix());
			shader->SetUniform(UNIFORM_DIR_LIGHT_DIRECTION, ShaderDataType::Float3, &(lightDirection));
			shader->SetUniform(UNIFORM_DIR_LIGHT_AMBIENT,ShaderDataType::Float3, &(ECS::SingletonSystems::LightningSystem::GetDirectionaLightAmbientColor()));
			shader->SetUniform(UNIFORM_DIR_LIGHT_DIFFUSE, ShaderDataType::Float3, &(ECS::SingletonSystems::LightningSystem::GetDirectionaLightDiffuseColor()));
			shader->SetUniform(UNIFORM_DIR_LIGHT_SPECULAR, ShaderDataType::Float3, &(ECS::SingletonSystems::LightningSystem::GetDirectionaLightSpecularColor()));
			shader->SetUniform(UNIFORM_SHADOW_DISTANCE, ShaderDataType::Float, &(ECS::SingletonSystems::LightningSystem::GetShadowDistance()));
			shader->SetUniform(UNIFORM_SHADOW_TRANSITION_DISTANCE, ShaderDataType::Float, &(ECS::SingletonSystems::LightningSystem::GetShadowTransitionDistance()));

			s_rendererPlatformInterface->DrawVertexArray(vertexArray);

			s_renderStats.drawCalls++;
			s_renderStats.vertices += vertexArray->GetIndexBuffer()->GetCount();
		}

		void Renderer::DrawShadowMap()
		{
			BL_PROFILE_FUNCTION();
			
			float shadowDistance = ECS::SingletonSystems::LightningSystem::GetShadowDistance();
						
			const glm::mat4 cameraMatrixTransform = ECS::SingletonSystems::CameraSystem::GetMainCameraTransform()->GetTransformMatrix();
			const glm::vec3 lightDirection = MathUtils::GetTransformForwardDirection(ECS::SingletonSystems::LightningSystem::GetDirectionaLightTransform()->GetTransformMatrix());

			const glm::mat4 lightView = glm::lookAt(ECS::SingletonSystems::CameraSystem::GetMainCameraTransform()->position - (glm::normalize(lightDirection) * shadowDistance * 0.5f), ECS::SingletonSystems::CameraSystem::GetMainCameraTransform()->position, MathUtils::Vector3UnitY);
			const glm::mat4 lightProjection = glm::ortho(-shadowDistance, shadowDistance, -shadowDistance, shadowDistance, 0.0f, shadowDistance * 2);
			s_lightViewProjection = lightProjection * lightView;	

			s_shadowFramebuffer->Bind();
			s_rendererPlatformInterface->SetClearColor(glm::vec4(1.0f));
			s_rendererPlatformInterface->SetFaceCulling(RendererPlatformInterface::FaceCulling::Front);			

			BL_DYNAMICARRAY_FOR(i, s_renderingComponentLookUps, 0)
			{
				ECS::ECSComponentLookUp* componentLookUp = s_renderingComponentLookUps[i];
				const ECS::ECSMeshComponent* meshComponent = componentLookUp->Read<ECS::ECSMeshComponent>(ECS::ECSComponentType::Mesh);

				if (!meshComponent->isShadowVisible)
				{
					continue;
				}

				const ECS::ECSTransformComponent* meshTransform = componentLookUp->Read<ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform);
				const ECS::ECSSkeletonComponent* skeletonComponent = componentLookUp->Read<ECS::ECSSkeletonComponent>(ECS::ECSComponentType::Skeleton);

				const glm::mat4 meshTransformMatrix = meshTransform->GetTransformMatrix();

				meshComponent->material->Bind();

				if (skeletonComponent != nullptr) {

					s_depthMapSkinnedShader->Bind();

					const Animation::AnimationJoint* joints = (skeletonComponent->joints);

					for (ui32 j = 0; j < (ui32)JointType::Count; ++j)
					{
						s_depthMapSkinnedShader->SetUniform(BL_STRING_TO_STRINGID(("u_joints[" + std::to_string(j) + "]")), ShaderDataType::Mat4, &(joints[j].jointModelSpaceTransform));
					}

					s_depthMapSkinnedShader->SetUniform(UNIFORM_LIGHT_SPACE_TRANSFORM, ShaderDataType::Mat4, &(s_lightViewProjection));
					s_depthMapSkinnedShader->SetUniform(UNIFORM_MODEL_SPACE_TRANSFORM, ShaderDataType::Mat4, &(meshTransformMatrix));

					s_rendererPlatformInterface->DrawVertexArray(meshComponent->vertexArray);

					s_depthMapSkinnedShader->Unbind();
				}
				else
				{
					s_depthMapShader->Bind();
					s_depthMapShader->SetUniform(UNIFORM_LIGHT_SPACE_TRANSFORM, ShaderDataType::Mat4, &(s_lightViewProjection));
					s_depthMapShader->SetUniform(UNIFORM_MODEL_SPACE_TRANSFORM, ShaderDataType::Mat4, &(meshTransformMatrix));

					s_rendererPlatformInterface->DrawVertexArray(meshComponent->vertexArray);

					s_depthMapShader->Unbind();
				}
	

				s_renderStats.drawCalls++;
				s_renderStats.vertices += meshComponent->vertexArray->GetIndexBuffer()->GetCount();

				meshComponent->material->Unbind();
			}

			s_rendererPlatformInterface->SetFaceCulling(RendererPlatformInterface::FaceCulling::Back);
			s_shadowFramebuffer->Unbind();		
		}

		void Renderer::DrawMeshes()
		{
			BL_PROFILE_FUNCTION();

			BL_DYNAMICARRAY_FOR(i, s_renderingComponentLookUps, 0)			
			{		
				ECS::ECSComponentLookUp* componentLookUp = s_renderingComponentLookUps[i];
				const ECS::ECSMeshComponent* meshComponent = componentLookUp->Read<ECS::ECSMeshComponent>(ECS::ECSComponentType::Mesh);

				if (!meshComponent->isVisible)
				{
					continue;
				}

				const ECS::ECSTransformComponent* meshTransform = componentLookUp->Read<ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform);
				const ECS::ECSSkeletonComponent* skeletonComponent = componentLookUp->Read<ECS::ECSSkeletonComponent>(ECS::ECSComponentType::Skeleton);

				meshComponent->material->Bind();

				if (skeletonComponent != nullptr) {

					const Animation::AnimationJoint* joints = (skeletonComponent->joints);

					for (ui32 j = 0; j < (ui32)JointType::Count; ++j)
					{
						meshComponent->material->GetShader()->SetUniform(BL_STRING_TO_STRINGID(("u_joints[" + std::to_string(j) + "]")), ShaderDataType::Mat4, &(joints[j].jointModelSpaceTransform));
					}
				}					
				
				Draw(meshComponent->vertexArray, meshComponent->material->GetShader(), meshComponent->material->GetReceiveShadows(), meshTransform->GetTransformMatrix());

				meshComponent->material->Unbind();
			}
		}		

		void Renderer::DrawDebugCommands()
		{
			s_debugDrawRender->SetMvpMatrix(s_sceneData.viewProjectionMatrix);
			dd::flush(Time::GetCurrentTimeMs());
		}

		void Renderer::RefreshComponentLookUps()
		{
			s_renderingComponentLookUps.Clear();

			BL_HASHMAP_FOR(SceneManagement::SceneManager::GetECSManager()->GetEntitySignatures(), iterator)
			{
				if ((iterator.GetValue() & s_renderingEcsSignature) == s_renderingEcsSignature)
				{
					ECS::ECSComponentLookUp* componentLookUp = &(SceneManagement::SceneManager::GetECSManager()->GetEntityComponents().Get(iterator.GetKey()));
					s_renderingComponentLookUps.PushBack(componentLookUp);
				}
			}
		}		

		void Renderer::DrawDebugBox(const glm::vec3& center, const glm::vec3& size, const glm::mat4& transformMatrix, const glm::vec3& color, int durationMs, bool depthEnabled /*= true*/)
		{			
			dd::box((float*)&center, (float*)&transformMatrix, (float*)&color, size[0], size[1], size[2], durationMs, depthEnabled);			
		}

		void Renderer::DrawDebugCircle(const glm::vec3& center, const glm::vec3& normal, float radius, ui32 numSteps, const glm::vec3& color, int durationMs /*= 0*/, bool depthEnabled /*= true*/)
		{
			dd::circle((float*)&center, (float*)&normal, (float*)&color, radius, (float)numSteps, durationMs, depthEnabled);
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