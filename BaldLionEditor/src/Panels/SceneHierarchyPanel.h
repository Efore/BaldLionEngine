#pragma once
#include "BaldLion/SceneManagement/Scene.h"
#include "EditorPanel.h"

using namespace BaldLion::SceneManagement;

namespace BaldLion {

	namespace Editor {

		class SceneHierarchyPanel : public EditorPanel {

		public:
			SceneHierarchyPanel() = default;
			SceneHierarchyPanel(Scene* sceneContext);

			const Scene* GetSceneContext() const { return m_sceneContext; }
			Scene* GetSceneContext() { return m_sceneContext; }

			void SetSceneContext(Scene* sceneContext);

			const ECS::ECSEntityID GetSelectedEntityID() const { return m_selectedEntityID; }
			virtual void OnImGuiRender() override;
			virtual void OnKeyPressed(int keyCode) override;

		private:

			void DrawEntityElement(const ECS::ECSEntity& entity, bool firstCall);
			void HandleInput();

		private:
			
			Scene* m_sceneContext;
			ECS::ECSEntityID m_selectedEntityID;
		};
	}
}
