#pragma once
#include "EditorViewportPanel.h"
#include "EditorPanel.h"

namespace BaldLion {

	namespace Editor 
	{
		class NavigationPanel : public EditorPanel {

		public:
			NavigationPanel();
			~NavigationPanel();

			void SetEditorViewportPanel(EditorViewportPanel* editorViewportPanel) 
			{
				m_editorViewportPanel = editorViewportPanel;
			}

			virtual void OnImGuiRender() override;
		private:

			EditorViewportPanel* m_editorViewportPanel;

		};		
	}
}
