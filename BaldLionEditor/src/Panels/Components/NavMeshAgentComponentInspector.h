#pragma once
#include "ComponentInspector.h"
#include "BaldLion/ECS/Components/ECSNavMeshAgentComponent.h"

namespace BaldLion
{
	namespace Editor {

		class NavMeshAgentComponentInspector : public ComponentInspector {

		public:

			static void OnImGuiRender(ECS::ECSComponent* component, SceneHierarchyPanel* sceneHierarchyPanel) {

				ECS::ECSNavMeshAgentComponent* navMeshAgentComponent = (ECS::ECSNavMeshAgentComponent*)component;

				ComponentInspector::BeginComponentRender("NavMesh Agent Component", component->GetComponentType(), sceneHierarchyPanel, (GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f) * 6.0f);

				ImGui::Text("Agent Idx: %i", navMeshAgentComponent->crowdAgentIdx);
				BL_IMGUI_LEFT_LABEL(ImGui::InputFloat, "Agent Max Speed", &navMeshAgentComponent->agentMaxSpeed);
				BL_IMGUI_LEFT_LABEL(ImGui::InputFloat, "Agent Max Acceleration", &navMeshAgentComponent->agentMaxAcceleration);

				ComponentInspector::EndComponentRender();
			}

		};

	}
}

