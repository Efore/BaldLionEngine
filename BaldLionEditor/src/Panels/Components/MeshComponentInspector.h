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

					const BoundingBox meshAABB = GeometryUtils::GetAABB(meshComponent->localBoundingBox, meshTransformMatrix);										

					Renderer::DrawDebugBox(meshAABB.GetCenter(), meshAABB.maxPoint - meshAABB.minPoint, glm::vec3(0.3f, 1.0f, 0.3f));
				}

			}

		};

	}
}


