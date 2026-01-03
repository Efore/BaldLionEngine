#pragma once
#include "ComponentInspector.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/ECS/Components/ECSMeshComponent.h"

namespace BaldLion
{
	namespace Editor {

		class TransformComponentInspector : public ComponentInspector {

		public:

			static void OnImGuiRender(ECS::ECSComponent* component,  SceneHierarchyPanel* sceneHierarchyPanel) {

				ComponentInspector::BeginComponentRender("Transform Component", component->GetComponentType(), sceneHierarchyPanel, (GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f) * 5.0f);

				ECS::ECSTransformComponent* componentTransform = (ECS::ECSTransformComponent*)component;				

				static bool positionEdited = false;
				static bool rotationEdited = false;
				static bool scaleEdited = false;

				bool modified = false;

				{
					glm::vec3 newPosition = componentTransform->position;
					if (EditorUtils::DrawVec3Handler("World Position", newPosition, 0.0f, 110.0f))
					{
						if (glm::any(glm::epsilonNotEqual(newPosition, componentTransform->position, glm::epsilon<float>())) && !positionEdited)
						{
							positionEdited = true;
						}
					}
					componentTransform->position = newPosition;
				}
				
				{
					glm::vec3 newRotation = glm::degrees(componentTransform->rotation);
					if (EditorUtils::DrawVec3Handler("World Rotation", newRotation, 0.0f, 110.0f))
					{
						if (glm::any(glm::epsilonNotEqual(newRotation, componentTransform->rotation, glm::epsilon<float>())) && !rotationEdited)
						{
							rotationEdited = true;
						}
					}
					componentTransform->rotation = glm::radians(newRotation);
				}

				{
					glm::vec3 newScale = componentTransform->scale;
					if (EditorUtils::DrawVec3Handler("World Scale", newScale, 1.0f, 110.0f))
					{
						if (glm::any(glm::epsilonNotEqual(newScale, componentTransform->scale, glm::epsilon<float>())) && !scaleEdited)
						{
							scaleEdited = true;
						}
					}
					componentTransform->scale = newScale;
				}

				ECS::ECSEntity* entity = SceneManager::GetECSManager()->GetEntityMap().Get(sceneHierarchyPanel->GetSelectedEntityID());

				if (entity->GetParentID() > 0) {

					ECS::ECSTransformComponent* parentTransformComponent = SceneManager::GetECSManager()->GetEntityComponents().Get(entity->GetParentID()).Write< ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform);

					glm::vec3 localPosition = componentTransform->position - parentTransformComponent->position;
					glm::vec3 localRotation = componentTransform->rotation - parentTransformComponent->rotation;
					glm::vec3 localScale = componentTransform->scale - parentTransformComponent->scale;

					ImGui::Separator();

					EditorUtils::DrawVec3Handler("Local Position", localPosition, 0.0f, 110.0f, false);
					EditorUtils::DrawVec3Handler("Local Rotation", (glm::vec3&)glm::degrees(localRotation), 0.0f, 110.0f, false);
					EditorUtils::DrawVec3Handler("Local Scale", localScale, 0.0f, 110.0f, false);
				}			

				if (positionEdited || rotationEdited || scaleEdited)
				{
					SceneManager::GetECSManager()->MarkEntityTransformAsChangedInHierarchy(entity->GetEntityID());
				}

				positionEdited = false;
				rotationEdited = false;
				scaleEdited = false;


				ComponentInspector::EndComponentRender();
			}

		};

	}
}

