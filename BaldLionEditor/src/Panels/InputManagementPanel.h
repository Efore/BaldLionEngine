#pragma once
#include "EditorPanel.h"

namespace BaldLion {

	namespace Editor {

		class InputManagementPanel : public EditorPanel {

		public:
			InputManagementPanel() : EditorPanel("Input Management") {}
			virtual void OnImGuiRender(float deltaTime) override;
		};
	}
}