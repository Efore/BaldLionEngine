#pragma once
#include "ComponentInspector.h"
#include "BaldLion/ECS/Components/ECSLocomotionComponent.h"

namespace BaldLion
{
	namespace Editor {

		class LocomotionComponentInspector : public ComponentInspector {

		public:

			static void OnImGuiRender(ECS::ECSComponent* component, SceneHierarchyPanel* sceneHierarchyPanel) {

				ECS::ECSLocomotionComponent* locomotionComponent = (ECS::ECSLocomotionComponent*)component;

				ComponentInspector::BeginComponentRender("Locomotion Component", component->GetComponentType(), sceneHierarchyPanel, (GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f) * 5.0f);

				BL_IMGUI_LEFT_LABEL(ImGui::InputFloat, "Rotation Speed", &locomotionComponent->rotationSpeed);				
				BL_IMGUI_LEFT_LABEL(ImGui::InputFloat, "Max Movement Speed", &locomotionComponent->maxMovementSpeed);
				BL_IMGUI_LEFT_LABEL(ImGui::InputFloat, "Max Acceleration", &locomotionComponent->maxAcceleration);
				
				ComponentInspector::EndComponentRender();
			}

		};

	}
}

