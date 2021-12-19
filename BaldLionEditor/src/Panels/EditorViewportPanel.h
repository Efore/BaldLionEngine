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

		private:

			glm::vec2 m_viewportSize;
			bool m_viewportFocused;
			ui32 m_imGuizmoOperation;

		};
	}
}
#pragma once
