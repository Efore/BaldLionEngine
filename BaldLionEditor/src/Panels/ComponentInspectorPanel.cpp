#pragma once
#include "ComponentInspectorPanel.h"
#include "Components/TransformComponentInspector.h"
#include "BaldLion/ECS/ECSManager.h"

namespace BaldLion {

	namespace Editor {

		ComponentInspectorPanel::ComponentInspectorPanel(SceneHierarchyPanel* sceneHierarchyPanel) : m_sceneHierarchyPanel(sceneHierarchyPanel)
		{

		}

		void ComponentInspectorPanel::OnImGuiRender()
		{
			ImGui::Begin("Components");

			const ECS::ECSEntityID selectedEntityID = m_sceneHierarchyPanel->GetSelectedEntityID();

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

			ImGui::End();
		}

		void ComponentInspectorPanel::SetHierarchyPanel(SceneHierarchyPanel* sceneHierarchyPanel)
		{
			m_sceneHierarchyPanel = sceneHierarchyPanel;
		}

		void ComponentInspectorPanel::RenderComponentInspector(ECS::ECSComponent* component)
		{

			switch (component->GetComponentID())
			{
				case ECS::ECSComponentID::Transform:

					float childHeight = (GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f) * 5.0f;
					ImGui::BeginChild("Transform", ImVec2(0, childHeight), true, ImGuiWindowFlags_MenuBar);

					if(ImGui::BeginMenuBar())
					{
						if (ImGui::BeginMenu("Transform Component"))
						{
							if (ImGui::MenuItem("Remove component"))
							{

							}

							ImGui::EndMenu();
						}
						ImGui::EndMenuBar();
					}

					TransformComponentInspector::OnImGuiRender(component);
					ImGui::EndChild();

				break;
			}

		
		}
	}
}
