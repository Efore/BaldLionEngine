#pragma once

#include "SceneHierarchyPanel.h"

namespace BaldLion {

	namespace Editor {

		class ViewportPanel : public EditorPanel {

		public:
			ViewportPanel() = default;
			ViewportPanel(SceneHierarchyPanel* sceneHierarchyPanel, const char* viewportID) : EditorPanel(viewportID), m_sceneHierarchyPanel(sceneHierarchyPanel) {}

			virtual void SetupViewportCamera() = 0;

			void SetHierarchyPanel(SceneHierarchyPanel* sceneHierarchyPanel) { m_sceneHierarchyPanel = sceneHierarchyPanel; }

		protected:

			SceneHierarchyPanel* m_sceneHierarchyPanel;
			ECS::ECSProjectionCameraComponent* m_viewportCamera;
			ECS::ECSTransformComponent* m_viewportCameraTransform;
		};
	}
}
#pragma once
