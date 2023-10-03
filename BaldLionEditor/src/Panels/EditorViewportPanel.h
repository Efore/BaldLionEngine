#pragma once
#include "ViewportPanel.h"

namespace BaldLion {

	namespace Editor {

		class EditorViewportPanel : public ViewportPanel {

		public:
			EditorViewportPanel();
			~EditorViewportPanel();

			virtual void OnImGuiRender() override;

			const glm::vec2& GetViewportSize() const { return m_viewportSize; }

			bool GetViewportIsFocused() const { return m_viewportFocused; }

		private:
			
			void HandleInput();
			void CheckSelectEntity();

		private:

			glm::vec2 m_viewportSize;
			bool m_viewportFocused;
			bool m_isManipulatingGizmo;
			ui32 m_imGuizmoOperation;
		};
	}
}
#pragma once
