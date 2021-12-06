#pragma once
#include "ViewportPanel.h"
#include <glm/glm.hpp>

namespace BaldLion {

	namespace Editor {

		class EditorViewportPanel : public ViewportPanel {

		public:
			EditorViewportPanel();
			EditorViewportPanel(SceneHierarchyPanel* sceneHierarchyPanel);

			virtual void OnImGuiRender() override;			

		private:

			glm::vec2 m_viewportSize;
			bool m_viewPortFocused;
			ui32 m_imGuizmoOperation;

		};
	}
}
#pragma once
