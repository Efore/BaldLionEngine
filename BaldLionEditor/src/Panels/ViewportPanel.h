#pragma once

#include "SceneHierarchyPanel.h"

namespace BaldLion {

	namespace Editor {

		class ViewportPanel {

		public:
			ViewportPanel() = default;
			ViewportPanel(SceneHierarchyPanel* sceneHierarchyPanel){ m_sceneHierarchyPanel = sceneHierarchyPanel; }

			virtual void OnImGuiRender() = 0;
			void SetHierarchyPanel(SceneHierarchyPanel* sceneHierarchyPanel) { m_sceneHierarchyPanel = sceneHierarchyPanel; }

		protected:

			SceneHierarchyPanel* m_sceneHierarchyPanel;
		};
	}
}
#pragma once
