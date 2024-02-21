#include "BaldLionGameLayer.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include "BaldLion/Utils/PlatformUtils.h"
#include "BaldLion/SceneManagement/Serialization/SceneSerializer.h"

namespace BaldLion
{
	namespace Game
	{	
		BaldLionGameLayer::BaldLionGameLayer()
		{
			BL_PROFILE_FUNCTION();
							
		}

		BaldLionGameLayer::~BaldLionGameLayer()
		{
		
		}

		void BaldLionGameLayer::OnActivate()
		{
			Physics::PhysicsManager::SetIsPhysicsActive(true);

			SceneManagement::SceneManager::OpenScene(SceneManagement::SceneManager::GetMainScenePathFile().c_str());

			m_ecsManager = SceneManagement::SceneManager::GetECSManager();
			
			BL_HASHTABLE_FOR(SceneManagement::SceneManager::GetECSManager()->GetEntityComponents(), it)
			{
				ECS::ECSProjectionCameraComponent* potentialCamera = it.GetValue().Write<ECS::ECSProjectionCameraComponent>(ECS::ECSComponentType::ProjectionCamera);
				if (potentialCamera != nullptr)
				{
					ECS::ECSTransformComponent* cameraTransform = it.GetValue().Write<ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform);
					ECS::SingletonSystems::CameraSystem::SetMainCamera(potentialCamera, cameraTransform);
					break;
				}
			}
		}

		void BaldLionGameLayer::OnDeactivate()
		{		
			
		}

		void BaldLionGameLayer::OnUpdate()
		{
			BL_PROFILE_FUNCTION();

			glm::mat4 cameraMatrixTransform = ECS::SingletonSystems::CameraSystem::GetMainCameraTransform()->GetTransformMatrix();
			ECS::ECSProjectionCameraComponent* viewportCamera = ECS::SingletonSystems::CameraSystem::GetMainCamera();

			const float cameraFov = viewportCamera->fov;
			const glm::mat4 viewMatrix = glm::inverse(cameraMatrixTransform);
			const glm::mat4 projectionMatrix = glm::perspective(glm::radians(cameraFov), viewportCamera->width / viewportCamera->height, viewportCamera->nearPlane, viewportCamera->farPlane);

			viewportCamera->viewProjectionMatrix = projectionMatrix * viewMatrix;

			const glm::mat4 shadowProjectionMatrix = glm::perspective(glm::radians(glm::min(179.0f, cameraFov * 2)), viewportCamera->width / viewportCamera->height, viewportCamera->nearPlane, viewportCamera->farPlane);
			viewportCamera->shadowProjectionMatrix = shadowProjectionMatrix * viewMatrix;

			SceneManagement::SceneManager::FrameStart();
			SceneManagement::SceneManager::Update(m_gameStateTimer.GetDeltaTime());	
			SceneManagement::SceneManager::FrameEnd();

			Renderer::BeginScene();			
			Renderer::DrawScene();
			Renderer::EndScene();		
		}

		void BaldLionGameLayer::OnImGuiRender()
		{
			BL_PROFILE_FUNCTION();

		}

		void BaldLionGameLayer::OnEvent(Event& e)
		{
			BaldLion::EventDispatcher dispatcher(e);
			
			dispatcher.Dispatch<BaldLion::WindowResizeEvent>(BL_BIND_FUNCTION(BaldLionGameLayer::OnWindowResizeEvent));
			dispatcher.Dispatch<BaldLion::KeyPressedEvent>(BL_BIND_FUNCTION(BaldLionGameLayer::OnKeyPressedEvent));
		}

		bool BaldLionGameLayer::OnWindowResizeEvent(WindowResizeEvent& e)
		{
			ui32 width = e.GetWidth();
			ui32 height = e.GetHeight();

			ECS::SingletonSystems::CameraSystem::SetCameraSize((float)width, (float)height);

			Renderer::OnWindowResize(width, height);

			return true;
		}
		
		bool BaldLionGameLayer::OnKeyPressedEvent(KeyPressedEvent& e)
		{
			if (e.GetRepeatCount() > 0)
				return false;			

			return false;
		}



	}
}