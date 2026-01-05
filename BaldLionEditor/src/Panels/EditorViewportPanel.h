#pragma once
#include "ViewportPanel.h"

namespace BaldLion {

	namespace Editor {

		class EditorViewportPanel : public ViewportPanel {

		public:
			EditorViewportPanel();
			~EditorViewportPanel();

			virtual void OnImGuiRender(float deltaTime) override;

			const glm::vec2& GetViewportSize() const { return m_viewportSize; }

			bool GetViewportIsFocused() const { return m_viewportFocused; }

		private:
			
			void HandleInput();
			void CheckSelectEntity();

		private:

			glm::vec2 m_viewportSize = glm::vec2(0.0f);;
			bool m_viewportFocused = false;
			bool m_isManipulatingGizmo = false;
			ui32 m_imGuizmoOperation = 0;
		};
	}
}
#pragma once
