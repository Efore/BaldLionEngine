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
					ECS::SingletonComponents::CameraSystem::SetMainCamera(potentialCamera, cameraTransform);
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

			glm::mat4 cameraMatrixTransform = ECS::SingletonComponents::CameraSystem::GetMainCameraTransform()->GetTransformMatrix();
			ECS::ECSProjectionCameraComponent* viewportCamera = ECS::SingletonComponents::CameraSystem::GetMainCamera();

			const glm::mat4 viewMatrix = glm::inverse(cameraMatrixTransform);
			const glm::mat4 projectionMatrix = glm::perspective(glm::radians(viewportCamera->fov), viewportCamera->width / viewportCamera->height, viewportCamera->nearPlane, viewportCamera->farPlane);

			viewportCamera->viewProjectionMatrix = projectionMatrix * viewMatrix;

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

			ECS::SingletonComponents::CameraSystem::SetCameraSize((float)width, (float)height);

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