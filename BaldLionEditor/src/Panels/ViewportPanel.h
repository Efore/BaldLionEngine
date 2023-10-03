#pragma once

#include "SceneHierarchyPanel.h"

namespace BaldLion {

	namespace Editor {

		class ViewportPanel : public EditorPanel {

		public:
			ViewportPanel() = default;
			ViewportPanel(SceneHierarchyPanel* sceneHierarchyPanel, const char* viewportID) : EditorPanel(viewportID), m_sceneHierarchyPanel(sceneHierarchyPanel) {}

			void SetHierarchyPanel(SceneHierarchyPanel* sceneHierarchyPanel) { m_sceneHierarchyPanel = sceneHierarchyPanel; }


		protected:

			glm::mat4 m_staticSelectedLastTransform;

			SceneHierarchyPanel* m_sceneHierarchyPanel;

		};
	}
}
#pragma once
