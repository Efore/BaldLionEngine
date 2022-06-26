#pragma once
#include "ComponentInspector.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"

namespace BaldLion
{
	namespace Editor {

		class TransformComponentInspector : public ComponentInspector {

		public:

			static void OnImGuiRender(ECS::ECSComponent* component,  SceneHierarchyPanel* sceneHierarchyPanel) {

				ComponentInspector::BeginComponentRender("Transform Component", component->GetComponentType(), sceneHierarchyPanel, (GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f) * 5.0f);

				ECS::ECSTransformComponent* componentTransform = (ECS::ECSTransformComponent*)component;
				
				glm::vec3 newPosition = componentTransform->position;
				EditorUtils::DrawVec3Handler("World Position", newPosition, 0.0f, 110.0f);				
				componentTransform->position = newPosition;
				
				glm::vec3 rotation = glm::degrees(componentTransform->rotation);
				EditorUtils::DrawVec3Handler("World Rotation", rotation, 0.0f, 110.0f);			
				componentTransform->rotation = glm::radians(rotation);

				glm::vec3 newScale = componentTransform->scale;
				EditorUtils::DrawVec3Handler("World Scale", newScale, 1.0f, 110.0f);				
				componentTransform->scale = newScale;

				ECS::ECSEntity* entity = SceneManager::GetECSManager()->GetEntityMap().Get(sceneHierarchyPanel->GetSelectedEntityID());

				if (entity->GetParentID() > 0) {

					ECS::ECSTransformComponent* parentTransformComponent = (ECS::ECSTransformComponent*)SceneManager::GetECSManager()->GetEntityComponents().Get(entity->GetParentID())[(ui32)ECS::ECSComponentType::Transform];

					glm::vec3 localPosition = componentTransform->position - parentTransformComponent->position;
					glm::vec3 localRotation = componentTransform->rotation - parentTransformComponent->rotation;
					glm::vec3 localScale = componentTransform->scale - parentTransformComponent->scale;

					ImGui::Separator();

					EditorUtils::DrawVec3Handler("Local Position", localPosition, 0.0f, 110.0f, false);
					EditorUtils::DrawVec3Handler("Local Rotation", glm::degrees(localRotation), 0.0f, 110.0f, false);
					EditorUtils::DrawVec3Handler("Local Scale", localScale, 0.0f, 110.0f, false);
				}

				ComponentInspector::EndComponentRender();
			}

		};

	}
}

