#pragma once

#include "ComponentInspector.h"
#include "BaldLion/ECS/Components/ECSMeshComponent.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/Rendering/Renderer.h"

namespace BaldLion
{
	namespace Editor {

		class MeshComponentInspector : public ComponentInspector {

		public:

			static void OnImGuiRender(ECS::ECSComponent* component, SceneHierarchyPanel* sceneHierarchyPanel) {

				ECS::ECSMeshComponent* meshComponent = (ECS::ECSMeshComponent*)component;

				ComponentInspector::BeginComponentRender("Mesh Component", component->GetComponentType(), sceneHierarchyPanel, (GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f) * 3.0f);

				static bool showDebugFrustrum = false;
				IMGUI_LEFT_LABEL(ImGui::Checkbox, "Show AABB", &showDebugFrustrum);

				ComponentInspector::EndComponentRender();

				if (showDebugFrustrum && sceneHierarchyPanel->GetSelectedEntityID() > 0)
				{
					ECS::ECSTransformComponent* meshTransform = (ECS::ECSTransformComponent*)SceneManager::GetECSManager()->GetEntityComponents().Get(sceneHierarchyPanel->GetSelectedEntityID())[(ui32)ECS::ECSComponentType::Transform];

					const glm::mat4 meshTransformMatrix = meshTransform->GetTransformMatrix();

					const glm::vec3 minPointInWorldSpace = meshTransformMatrix * glm::vec4(meshComponent->localAABB.minPoint, 1.0f);
					const glm::vec3 maxPointInWorldSpace = meshTransformMatrix * glm::vec4(meshComponent->localAABB.maxPoint, 1.0f);

					const float minX = minPointInWorldSpace.x < maxPointInWorldSpace.x ? minPointInWorldSpace.x : maxPointInWorldSpace.x;
					const float minY = minPointInWorldSpace.y < maxPointInWorldSpace.y ? minPointInWorldSpace.y : maxPointInWorldSpace.y;
					const float minZ = minPointInWorldSpace.z < maxPointInWorldSpace.z ? minPointInWorldSpace.z : maxPointInWorldSpace.z;

					const float maxX = minPointInWorldSpace.x > maxPointInWorldSpace.x ? minPointInWorldSpace.x : maxPointInWorldSpace.x;
					const float maxY = minPointInWorldSpace.y > maxPointInWorldSpace.y ? minPointInWorldSpace.y : maxPointInWorldSpace.y;
					const float maxZ = minPointInWorldSpace.z > maxPointInWorldSpace.z ? minPointInWorldSpace.z : maxPointInWorldSpace.z;

					const AABB meshAABB = { glm::vec3(minX,minY,minZ), glm::vec3(maxX,maxY,maxZ) };

					glm::vec3 midPoint = meshAABB.minPoint + (meshAABB.maxPoint - meshAABB.minPoint) * 0.5f;

					Renderer::DrawDebugBox(midPoint, meshAABB.maxPoint - meshAABB.minPoint, glm::vec3(0.3f, 1.0f, 0.3f));
				}

			}

		};

	}
}


