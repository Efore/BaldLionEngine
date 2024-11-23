#pragma once
#include "BaldLion/SceneManagement/Scene.h"
#include "EditorPanel.h"

using namespace BaldLion::SceneManagement;

namespace BaldLion {

	namespace Editor {

		class SceneHierarchyPanel : public EditorPanel {

		public:
			SceneHierarchyPanel();
			SceneHierarchyPanel(Scene* sceneContext);

			const ECS::ECSEntityID GetSelectedEntityID() const { return m_selectedEntityID; }
			void SetSelectedEntityID(ECS::ECSEntityID newEntity) { m_selectedEntityID = newEntity; }

			virtual void OnImGuiRender(float deltaTime) override;
			virtual void OnKeyPressed(int keyCode) override;

		private:

			void DrawEntityElement(const ECS::ECSEntity& entity, bool firstCall, bool& selectedThisFrame);

		private:

			ECS::ECSEntityID m_selectedEntityID = -1;
		};
	}
}
