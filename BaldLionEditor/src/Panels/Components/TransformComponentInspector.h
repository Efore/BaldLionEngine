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
				UtilsEditor::DrawVec3Handler("World Position", newPosition, 1.0f, 110.0f);

				glm::vec3 deltaPosition = newPosition - componentTransform->position;
				componentTransform->position = newPosition;
				
				glm::vec3 rotation = glm::degrees(componentTransform->rotation);
				UtilsEditor::DrawVec3Handler("World Rotation", rotation, 1.0f, 110.0f);

				glm::vec3 newRotation = glm::radians(glm::vec3(rotation.x, rotation.y, rotation.z));
				glm::vec3 deltaRotation = newRotation - componentTransform->rotation;
				componentTransform->rotation = newRotation;

				glm::vec3 newScale = componentTransform->scale;
				UtilsEditor::DrawVec3Handler("World Scale", newScale, 1.0f, 110.0f);

				glm::vec3 deltaScale = newScale - componentTransform->scale;
				componentTransform->scale = newScale;

				ECS::ECSComponentLookUp selectedEntityComponents;
				if (SceneManager::GetECSManager()->GetEntityComponents().TryGet(sceneHierarchyPanel->GetSelectedEntityID(), selectedEntityComponents))
				{
					ECS::ECSHierarchyComponent* hierarchyComponent = selectedEntityComponents.Write<ECS::ECSHierarchyComponent>(ECS::ECSComponentType::Hierarchy);
					if (hierarchyComponent != nullptr)
					{
						UtilsEditor::TransformChildsRecursive(hierarchyComponent, deltaPosition, deltaRotation, deltaScale);

						if (hierarchyComponent->parentEntityID > 0)
						{
							if (SceneManager::GetECSManager()->GetEntityComponents().TryGet(hierarchyComponent->parentEntityID, selectedEntityComponents))
							{
								const ECS::ECSTransformComponent* parentTrasnformComponent = selectedEntityComponents.Read<ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform);

								glm::vec3 localPosition = componentTransform->position - parentTrasnformComponent->position;
								glm::vec3 localRotation = componentTransform->rotation - parentTrasnformComponent->rotation;
								glm::vec3 localScale = componentTransform->scale - parentTrasnformComponent->scale;

								ImGui::Separator();

								UtilsEditor::DrawVec3Handler("Local Position", localPosition, 0.0f, 110.0f, false);
								UtilsEditor::DrawVec3Handler("Local Rotation", localRotation, 0.0f, 110.0f, false);
								UtilsEditor::DrawVec3Handler("Local Scale", localScale, 0.0f, 110.0f, false);
							}
						}
					}
				}

				ComponentInspector::EndComponentRender();
			}

		};

	}
}

