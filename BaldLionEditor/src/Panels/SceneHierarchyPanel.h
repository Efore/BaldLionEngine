#pragma once
#include "BaldLion/SceneManagement/Scene.h"

using namespace BaldLion::SceneManagement;

namespace BaldLion {

	namespace Editor {

		class SceneHierarchyPanel {

		public:
			SceneHierarchyPanel() = default;
			SceneHierarchyPanel(Scene* sceneContext);
			void SetSceneContext(Scene* sceneContext);

			void OnImGuiRender();
		private:

			void DrawEntityElement(const ECS::ECSEntity& entity, bool firstCall);

		private:
			
			Scene* m_sceneContext;
			ECS::ECSEntityID m_selectedEntityID;
		};
	}
}
