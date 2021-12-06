#pragma once
#include "ComponentInspector.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"

namespace BaldLion
{
	namespace Editor {

		class TransformComponentInspector : public ComponentInspector {

		public:

			static void OnImGuiRender(ECS::ECSComponent* component, SceneHierarchyPanel* sceneHierarchyPanel) {

				ComponentInspector::BeginComponentRender("Transform Component", component->GetComponentType(), sceneHierarchyPanel, (GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f) * 5.0f);

				ECS::ECSTransformComponent* componentTransform = (ECS::ECSTransformComponent*)component;

				DrawVec3Handler("Position", componentTransform->position);

				glm::vec3 rotation = glm::degrees(componentTransform->rotation);
				DrawVec3Handler("Rotation", rotation);
				componentTransform->rotation = glm::radians(glm::vec3(rotation.x, rotation.y, rotation.z));

				DrawVec3Handler("Scale", componentTransform->scale, 1.0f);

				ComponentInspector::EndComponentRender();
			}

		};

	}
}

