#pragma once
#include "EditorViewportPanel.h"

#include "ImGuizmo.h"
#include "EditorUtils.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <BaldLion.h>

namespace BaldLion {

	namespace Editor {

		EditorViewportPanel::EditorViewportPanel() : ViewportPanel(nullptr, "Viewport"), m_viewportFocused(false), m_imGuizmoOperation(ImGuizmo::OPERATION::TRANSLATE)
		{

		}

		EditorViewportPanel::~EditorViewportPanel()
		{

		}

		void EditorViewportPanel::OnImGuiRender(float deltaTime)
		{		
			ImGui::Begin(BL_STRINGID_TO_STR_C(m_panelName));
			m_panelID = ImGui::GetCurrentWindow()->ID;

			ImGui::SameLine();
			ImGui::PushID(BL_STRING_TO_STRINGID("Manipulate Translation"));
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0.8f, 0.2f, 0.2f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0.8f, 0.3f, 0.3f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0.8f, 0.4f, 0.4f));
			if (ImGui::Button("T")) {
				m_imGuizmoOperation = 0;
			}
			ImGui::PopStyleColor(3);
			ImGui::PopID();

			ImGui::SameLine();
			ImGui::PushID(BL_STRING_TO_STRINGID("Manipulate Rotation"));
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0.2f, 0.8f, 0.2f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0.3f, 0.8f, 0.3f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0.4f, 0.8f, 0.4f));
			if (ImGui::Button("R")) {
				m_imGuizmoOperation = 1;
			}
			ImGui::PopStyleColor(3);
			ImGui::PopID();

			ImGui::SameLine();
			ImGui::PushID(BL_STRING_TO_STRINGID("Manipulate Scale"));
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0.2f, 0.2f, 0.8f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0.3f, 0.3f, 0.8f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0.4f, 0.4f, 0.8f));
			if (ImGui::Button("S")) {
				m_imGuizmoOperation = 2;
			}
			ImGui::PopStyleColor(3);
			ImGui::PopID();

			m_viewportFocused = ImGui::IsWindowFocused();
     
			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			if (m_viewportSize != glm::vec2{ viewportPanelSize.x, viewportPanelSize.y })
			{
				m_viewportSize = { viewportPanelSize.x, viewportPanelSize.y };
				Renderer::GetFrameBuffer()->Resize((ui32)viewportPanelSize.x, (ui32)viewportPanelSize.y);
			}

			ImGui::Image((void*)Renderer::GetFrameBuffer()->GetColorAttachmentID(), ImVec2{ m_viewportSize.x , m_viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

			//Gizmos
			ECS::ECSEntityID selectedEntityID = m_sceneHierarchyPanel->GetSelectedEntityID();
			if (selectedEntityID < 0)
			{
				ImGui::End();
				return;
			}

			ECS::ECSComponentLookUp selectedEntityComponents;			
			
			glm::mat4 cameraView = glm::inverse(ECS::SingletonSystems::CameraSystem::GetMainCameraTransform()->GetTransformMatrix());
			const ECS::ECSProjectionCameraComponent* mainCamera = ECS::SingletonSystems::CameraSystem::GetMainCamera();

			glm::mat4 cameraProjection = glm::perspective(glm::radians(mainCamera->fov), mainCamera->width / mainCamera->height, mainCamera->nearPlane, mainCamera->farPlane);

			ImGuizmo::SetOrthographic(false);

			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

			if (SceneManager::GetECSManager()->GetEntityComponents().TryGet(selectedEntityID, selectedEntityComponents))
			{
				ECS::ECSTransformComponent* entityTransformComponent = selectedEntityComponents.Write<ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform);

				if (entityTransformComponent)
				{	
					ECS::ECSEntity* entity = SceneManager::GetECSManager()->GetEntityMap().Get(selectedEntityID);

					const glm::mat4 preManipulationMat = entityTransformComponent->GetTransformMatrix();
					glm::mat4 manipulationMat = preManipulationMat;
					
					ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), (ImGuizmo::OPERATION)m_imGuizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(manipulationMat));

					if (ImGuizmo::IsUsing())
					{
						if (!m_isManipulatingGizmo)
						{
							m_staticSelectedLastTransform = preManipulationMat;
							m_isManipulatingGizmo = true;
						}

						glm::vec3 translation, rotation, scale;
						MathUtils::DecomposeTransformMatrix(manipulationMat, translation, rotation, scale);

						glm::vec3 deltaRotation = rotation - entityTransformComponent->rotation;
						
						entityTransformComponent->position = translation;
						entityTransformComponent->rotation += deltaRotation;
						entityTransformComponent->scale = scale;	
						
						m_isManipulatingGizmo = false;
						SceneManager::GetECSManager()->MarkEntityAsChangedInHierarchy(selectedEntityID);						
					}
										
				}
			}	
			else 
			{
				m_isManipulatingGizmo = false;
			}
			
			ImGui::End();			
		
			HandleInput();			
		}

		void EditorViewportPanel::HandleInput()
		{
			if (!m_viewportFocused)
				return;

			if (!BaldLion::Input::PlatformInput::IsMouseButtonPress(BL_MOUSE_BUTTON_2))
			{
				if (BaldLion::Input::PlatformInput::IsKeyPressed(BL_KEY_Q))
				{
					m_imGuizmoOperation = 0;
				}
				else if (BaldLion::Input::PlatformInput::IsKeyPressed(BL_KEY_W))
				{
					m_imGuizmoOperation = 1;
				}
				else if (BaldLion::Input::PlatformInput::IsKeyPressed(BL_KEY_E))
				{
					m_imGuizmoOperation = 2;
				}
			}
			
			if (!m_isManipulatingGizmo && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				//CheckSelectEntity();
			}
	
		}		

		void EditorViewportPanel::CheckSelectEntity()
		{			
			glm::vec2 mouseInWindow;
			if (EditorUtils::GetMouseRelativePosInWindow(m_panelID, mouseInWindow))
			{	
				const glm::vec3 rayOrigin = ECS::SingletonSystems::CameraSystem::GetMainCameraTransform()->position;

				const glm::vec4 rayClip = glm::vec4(mouseInWindow.x, mouseInWindow.y, 1.0f, 1.0f);

				glm::vec3 rayDirection = glm::inverse(ECS::SingletonSystems::CameraSystem::GetMainCamera()->viewProjectionMatrix) * rayClip;
				rayDirection = glm::normalize(rayDirection);

				//Renderer::DrawDebugLine(rayOrigin, rayDirection * 5000.0f, glm::vec3(1.0f, 0.0f, 0.0f), false, 5000);

				ECS::ECSEntityID closestEntityID = 0;
				float sqrClosestDistance = FLT_MAX;
				
				BL_HASHTABLE_FOR(SceneManager::GetECSManager()->GetEntityComponents(), it)
				{
					const ECS::ECSMeshComponent* meshComponent = it.GetValue().Read<ECS::ECSMeshComponent>(ECS::ECSComponentType::Mesh);

					if (meshComponent != nullptr)
					{
						const ECS::ECSTransformComponent* transform = it.GetValue().Read< ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform);

						const BoundingBox meshAABB = GeometryUtils::GetAABB(meshComponent->localBoundingBox, transform->GetTransformMatrix());												

						if (ECS::SingletonSystems::CameraSystem::IsAABBVisible(meshAABB) && meshAABB.IsIntersectedByRayFast(rayOrigin, rayDirection))
						{			
							float sqrDistance = glm::length2(meshAABB.center - rayOrigin);
							if (sqrDistance < sqrClosestDistance)
							{
								sqrClosestDistance = sqrDistance;
								closestEntityID = it.GetKey();
							}
						}
					}
				}

				if (closestEntityID > 0)
				{
					ECS::ECSEntity* closestEntity = SceneManager::GetECSManager()->GetEntityMap().Get(closestEntityID);
					while (closestEntity->GetParentID() > 0)
					{
						closestEntity = SceneManager::GetECSManager()->GetEntityMap().Get(closestEntity->GetParentID());
						closestEntityID = closestEntity->GetEntityID();
					}
					m_sceneHierarchyPanel->SetSelectedEntityID(closestEntityID);
				}
				else {
					m_sceneHierarchyPanel->SetSelectedEntityID(0);
				}
			}
		}

	}
}
