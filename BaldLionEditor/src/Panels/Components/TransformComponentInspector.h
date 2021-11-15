#pragma once
#include "ComponentInspector.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"

namespace BaldLion
{
	namespace Editor {

		class TransformComponentInspector : public ComponentInspector {

		public:

			static void OnImGuiRender(ECS::ECSComponent* component) {

				ECS::ECSTransformComponent* componentTransform = (ECS::ECSTransformComponent*)component;

				DrawVec3Handler("Position", componentTransform->position);

				glm::vec3 rotation = glm::degrees(componentTransform->rotation);
				DrawVec3Handler("Rotation", rotation);
				componentTransform->rotation = glm::radians(glm::vec3(rotation.x, rotation.y, rotation.z));

				DrawVec3Handler("Scale", componentTransform->scale, 1.0f);
			}

		};

	}
}

