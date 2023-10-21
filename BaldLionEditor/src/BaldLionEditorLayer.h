#pragma once
#include <BaldLion.h>
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/EntityPropertiesPanel.h"
#include "Panels/EditorViewportPanel.h"
#include "Panels/MemoryAllocationPanel.h"
#include "Panels/RenderingDataPanel.h"
#include "Panels/ResourcesPanel.h"
#include "Panels/AnimatorPanel.h"
#include "Panels/NavigationPanel.h"
#include "../../BaldLionGame/src/BaldLionGameLayer.h"

namespace BaldLion
{
	namespace Editor
	{
		class BaldLionEditorLayer : public GameStateLayer
		{

		public:
			BaldLionEditorLayer();
			virtual ~BaldLionEditorLayer();

			virtual void OnActivate() override;
			virtual void OnDeactivate() override;

			virtual void OnUpdate() override;
			virtual void OnImGuiRender() override;
			virtual void OnEvent(Event& e) override;

		private:
			void SetupEditorCamera();
			void RenderDockSpace();
			bool OnWindowResizeEvent(WindowResizeEvent& e);
			bool OnKeyPressedEvent(KeyPressedEvent& e);

			void OpenScene();
			void SaveScene();
			void SaveSceneAs();
			void NewScene();

			void StartGame();
			void StopGame();

			void HandleInput();
			void MoveViewportCamera();
			void CalculateCameraMovement(const float cameraMovementSpeed, const glm::mat4& cameraTransform, glm::vec3& cameraMovement);
			void CalculateCameraRotation(const float cameraRotationSpeed, float& prevX, float& prevY, float& cameraYaw, float& cameraPitch);
			
		private:

			ECS::ECSManager* m_ecsManager = nullptr;

			//Panels
			SceneHierarchyPanel m_sceneHierarchyPanel;
			EntityPropertiesPanel m_entityPropertiesPanel;
			EditorViewportPanel m_editorViewportPanel;
			RenderingDataPanel m_renderingDataPanel;
			MemoryAllocationPanel m_memoryAllocationPanel;
			ResourcesPanel m_resourcesPanel;
			AnimatorPanel m_animatorPanel;
			NavigationPanel m_navigationPanel;

			ECS::ECSProjectionCameraComponent* m_viewportCamera;
			ECS::ECSTransformComponent* m_viewportCameraTransform;
			BaldLion::Game::BaldLionGameLayer* m_baldLionGameLayer;

			float m_cameraMovementSpeed;
			float m_cameraRotationSpeed;
			float m_prevX;
			float m_prevY;
			float m_cameraYaw;
			float m_cameraPitch;
			glm::mat4 m_cameraProjection;
			glm::mat4 m_cameraView;

			bool m_isActive = false;
		};
	}
}