#pragma once
#include "EntityPropertiesPanel.h"
#include "EditorUtils.h"

#include "Components/TransformComponentInspector.h"
#include "Components/DirectionalLightComponentInspector.h"
#include "Components/CameraFollowComponentInspector.h"
#include "Components/ProjectionCameraComponentInspector.h"
#include "Components/AnimationComponentInspector.h"
#include "Components/MeshComponentInspector.h"
#include "Components/PhyisicsBodyComponentInspector.h"
#include "Components/LocomotionComponentInspector.h"
#include "Components/NavMeshAgentComponentInspector.h"
#include "Components/HTNAgentComponentInspector.h"

#include "BaldLion/ECS/ECSManager.h"
#include "BaldLion/ECS/ECSComponentsInclude.h"
#include "BaldLion/Utils/MathUtils.h"
#include "BaldLion/ResourceManagement/ResourceManager.h"
#include "BaldLion/Rendering/Mesh.h"
#include "BaldLion/ECS/SingletonSystems/LightningSystem.h"
#include "BaldLion/Animation/AnimationManager.h"



namespace BaldLion {
	
	using namespace ECS;
	namespace Editor {

		EntityPropertiesPanel::EntityPropertiesPanel() : EditorPanel("Properties")
		{

		}

		EntityPropertiesPanel::EntityPropertiesPanel(SceneHierarchyPanel* sceneHierarchyPanel) : EditorPanel("Properties"), m_sceneHierarchyPanel(sceneHierarchyPanel)
		{

		}

		void EntityPropertiesPanel::OnImGuiRender(float deltaTime)
		{
			ImGui::Begin(BL_STRINGID_TO_STR_C(m_panelName));
			m_panelID = ImGui::GetCurrentWindow()->ID;

			if (!SceneManagement::SceneManager::GetMainScene())
			{
				ImGui::End();
				return;
			}

			const ECS::ECSEntityID selectedEntityID = m_sceneHierarchyPanel->GetSelectedEntityID();

			if (selectedEntityID < 0)
			{
				ImGui::End();
				return;
			}

			ECS::ECSEntity* entity = nullptr;
			if (SceneManagement::SceneManager::GetMainScene()->GetECSManager()->GetEntityMap().TryGet(selectedEntityID, entity)) {

				ImGui::SameLine();
				ImGui::Text(BL_STRINGID_TO_STR_C(entity->GetEntityName()));

				const char* componentTypeNames[] = { 
										"Transform",
										"ProjectionCamera",
										"Mesh",
										"Skeleton",
										"DirectionalLight",										
										"Animation",
										"Physics Body",
										"NavMesh Agent",
										"Locomotion",
										"Camera Follow",
										"Player Controller"
									};

				const char* physicsBodyShapes[] = {
										"Box",
										"Sphere",
										"Cilinder"
				};

				const char* meshStatic[] = {
										"Dynamic",
										"Static"
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
					const char* physicsBodyPopup = "Physics Body Shape";
					const char* followedEntityPopup = "Choose Followed Entity";
					const char* htnAgentPopup = "Choose HTN Domain and World State Blackboard";

					if (SceneManagement::SceneManager::GetMainScene()->GetECSManager()->GetEntityComponents().TryGet(selectedEntityID, selectedEntityComponents))
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

										newComponent = SceneManagement::SceneManager::GetMainScene()->GetECSManager()->CreateComponent<ECS::ECSTransformComponent>(ECSComponentType::Transform,
											MathUtils::Vector3Zero,
											MathUtils::Vector3Zero,
											glm::vec3(1.0f));

										break;

									case ECS::ECSComponentType::ProjectionCamera:

										newComponent = SceneManagement::SceneManager::GetMainScene()->GetECSManager()->CreateComponent<ECS::ECSProjectionCameraComponent>(ECSComponentType::ProjectionCamera,
											45.0f,
											600.0f,
											480.0f,
											0.1f,
											1000.0f);

										break;

									case ECS::ECSComponentType::Mesh:
									case ECS::ECSComponentType::Skeleton:
									
										ImGui::OpenPopup(meshPopup);									
									
										break;									

									case ECS::ECSComponentType::DirectionalLight:

										newComponent = SceneManagement::SceneManager::GetMainScene()->GetECSManager()->CreateComponent<ECSDirectionalLightComponent>(ECSComponentType::DirectionalLight,
											glm::vec3(0.0f),
											glm::vec3(1.0f),
											glm::vec3(0.2f));

										ECS::SingletonSystems::LightningSystem::SetDirectionalLight((ECSDirectionalLightComponent*)newComponent);
											
										break;

									case ECS::ECSComponentType::Locomotion:
										newComponent = SceneManagement::SceneManager::GetMainScene()->GetECSManager()->CreateComponent<ECSLocomotionComponent>(ECSComponentType::Locomotion,
											5.0f, 3.5f, 3.5f);
										break;

									case ECS::ECSComponentType::Animation:
										ImGui::OpenPopup(animatorPopup);
										break;

									case ECS::ECSComponentType::PhysicsBody:
										ImGui::OpenPopup(physicsBodyPopup);
										break;

									case ECS::ECSComponentType::NavMeshAgent:
									{
										const ECS::ECSTransformComponent* transformComponent =
											SceneManagement::SceneManager::GetMainScene()->GetECSManager()->GetEntityComponents().Get(selectedEntityID).Read<ECS::ECSTransformComponent>(ECSComponentType::Transform);

										if (transformComponent != nullptr)
										{
											newComponent = SceneManagement::SceneManager::GetMainScene()->GetECSManager()->CreateComponent<ECSNavMeshAgentComponent>(ECSComponentType::NavMeshAgent,
												transformComponent->position, 3.5f, 3.5f);

											ECS::ECSLocomotionComponent* locomotionComponent =
												SceneManagement::SceneManager::GetMainScene()->GetECSManager()->GetEntityComponents().Get(selectedEntityID).Write<ECS::ECSLocomotionComponent>(ECSComponentType::Locomotion);

											if (locomotionComponent == nullptr)
											{
												locomotionComponent = SceneManagement::SceneManager::GetMainScene()->GetECSManager()->CreateComponent<ECSLocomotionComponent>(ECSComponentType::Locomotion,
													5.0f, 3.5f, 3.5f);
												SceneManagement::SceneManager::GetMainScene()->GetECSManager()->AddComponentToEntity(selectedEntityID, locomotionComponent);
											}
										}
									}
										break;

									case ECS::ECSComponentType::CameraFollow:
									{
										ImGui::OpenPopup(followedEntityPopup);
									}
										break;
									case ECS::ECSComponentType::PlayerController:
										newComponent = SceneManagement::SceneManager::GetMainScene()->GetECSManager()->CreateComponent<ECSPlayerControllerComponent>(ECSComponentType::PlayerController);
										break;
									case ECS::ECSComponentType::HTNAgent:
									{
										ImGui::OpenPopup(htnAgentPopup);
									}
										break;
									}

									break;
								}
							}							
						}
					}
					
					Rendering::Mesh* mesh = EditorUtils::RenderResourceInspectorPopup<Rendering::Mesh>(meshPopup, ResourceManagement::ResourceType::Mesh);

					if (mesh)
					{	
						ECSTransformComponent* transform = selectedEntityComponents.Write<ECSTransformComponent>(ECSComponentType::Transform);
						if (transform == nullptr)
						{
							transform = SceneManagement::SceneManager::GetMainScene()->GetECSManager()->CreateComponent<ECS::ECSTransformComponent>(ECSComponentType::Transform,
								MathUtils::Vector3Zero,
								MathUtils::Vector3Zero,
								glm::vec3(1.0f));

							SceneManagement::SceneManager::GetMainScene()->GetECSManager()->AddComponentToEntity(selectedEntityID, transform);
						}

						ECS::ECSSkeletonComponent* skeletonComponent = nullptr;
						ECS::ECSMeshComponent* meshComponent = nullptr;
						mesh->GenerateMeshComponent(SceneManagement::SceneManager::GetMainScene()->GetECSManager(), true, transform, meshComponent, skeletonComponent);

						newComponent = meshComponent;

						if (skeletonComponent) 
						{
							SceneManagement::SceneManager::GetMainScene()->GetECSManager()->AddComponentToEntity(selectedEntityID, skeletonComponent);
						}
					}

					if (ImGui::BeginPopupModal(animatorPopup, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
					{
						BL_HASHTABLE_FOR(Animation::AnimationManager::GetAnimators(), hashMapIterator)
						{							
							if (ImGui::Selectable(BL_STRINGID_TO_STR_C(hashMapIterator.GetValue()->GetResourcePath())))
							{
								newComponent = SceneManagement::SceneManager::GetMainScene()->GetECSManager()->CreateComponent<ECS::ECSAnimationComponent>(ECSComponentType::Animation,
									hashMapIterator.GetValue()->GetResourceID(),
									hashMapIterator.GetValue()->GetInitialAnimationID());
							}							
						}
						ImGui::EndPopup();
					}

					if (ImGui::BeginPopupModal(physicsBodyPopup, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
					{				
						Physics::PhysicsShape shape;
						for (int i = 0; i < 3; ++i)
						{
							if (ImGui::Selectable(physicsBodyShapes[i]))
							{
								shape = (Physics::PhysicsShape)i;

								newComponent = SceneManagement::SceneManager::GetMainScene()->GetECSManager()->CreateComponent<ECS::ECSPhysicsBodyComponent>(ECSComponentType::PhysicsBody,
									shape,
									Physics::PhysicsBodyType::Static,
									glm::vec3(1.0f),
									MathUtils::Vector3Zero,
									MathUtils::Vector3Zero,
									1.0f);
							}
						}
						
						ImGui::EndPopup();
					}

					if (ImGui::BeginPopupModal(followedEntityPopup, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
					{
						BL_DYNAMICARRAY_FOR(i, SceneManagement::SceneManager::GetMainScene()->GetECSManager()->GetEntities(), 0)
						{
							StringId entityName = SceneManagement::SceneManager::GetMainScene()->GetECSManager()->GetEntities()[i].GetEntityName();
							if (ImGui::Selectable(BL_STRINGID_TO_STR_C(entityName)))
							{
								const ECSEntityID chosenEntityID = SceneManagement::SceneManager::GetMainScene()->GetECSManager()->GetEntities()[i].GetEntityID();

								ECSTransformComponent* transform = selectedEntityComponents.Write<ECSTransformComponent>(ECSComponentType::Transform);
								const ECSTransformComponent* followedEntityTransform = SceneManagement::SceneManager::GetECSManager()->GetEntityComponents().Get(chosenEntityID).Read<ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform);

								const glm::vec3 followedEntityDirection = glm::normalize(MathUtils::GetTransformForwardDirection(followedEntityTransform->GetTransformMatrix()));
								const glm::vec3 cameraPosition = followedEntityTransform->position - (followedEntityDirection * 5.0f);

								transform->position = cameraPosition;
								transform->rotation = followedEntityTransform->rotation;


								newComponent = SceneManagement::SceneManager::GetMainScene()->GetECSManager()->CreateComponent<ECS::ECSCameraFollowComponent>(ECSComponentType::CameraFollow,
									chosenEntityID,
									glm::vec2(0.0f),
									5.0f,
									1.0f);
							}							
								
						}

						ImGui::EndPopup();
					}

					if (ImGui::BeginPopupModal(htnAgentPopup, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
					{
						DynamicArray<StringId> domainNames = DynamicArray<StringId>(AllocationType::Linear_Frame, AI::HTN::HTNManager::s_definedDomains.Size());
						BL_HASHMAP_FOR(AI::HTN::HTNManager::s_definedDomains, it)
						{
							domainNames.EmplaceBack(it.GetKey());
						}

						static int domainName_idx = 0;
						const char* domainName_value = BL_STRINGID_TO_STR_C(domainNames[domainName_idx]);
						ImGuiComboFlags flags = ImGuiComboFlags_PopupAlignLeft;

						static StringId domainID;

						if (ImGui::BeginCombo("Domain", domainName_value, flags))
						{
							for (int n = 0; n < domainNames.Size(); n++)
							{
								const bool is_selected = (domainName_idx == n);
								if (ImGui::Selectable(BL_STRINGID_TO_STR_C(domainNames[n]), is_selected))
								{
									domainID = domainNames[n];
									domainName_idx = n;
								}

								// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}

						DynamicArray<StringId> blackboardNames = DynamicArray<StringId>(AllocationType::Linear_Frame, AI::HTN::HTNManager::s_definedWorldStateBlackboards.Size());
						BL_HASHMAP_FOR(AI::HTN::HTNManager::s_definedWorldStateBlackboards, it)
						{
							blackboardNames.EmplaceBack(it.GetKey());
						}

						static StringId worldStateBlackboardID;

						static int blackboardName_idx = 0;
						const char* blackboardName_value = BL_STRINGID_TO_STR_C(blackboardNames[blackboardName_idx]);

						if (ImGui::BeginCombo("World State Blackboard", blackboardName_value, flags))
						{
							for (int n = 0; n < blackboardNames.Size(); n++)
							{
								const bool is_selected = (blackboardName_idx == n);
								if (ImGui::Selectable(BL_STRINGID_TO_STR_C(blackboardNames[n]), is_selected))
								{
									worldStateBlackboardID = blackboardNames[n];
									blackboardName_idx = n;
								}

								// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}

						if (domainID > 0 && worldStateBlackboardID > 0)
						{
							newComponent = SceneManagement::SceneManager::GetMainScene()->GetECSManager()->CreateComponent<ECS::ECSHTNAgentComponent>(ECSComponentType::HTNAgent,
								domainID, worldStateBlackboardID);
						}

						ImGui::EndPopup();
					}

					if (newComponent) 
					{
						SceneManagement::SceneManager::GetMainScene()->GetECSManager()->AddComponentToEntity(selectedEntityID, newComponent);
					}

					ImGui::EndPopup();
				}

				ImGui::Separator();			
				
				ImGui::Text("Entity ID: %i", selectedEntityID);

				ImGui::Separator();

				ECS::ECSComponentLookUp selectedEntityComponents;

				if (SceneManagement::SceneManager::GetMainScene()->GetECSManager()->GetEntityComponents().TryGet(selectedEntityID, selectedEntityComponents)) {

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
					MeshComponentInspector::OnImGuiRender(component, m_sceneHierarchyPanel);					
					break;

				case ECS::ECSComponentType::Skeleton:
					ComponentInspector::BeginComponentRender("Skeleton Component", ECS::ECSComponentType::Skeleton, m_sceneHierarchyPanel, GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f);
					ComponentInspector::EndComponentRender();
					break;

				case ECS::ECSComponentType::Locomotion:
					LocomotionComponentInspector::OnImGuiRender(component, m_sceneHierarchyPanel);
					break;

				case ECS::ECSComponentType::Animation:
					AnimationComponentInspector::OnImGuiRender(component, m_sceneHierarchyPanel);					
					break;

				case ECS::ECSComponentType::PhysicsBody:
					PhyisicsBodyComponentInspector::OnImGuiRender(component, m_sceneHierarchyPanel);
					break;

				case ECS::ECSComponentType::NavMeshAgent:
					NavMeshAgentComponentInspector::OnImGuiRender(component, m_sceneHierarchyPanel);
					break;

				case ECS::ECSComponentType::CameraFollow:
					CameraFollowInspector::OnImGuiRender(component, m_sceneHierarchyPanel);
					break;
				case ECS::ECSComponentType::PlayerController:
					ComponentInspector::BeginComponentRender("Player Controller", ECS::ECSComponentType::PlayerController, m_sceneHierarchyPanel, GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f);
					ComponentInspector::EndComponentRender();
					break;
				case ECS::ECSComponentType::HTNAgent:
					HTNAgentComponentInspector::OnImGuiRender(component, m_sceneHierarchyPanel);
					break;
				default:
					break;
				
			}
		}


	}
}
