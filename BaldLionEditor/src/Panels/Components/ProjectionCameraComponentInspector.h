#pragma once

#include "ComponentInspector.h"
#include "BaldLion/ECS/Components/ECSProjectionCameraComponent.h"
#include "BaldLion/Rendering/Renderer.h"

namespace BaldLion
{
	namespace Editor {

		class ProjectionCameraComponentInspector : public ComponentInspector {

		public:

			static void OnImGuiRender(ECS::ECSComponent* component, SceneHierarchyPanel* sceneHierarchyPanel) {
				

				ECS::ECSProjectionCameraComponent* projectionCameraComponent = (ECS::ECSProjectionCameraComponent*)component;

				ComponentInspector::BeginComponentRender("Projection Camera Component", component->GetComponentType(), sceneHierarchyPanel, (GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f) * 9.0f);

				BL_IMGUI_LEFT_LABEL(ImGui::InputFloat, "Near Plane Z", (float*)&projectionCameraComponent->nearPlane);
				BL_IMGUI_LEFT_LABEL(ImGui::InputFloat, "Far Plane Z", (float*)&projectionCameraComponent->farPlane);
				BL_IMGUI_LEFT_LABEL(ImGui::InputFloat, "Width", (float*)&projectionCameraComponent->width);
				BL_IMGUI_LEFT_LABEL(ImGui::InputFloat, "Height", (float*)&projectionCameraComponent->height);

				BL_IMGUI_LEFT_LABEL(ImGui::DragFloat, "Field of View", (float*)&projectionCameraComponent->fov, 1.0f, 20.0f, 180.0f);

				static bool showDebugFrustrum = false;
				BL_IMGUI_LEFT_LABEL(ImGui::Checkbox, "Show debug frustrum", &showDebugFrustrum);

				ComponentInspector::EndComponentRender();	

				if (showDebugFrustrum && sceneHierarchyPanel->GetSelectedEntityID() > 0)
				{
					const ECS::ECSTransformComponent* entityTransformComponent = 
						SceneManager::GetECSManager()->GetEntityComponents().Get(sceneHierarchyPanel->GetSelectedEntityID()).Read<ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform);

					const glm::mat4 viewMatrix = glm::inverse(entityTransformComponent->GetTransformMatrix());

					const glm::mat4 projectionMatrix = glm::perspective(glm::radians(projectionCameraComponent->fov), projectionCameraComponent->width / projectionCameraComponent->height, projectionCameraComponent->nearPlane, projectionCameraComponent->farPlane);

					const glm::mat4 frustrum = glm::inverse(projectionMatrix * viewMatrix);

					Renderer::DrawDebugFrustrum(frustrum, glm::vec3(0.3f, 1.0f, 0.3f), 0, false);
				}
			}

		};

	}
}

