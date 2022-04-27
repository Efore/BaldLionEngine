#pragma once
#include "BaldLion/SceneManagement/Scene.h"
#include "EditorPanel.h"

using namespace BaldLion::SceneManagement;

namespace BaldLion {

	namespace Editor {

		class ResourcesPanel : public EditorPanel {

		public:
			ResourcesPanel() = default;

			virtual void OnImGuiRender() override;
		};
	}
}
