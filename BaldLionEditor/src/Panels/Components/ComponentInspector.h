#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/glm.hpp>
#include "SceneHierarchyPanel.h"
#include "EditorUtils.h"

namespace BaldLion {

	namespace Editor {

		class ComponentInspector {

		public:

			static void BeginComponentRender(const char* componentName, ECS::ECSComponentType componentID, SceneHierarchyPanel* sceneHierarchyPanel, float childHeight) {

				ImGui::BeginChild(componentName, ImVec2(0, childHeight), true, ImGuiWindowFlags_MenuBar);

				if (ImGui::BeginMenuBar())
				{
					if (ImGui::BeginMenu(componentName)) //name of the component here
					{
						if (ImGui::MenuItem("Remove component"))
						{
							sceneHierarchyPanel->GetSceneContext()->GetECSManager()->RemoveComponentFromEntity(componentID, sceneHierarchyPanel->GetSelectedEntityID());
						}

						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();
				}
			}

			static void EndComponentRender() {

				ImGui::EndChild();
			}						
			
		};

		

	}
}
