#pragma once
#include "BaldLion/SceneManagement/Scene.h"
#include "EditorPanel.h"

using namespace BaldLion::SceneManagement;

namespace BaldLion {

	namespace Editor {

		class ResourcesPanel : public EditorPanel {

		public:			
			ResourcesPanel() : EditorPanel("Resources"){}

			virtual void OnImGuiRender(float deltaTime) override;

		private:
			void AddResource();
		};
	}
}
