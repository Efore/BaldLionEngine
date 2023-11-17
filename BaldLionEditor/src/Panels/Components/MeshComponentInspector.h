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

				ComponentInspector::BeginComponentRender("Mesh Component", component->GetComponentType(), sceneHierarchyPanel, (GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f) * 5.0f);

				static bool showAABB = false;
				BL_IMGUI_LEFT_LABEL(ImGui::Checkbox, "Show AABB", &showAABB);

				static bool showNormals = false;
				BL_IMGUI_LEFT_LABEL(ImGui::Checkbox, "Show Normals", &showNormals);

				ComponentInspector::EndComponentRender();

				if (sceneHierarchyPanel->GetSelectedEntityID() > 0)
				{
					const ECS::ECSTransformComponent* meshTransform = SceneManager::GetECSManager()->GetEntityComponents().Get(sceneHierarchyPanel->GetSelectedEntityID()).Read< ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform);

					const glm::mat4 meshTransformMatrix = meshTransform->GetTransformMatrix();

					if (showAABB)
					{						
						const BoundingBox meshAABB = GeometryUtils::GetAABB(meshComponent->localBoundingBox, meshTransformMatrix);
						Renderer::DrawDebugBox(meshAABB.center, meshAABB.maxPoint - meshAABB.minPoint, glm::mat4(1.0f), glm::vec3(0.3f, 1.0f, 0.3f));					
					}

					if (showNormals)
					{ 
						BL_DYNAMICARRAY_FOREACH(meshComponent->vertices)
						{
							glm::vec3 position = meshTransformMatrix * glm::vec4(meshComponent->vertices[i].position, 1.0f);
							glm::vec3 normal = meshTransformMatrix * glm::vec4(meshComponent->vertices[i].normal, 1.0f);

							Renderer::DrawDebugLine(position, position + normal, glm::vec3(1.0f, 0.2f, 0.2f), 0.5f);
						}
					}
				}

			}

		};

	}
}


