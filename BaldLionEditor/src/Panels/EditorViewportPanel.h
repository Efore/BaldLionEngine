#pragma once
#include "ViewportPanel.h"
#include <glm/glm.hpp>

namespace BaldLion {

	namespace Editor {

		class EditorViewportPanel : public ViewportPanel {

		public:
			EditorViewportPanel();
			EditorViewportPanel(SceneHierarchyPanel* sceneHierarchyPanel);
			~EditorViewportPanel();

			virtual void SetupViewportCamera() override;

			virtual void OnImGuiRender() override;

		private:
			void HandleInput();
			void MoveViewportCamera();
			void CalculateCameraMovement(const float cameraMovementSpeed, const glm::mat4& cameraTransform, glm::vec3& cameraMovement);
			void CalculateCameraRotation(const float cameraRotationSpeed, float& prevX, float& prevY, float& cameraYaw, float& cameraPitch);
			void CheckSelectEntity();

		private:

			glm::vec2 m_viewportSize;
			bool m_viewportFocused;
			bool m_isManipulatingGizmo;
			ui32 m_imGuizmoOperation;

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
#pragma once
