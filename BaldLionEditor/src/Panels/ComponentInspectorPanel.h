#pragma once
#include "BaldLion/SceneManagement/Scene.h"
#include "SceneHierarchyPanel.h"
#include "BaldLion/ECS/ECSComponent.h"

namespace BaldLion {

	namespace Editor {

		class ComponentInspectorPanel {

		public:
			ComponentInspectorPanel() = default;
			ComponentInspectorPanel(SceneHierarchyPanel* sceneHierarchyPanel);			

			void OnImGuiRender();
			void SetHierarchyPanel(SceneHierarchyPanel* sceneHierarchyPanel);

		private:
			void RenderComponentInspector(ECS::ECSComponent* component);

		private:

			SceneHierarchyPanel* m_sceneHierarchyPanel;			
		};
	}
}