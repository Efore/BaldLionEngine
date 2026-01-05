#pragma once
#include "SceneHierarchyPanel.h"
#include "BaldLion/ECS/ECSComponent.h"
#include "BaldLion/ResourceManagement/ResourceManager.h"

namespace BaldLion {

	namespace Editor {

		class EntityPropertiesPanel : public EditorPanel {

		public:
			EntityPropertiesPanel();
			EntityPropertiesPanel(SceneHierarchyPanel* sceneHierarchyPanel);			

			virtual void OnImGuiRender(float deltaTime) override;
			void SetHierarchyPanel(SceneHierarchyPanel* sceneHierarchyPanel);

		private:
			void RenderComponentInspector(ECS::ECSComponent* component);


		private:

			SceneHierarchyPanel* m_sceneHierarchyPanel = nullptr;			
		};
	}
}
