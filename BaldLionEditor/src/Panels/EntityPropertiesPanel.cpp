#pragma once
#include "EntityPropertiesPanel.h"
#include "UtilsEditor.h"

#include "Components/TransformComponentInspector.h"
#include "Components/DirectionalLightComponentInspector.h"
#include "Components/ProjectionCameraComponentInspector.h"
#include "BaldLion/ECS/ECSManager.h"
#include "BaldLion/ECS/ECSComponentsInclude.h"
#include "BaldLion/Utils/MathUtils.h"
#include "BaldLion/ResourceManagement/ResourceManager.h"
#include "BaldLion/Rendering/Mesh.h"
#include "BaldLion/ECS/ComponentsSingleton/ECSLightSingleton.h"
#include "BaldLion/Animation/AnimationManager.h"


namespace BaldLion {
	
	using namespace ECS;
	namespace Editor {

		EntityPropertiesPanel::EntityPropertiesPanel(SceneHierarchyPanel* sceneHierarchyPanel) : m_sceneHierarchyPanel(sceneHierarchyPanel)
		{

		}

		void EntityPropertiesPanel::OnImGuiRender()
		{
			ImGui::Begin("Properties");

			const ECS::ECSEntityID selectedEntityID = m_sceneHierarchyPanel->GetSelectedEntityID();

			ECS::ECSEntity* entity = nullptr;
			if (m_sceneHierarchyPanel->GetSceneContext()->GetECSManager()->GetEntityMap().TryGet(selectedEntityID, entity)) {

				ImGui::SameLine();
				ImGui::Text(BL_STRINGID_TO_STR_C(entity->GetEntityName()));

				const char* componentTypeNames[] = { 
										"Transform",
										"ProjectionCamera",
										"Mesh",
										"Skeleton",
										"DirectionalLight",										
										"Animation"
									};

				// Simple selection popup
				// (If you want to show the current selection inside the Button itself, you may want to build a string using the "###" operator to preserve a constant ID with a variable label)
				if (ImGui::Button("AddComponent"))
				{
					ImGui::OpenPopup("add_component_popup");
				}
				
				if (ImGui::BeginPopup("add_component_popup"))
				{
					ImGui::Text("Components");
					ImGui::Separator();

					ECSComponentLookUp selectedEntityComponents;
					ECSComponent* newComponent = nullptr;

					const char* meshPopup = "Choose Mesh";
					const char* animatorPopup = "Choose Animator";
					

					if (m_sceneHierarchyPanel->GetSceneContext()->GetECSManager()->GetEntityComponents().TryGet(selectedEntityID, selectedEntityComponents)) 
					{
						for (int i = 0; i < IM_ARRAYSIZE(componentTypeNames); i++)
						{
							if (selectedEntityComponents[i] == nullptr)
							{
								if (ImGui::Button(componentTypeNames[i]))
								{
									ECSComponentType chosenType = (ECSComponentType)i;

									switch (chosenType)
									{

									case ECS::ECSComponentType::Transform:

										newComponent = m_sceneHierarchyPanel->GetSceneContext()->GetECSManager()->AddComponent<ECS::ECSTransformComponent>(ECSComponentType::Transform,
											MathUtils::Vector3Zero,
											MathUtils::Vector3Zero,
											glm::vec3(1.0f));

										break;

									case ECS::ECSComponentType::ProjectionCamera:

										newComponent = m_sceneHierarchyPanel->GetSceneContext()->GetECSManager()->AddComponent<ECS::ECSProjectionCameraComponent>(ECSComponentType::ProjectionCamera,
											45.0f,
											600.0f,
											480.0f,
											0.1f,
											50000.0f);

										break;

									case ECS::ECSComponentType::Mesh:
									case ECS::ECSComponentType::Skeleton:
									
										ImGui::OpenPopup(meshPopup);									
									
										break;									

									case ECS::ECSComponentType::DirectionalLight:

										newComponent = m_sceneHierarchyPanel->GetSceneContext()->GetECSManager()->AddComponent<ECSDirectionalLightComponent>(ECSComponentType::DirectionalLight,
											glm::vec3(0.0f),
											glm::vec3(1.0f),
											glm::vec3(0.2f),
											glm::vec3(-1.0f, -1.0f, -1.0f));

										ECS::SingletonComponents::ECSLightSingleton::SetDirectionalLight((ECSDirectionalLightComponent*)newComponent);
											
										break;

									case ECS::ECSComponentType::PointLight:
										break;

									case ECS::ECSComponentType::Animation:
										ImGui::OpenPopup(animatorPopup);
										break;
									
									}

									break;
								}
							}
						}
					}
					
					Rendering::Mesh* mesh = UtilsEditor::RenderResourceInspectorPopup<Rendering::Mesh>(meshPopup, ResourceManagement::ResourceType::Mesh);

					if (mesh)
					{						
						if (selectedEntityComponents[(ui32)ECSComponentType::Transform] == nullptr) 
						{
							ECSTransformComponent* transform = m_sceneHierarchyPanel->GetSceneContext()->GetECSManager()->AddComponent<ECS::ECSTransformComponent>(ECSComponentType::Transform,
								MathUtils::Vector3Zero,
								MathUtils::Vector3Zero,
								glm::vec3(1.0f));

							m_sceneHierarchyPanel->GetSceneContext()->GetECSManager()->AddComponentToEntity(selectedEntityID, transform);
						}

						ECS::ECSSkeletonComponent* skeletonComponent = nullptr;
						ECS::ECSMeshComponent* meshComponent = nullptr;
						mesh->GenerateMeshComponent(m_sceneHierarchyPanel->GetSceneContext()->GetECSManager(), false, meshComponent, skeletonComponent);

						newComponent = meshComponent;

						if (skeletonComponent) 
						{
							m_sceneHierarchyPanel->GetSceneContext()->GetECSManager()->AddComponentToEntity(selectedEntityID, skeletonComponent);
						}
					}

					if (ImGui::BeginPopupModal(animatorPopup, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
					{
						BL_HASHTABLE_FOR(Animation::AnimationManager::GetAnimators(), hashMapIterator)
						{
							if (ImGui::Selectable(BL_STRINGID_TO_STR_C(hashMapIterator.GetValue()->GetResourcePath())))
							{
								newComponent = m_sceneHierarchyPanel->GetSceneContext()->GetECSManager()->AddComponent<ECS::ECSAnimationComponent>(ECSComponentType::Animation,
									hashMapIterator.GetValue()->GetResourceID(),
									hashMapIterator.GetValue()->GetInitialAnimationID());
							}
							
						}
						ImGui::EndPopup();
					}

					if (newComponent) 
					{
						m_sceneHierarchyPanel->GetSceneContext()->GetECSManager()->AddComponentToEntity(selectedEntityID, newComponent);
					}

					ImGui::EndPopup();
				}

				ImGui::Separator();			
				

				ECS::ECSComponentLookUp selectedEntityComponents;

				if (m_sceneHierarchyPanel->GetSceneContext()->GetECSManager()->GetEntityComponents().TryGet(selectedEntityID, selectedEntityComponents)) {

					ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
					for (ui32 i = 0; i < (ui32)ECS::ECSComponentType::Count; ++i)
					{
						if (selectedEntityComponents[i] != nullptr)
						{
							RenderComponentInspector(selectedEntityComponents[i]);
						}
					}

					ImGui::PopStyleVar();
				}
			}

			ImGui::End();
		}

		void EntityPropertiesPanel::SetHierarchyPanel(SceneHierarchyPanel* sceneHierarchyPanel)
		{
			m_sceneHierarchyPanel = sceneHierarchyPanel;
		}

		void EntityPropertiesPanel::RenderComponentInspector(ECS::ECSComponent* component)
		{
			switch (component->GetComponentType())
			{				
				case ECS::ECSComponentType::Transform:
					TransformComponentInspector::OnImGuiRender(component, m_sceneHierarchyPanel);
				break;

				case ECS::ECSComponentType::DirectionalLight:
					DirectionalLightComponentInspector::OnImGuiRender(component, m_sceneHierarchyPanel);
					break;

				case ECS::ECSComponentType::ProjectionCamera:
					ProjectionCameraComponentInspector::OnImGuiRender(component, m_sceneHierarchyPanel);
					break;

				case ECS::ECSComponentType::Mesh:
					ComponentInspector::BeginComponentRender("Mesh Component", ECS::ECSComponentType::Mesh, m_sceneHierarchyPanel, GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f);
					ComponentInspector::EndComponentRender();
					break;

				case ECS::ECSComponentType::Skeleton:
					ComponentInspector::BeginComponentRender("Skeleton Component", ECS::ECSComponentType::Skeleton, m_sceneHierarchyPanel, GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f);
					ComponentInspector::EndComponentRender();
					break;

				case ECS::ECSComponentType::PointLight:
					ComponentInspector::BeginComponentRender("Point Light Component", ECS::ECSComponentType::PointLight, m_sceneHierarchyPanel, GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f);
					ComponentInspector::EndComponentRender();
					break;

				case ECS::ECSComponentType::Animation:
					ComponentInspector::BeginComponentRender("Animation Component", ECS::ECSComponentType::Animation, m_sceneHierarchyPanel, GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f);
					ComponentInspector::EndComponentRender();
					break;
				
				default:
					break;
				
			}
		}


	}
}
