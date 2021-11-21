#pragma once
#include "EntityPropertiesPanel.h"
#include "Components/TransformComponentInspector.h"
#include "Components/DirectionalLightComponentInspector.h"
#include "BaldLion/ECS/ECSManager.h"

namespace BaldLion {

	namespace Editor {

		EntityPropertiesPanel::EntityPropertiesPanel(SceneHierarchyPanel* sceneHierarchyPanel) : m_sceneHierarchyPanel(sceneHierarchyPanel)
		{

		}

		void EntityPropertiesPanel::OnImGuiRender()
		{
			ImGui::Begin("Properties");

			const ECS::ECSEntityID selectedEntityID = m_sceneHierarchyPanel->GetSelectedEntityID();

			ECS::ECSEntity* entity = nullptr;
			if (m_sceneHierarchyPanel->GetSceneContext()->GetECSManager()->GetEntityMap().TryGet(selectedEntityID, entity)) {

				ImGui::Text(STRINGID_TO_STRING(entity->GetEntityName()));
				ImGui::Separator();

				ECS::ECSComponentLookUp selectedEntityComponents;

				if (m_sceneHierarchyPanel->GetSceneContext()->GetECSManager()->GetEntityComponents().TryGet(selectedEntityID, selectedEntityComponents)) {

					ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
					for (ui32 i = 0; i < (ui32)ECS::ECSComponentID::Count; ++i)
					{
						if (selectedEntityComponents[i] != nullptr)
						{
							RenderComponentInspector(selectedEntityComponents[i]);
						}
					}

					ImGui::PopStyleVar();
				}
			}

			ImGui::End();
		}

		void EntityPropertiesPanel::SetHierarchyPanel(SceneHierarchyPanel* sceneHierarchyPanel)
		{
			m_sceneHierarchyPanel = sceneHierarchyPanel;
		}

		void EntityPropertiesPanel::RenderComponentInspector(ECS::ECSComponent* component)
		{
			switch (component->GetComponentID())
			{
				case ECS::ECSComponentID::Transform:
					TransformComponentInspector::OnImGuiRender(component);
				break;
				case ECS::ECSComponentID::DirectionalLight:
					DirectionalLightComponentInspector::OnImGuiRender(component);
				break;
			}
		}
	}
}
