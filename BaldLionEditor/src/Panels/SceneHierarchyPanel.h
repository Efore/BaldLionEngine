#pragma once
#include "BaldLion/SceneManagement/Scene.h"

using namespace BaldLion::SceneManagement;

namespace BaldLion {

	namespace Editor {

		class SceneHierarchyPanel {

		public:
			SceneHierarchyPanel() = default;
			SceneHierarchyPanel(Scene* sceneContext);

			const Scene* GetSceneContext() const { return m_sceneContext; }
			Scene* GetSceneContext() { return m_sceneContext; }

			void SetSceneContext(Scene* sceneContext);

			const ECS::ECSEntityID GetSelectedEntityID() const { return m_selectedEntityID; }
			void OnImGuiRender();

		private:

			void DrawEntityElement(const ECS::ECSEntity& entity, bool firstCall);

		private:
			
			Scene* m_sceneContext;
			ECS::ECSEntityID m_selectedEntityID;
		};
	}
}