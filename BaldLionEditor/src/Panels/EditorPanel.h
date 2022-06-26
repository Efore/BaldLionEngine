#pragma once

namespace BaldLion {

	namespace Editor {

		class EditorPanel {

		public:
			EditorPanel(const char* panelName) {
				m_panelName = BL_STRING_TO_STRINGID(panelName);
			}

			virtual ~EditorPanel(){}
			virtual void OnImGuiRender()
			{
			}
			virtual void OnKeyPressed(int keycode){}

			protected:

			bool m_viewportFocused;		
			StringId m_panelName;
			ui32 m_panelID;
		};
	}
}
#pragma once

