#pragma once
#include "EntityPropertiesPanel.h"
#include "Components/TransformComponentInspector.h"
#include "Components/DirectionalLightComponentInspector.h"
#include "Components/ProjectionCameraComponentInspector.h"
#include "BaldLion/ECS/ECSManager.h"
#include "BaldLion/Utils/MathUtils.h"
#include "BaldLion/ResourceManagement/ResourceManager.h"
#include "BaldLion/Rendering/Mesh.h"


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
				ImGui::Text(STRINGID_TO_STR_C(entity->GetEntityName()));

				const char* names[] = { 
										"Transform",
										"ProjectionCamera",
										"Mesh",
										"Skeleton",
										"DirectionalLight",
										"PointLight",
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

					for (int i = 0; i < IM_ARRAYSIZE(names); i++)
					{
						if (ImGui::Selectable(names[i]))
						{
							ECSComponentLookUp selectedEntityComponents;
							if (m_sceneHierarchyPanel->GetSceneContext()->GetECSManager()->GetEntityComponents().TryGet(selectedEntityID, selectedEntityComponents)) 
							{
								if (selectedEntityComponents[i] == nullptr) 
								{		
									ECSComponentType chosenType = (ECSComponentType)i;
									ECSComponent* newComponent = nullptr;

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
									{
										ImGui::OpenPopup("Choose Mesh");

										if (ImGui::BeginPopupModal("Choose Mesh", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
										{		
											if (ImGui::Button("Close"))
											{
												ImGui::CloseCurrentPopup();
											}

											ImGui::Separator();

											for (auto hashMapIterator = ResourceManagement::ResourceManager::GetResourceMap().Begin(); hashMapIterator != ResourceManagement::ResourceManager::GetResourceMap().End(); ++hashMapIterator)
											{
												if (hashMapIterator.GetValue()->GetResourceType() != ResourceManagement::ResourceType::Mesh)
												{
													continue;
												}

												if (ImGui::Selectable(STRINGID_TO_STR_C(hashMapIterator.GetValue()->GetResourceName())))
												{
													Rendering::Mesh* meshToLoad = (Rendering::Mesh*)hashMapIterator.GetValue();
													newComponent = meshToLoad->GenerateMeshComponent(m_sceneHierarchyPanel->GetSceneContext()->GetECSManager(), false);
												}
											}

											ImGui::EndPopup();
										}
									}
										break;

									case ECS::ECSComponentType::Skeleton:
										break;

									case ECS::ECSComponentType::DirectionalLight:

										newComponent = m_sceneHierarchyPanel->GetSceneContext()->GetECSManager()->AddComponent<ECSDirectionalLightComponent>(ECSComponentType::DirectionalLight,
											glm::vec3(0.0f),
											glm::vec3(1.0f),
											glm::vec3(0.2f),
											glm::vec3(-1.0f, -1.0f, -1.0f));
											
										break;

									case ECS::ECSComponentType::PointLight:
										break;

									case ECS::ECSComponentType::Animation:
										break;
									
									}

									if (newComponent) {
										m_sceneHierarchyPanel->GetSceneContext()->GetECSManager()->AddComponentToEntity(selectedEntityID, newComponent);
									}
								}
							}
						}
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
