#pragma once
#include "EditorPanel.h"

namespace BaldLion {

	namespace Editor {

		class RenderingDataPanel : public EditorPanel {

		public:
			RenderingDataPanel() : EditorPanel("Rendering Data"){}
			virtual void OnImGuiRender() override;
		};
	}
}
#pragma once
