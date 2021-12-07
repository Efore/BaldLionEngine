#pragma once

#include "SceneHierarchyPanel.h"

namespace BaldLion {

	namespace Editor {

		class ViewportPanel {

		public:
			ViewportPanel() = default;
			ViewportPanel(SceneHierarchyPanel* sceneHierarchyPanel){ m_sceneHierarchyPanel = sceneHierarchyPanel; }

			virtual void SetupViewportCamera() = 0;

			virtual void OnImGuiRender(TimeStep timeStep) = 0;
			void SetHierarchyPanel(SceneHierarchyPanel* sceneHierarchyPanel) { m_sceneHierarchyPanel = sceneHierarchyPanel; }

		protected:

			SceneHierarchyPanel* m_sceneHierarchyPanel;
			ECS::ECSProjectionCameraComponent* m_viewportCamera;
			ECS::ECSTransformComponent* m_viewportCameraTransform;
		};
	}
}
#pragma once
