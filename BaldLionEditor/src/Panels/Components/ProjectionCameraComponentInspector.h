#pragma once

#include "ComponentInspector.h"
#include "BaldLion/ECS/Components/ECSProjectionCameraComponent.h"

namespace BaldLion
{
	namespace Editor {

		class ProjectionCameraComponentInspector : public ComponentInspector {

		public:

			static void OnImGuiRender(ECS::ECSComponent* component, SceneHierarchyPanel* sceneHierarchyPanel) {

				ECS::ECSProjectionCameraComponent* projectionCameraComponent = (ECS::ECSProjectionCameraComponent*)component;

				ComponentInspector::BeginComponentRender("Projection Camera Component", component->GetComponentType(), sceneHierarchyPanel, (GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f) * 9.0f);

				ImGui::InputFloat("Near Plane Z", (float*)&projectionCameraComponent->nearPlane);
				ImGui::InputFloat("Far Plane Z", (float*)&projectionCameraComponent->farPlane);
				ImGui::InputFloat("Width", (float*)&projectionCameraComponent->width);
				ImGui::InputFloat("Height", (float*)&projectionCameraComponent->height);

				ImGui::DragFloat("Field of View", (float*)&projectionCameraComponent->fov, 1.0f, 20.0f, 180.0f);

				ComponentInspector::EndComponentRender();
			}

		};

	}
}
