#pragma once

namespace BaldLion {

	namespace Editor {

		class EditorPanel {

		public:
			EditorPanel() = default;
			virtual ~EditorPanel(){}
			virtual void OnImGuiRender()
			{
			}
			virtual void OnKeyPressed(int keycode){}

			protected:

			bool m_viewportFocused;			
		};
	}
}
#pragma once

