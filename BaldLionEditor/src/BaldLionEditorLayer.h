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

namespace BaldLion
{
	namespace Editor
	{
		class BaldLionEditorLayer : public GameStateLayer
		{

		public:
			BaldLionEditorLayer();
			virtual ~BaldLionEditorLayer();

			virtual void OnAttach() override;
			virtual void OnDetach() override;

			virtual void OnUpdate() override;
			virtual void OnImGuiRender() override;
			virtual void OnEvent(Event& e) override;

			void SetupEditorCamera();

			ECS::ECSProjectionCameraComponent* GetViewportCamera() const { return m_viewportCamera; }
			ECS::ECSTransformComponent* GetViewportCameraTransform() const { return m_viewportCameraTransform; }

		private:
			void RenderDockSpace();
			bool OnWindowResizeEvent(WindowResizeEvent& e);
			bool OnKeyPressedEvent(KeyPressedEvent& e);

			void OpenScene();
			void SaveScene();
			void SaveSceneAs();
			void NewScene();

			void HandleInput();
			void MoveViewportCamera();
			void CalculateCameraMovement(const float cameraMovementSpeed, const glm::mat4& cameraTransform, glm::vec3& cameraMovement);
			void CalculateCameraRotation(const float cameraRotationSpeed, float& prevX, float& prevY, float& cameraYaw, float& cameraPitch);
			
		private:

			ECS::ECSManager* m_ecsManager;

			//Panels
			SceneHierarchyPanel m_sceneHierarchyPanel;
			EntityPropertiesPanel m_entityPropertiesPanel;
			EditorViewportPanel m_editorViewportPanel;
			RenderingDataPanel m_renderingDataPanel;
			MemoryAllocationPanel m_memoryAllocationPanel;
			ResourcesPanel m_resourcesPanel;
			AnimatorPanel m_animatorPanel;
			NavigationPanel m_navigationPanel;

			std::string m_currentScenePathFile;

			ECS::ECSProjectionCameraComponent* m_viewportCamera;
			ECS::ECSTransformComponent* m_viewportCameraTransform;

			float m_cameraMovementSpeed;
			float m_cameraRotationSpeed;
			float m_prevX;
			float m_prevY;
			float m_cameraYaw;
			float m_cameraPitch;
			glm::mat4 m_cameraProjection;
			glm::mat4 m_cameraView;
		};
	}
}