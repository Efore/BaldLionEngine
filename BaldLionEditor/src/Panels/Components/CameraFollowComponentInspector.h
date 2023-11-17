#pragma once
#include "ComponentInspector.h"
#include "BaldLion/ECS/Components/ECSCameraFollowComponent.h"
#include "BaldLion/ECS/Components/ECSMeshComponent.h"
#include "BaldLion/Rendering/Renderer.h"

namespace BaldLion
{
	namespace Editor {

		class CameraFollowInspector : public ComponentInspector {

		public:

			static void OnImGuiRender(ECS::ECSComponent* component,  SceneHierarchyPanel* sceneHierarchyPanel) {

				ComponentInspector::BeginComponentRender("Camera Follow Component", component->GetComponentType(), sceneHierarchyPanel, (GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f) * 5.0f);

				ECS::ECSCameraFollowComponent* componentCameraFollow = (ECS::ECSCameraFollowComponent*)component;

				glm::vec2 newPosition = componentCameraFollow->offsetXY;
				if (EditorUtils::DrawVec2Handler("Offset XY", newPosition, 0.0f, 110.0f))
				{
					componentCameraFollow->offsetXY = newPosition;						
				}

				BL_IMGUI_LEFT_LABEL(ImGui::InputFloat, "Offset Z", &componentCameraFollow->offsetZ);
				BL_IMGUI_LEFT_LABEL(ImGui::InputFloat, "Rotation Speed", &componentCameraFollow->rotationSpeed);

				if (componentCameraFollow->followedEntityID > 0)
				{
					const ECS::ECSTransformComponent* entityTransformComponent =
						SceneManager::GetECSManager()->GetEntityComponents().Get(sceneHierarchyPanel->GetSelectedEntityID()).Read<ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform);

					const ECS::ECSTransformComponent* followedEntityTransform = SceneManager::GetECSManager()->GetEntityComponents().Get(componentCameraFollow->followedEntityID).Read<ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform);
					
				}


				ComponentInspector::EndComponentRender();
			}

		};

	}
}

