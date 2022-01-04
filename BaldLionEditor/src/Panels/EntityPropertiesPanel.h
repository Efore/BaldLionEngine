#pragma once
#include "SceneHierarchyPanel.h"
#include "BaldLion/ECS/ECSComponent.h"

namespace BaldLion {

	namespace Editor {

		class EntityPropertiesPanel : public EditorPanel {

		public:
			EntityPropertiesPanel() = default;
			EntityPropertiesPanel(SceneHierarchyPanel* sceneHierarchyPanel);			

			virtual void OnImGuiRender() override;
			void SetHierarchyPanel(SceneHierarchyPanel* sceneHierarchyPanel);

		private:
			void RenderComponentInspector(ECS::ECSComponent* component);

		private:

			SceneHierarchyPanel* m_sceneHierarchyPanel;			
		};
	}
}
