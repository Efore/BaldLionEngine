#pragma once
#include "EditorViewportPanel.h"
#include <BaldLion.h>
#include <imgui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

namespace BaldLion {

	namespace Editor {

		EditorViewportPanel::EditorViewportPanel(SceneHierarchyPanel* sceneHierarchyPanel) : ViewportPanel(sceneHierarchyPanel), m_viewPortFocused(false), m_imGuizmoOperation(0)
		{

		}

		EditorViewportPanel::EditorViewportPanel() : m_viewPortFocused(false), m_imGuizmoOperation(ImGuizmo::OPERATION::TRANSLATE)
		{
		}

		void EditorViewportPanel::OnImGuiRender()
		{

			if (BaldLion::Input::IsKeyPressed(BL_KEY_Q)) 
			{
				m_imGuizmoOperation = 0;
			} 
			else if (BaldLion::Input::IsKeyPressed(BL_KEY_W))
			{
				m_imGuizmoOperation = 1;
			}
			else if (BaldLion::Input::IsKeyPressed(BL_KEY_E)) 
			{
				m_imGuizmoOperation = 2;
			}

			ImGui::Begin("Viewport");

			ImGui::SameLine();
			ImGui::PushID(STRING_TO_STRINGID("Manipulate Translation"));
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0.8f, 0.2f, 0.2f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0.8f, 0.3f, 0.3f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0.8f, 0.4f, 0.4f));
			if (ImGui::Button("T")) {
				m_imGuizmoOperation = 0;
			}
			ImGui::PopStyleColor(3);
			ImGui::PopID();

			ImGui::SameLine();
			ImGui::PushID(STRING_TO_STRINGID("Manipulate Rotation"));
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0.2f, 0.8f, 0.2f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0.3f, 0.8f, 0.3f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0.4f, 0.8f, 0.4f));
			if (ImGui::Button("R")) {
				m_imGuizmoOperation = 1;
			}
			ImGui::PopStyleColor(3);
			ImGui::PopID();

			ImGui::SameLine();
			ImGui::PushID(STRING_TO_STRINGID("Manipulate Scale"));
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0.2f, 0.2f, 0.8f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0.3f, 0.3f, 0.8f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0.4f, 0.4f, 0.8f));
			if (ImGui::Button("S")) {
				m_imGuizmoOperation = 2;
			}
			ImGui::PopStyleColor(3);
			ImGui::PopID();

			m_viewPortFocused = ImGui::IsWindowFocused();

			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			if (m_viewportSize != glm::vec2{ viewportPanelSize.x, viewportPanelSize.y })
			{
				m_viewportSize = { viewportPanelSize.x, viewportPanelSize.y };
				Renderer::GetFrameBuffer()->Resize((ui32)viewportPanelSize.x, (ui32)viewportPanelSize.y);
			}

			ImGui::Image((void*)Renderer::GetFrameBuffer()->GetColorAttachmentID(), ImVec2{ m_viewportSize.x , m_viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

			//Gizmos
			ECS::ECSEntityID selectedEntityID = m_sceneHierarchyPanel->GetSelectedEntityID();
			ECS::ECSComponentLookUp selectedEntityComponents;

			ECS::ECSTransformComponent*  cameraTransformComponent = ECS::SingletonComponents::ECSProjectionCameraSingleton::GetMainCameraTransform();
			const ECS::ECSProjectionCameraComponent* cameraComponent = ECS::SingletonComponents::ECSProjectionCameraSingleton::GetMainCamera();

			glm::mat4 cameraView = glm::inverse(cameraTransformComponent->GetTransformMatrix());
			const glm::mat4 cameraProjection = glm::perspective(glm::radians(cameraComponent->fov), cameraComponent->width / cameraComponent->height, cameraComponent->nearPlane, cameraComponent->farPlane);

			ImGuizmo::SetOrthographic(false);

			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

			if (SceneManager::GetECSManager()->GetEntityComponents().TryGet(selectedEntityID, selectedEntityComponents))
			{
				ECS::ECSTransformComponent* entityTransformComponent = selectedEntityComponents.Write<ECS::ECSTransformComponent>(ECS::ECSComponentID::Transform);
				if (entityTransformComponent)
				{
					glm::mat4 entityTransformMat = entityTransformComponent->GetTransformMatrix();

					if (cameraTransformComponent != entityTransformComponent)
					{
						ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), (ImGuizmo::OPERATION)m_imGuizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(entityTransformMat));

						if (ImGuizmo::IsUsing())
						{
							glm::vec3 translation, rotation, scale;
							MathUtils::DecomposeTransformMatrix(entityTransformMat, translation, rotation, scale);
							glm::vec deltaRotation = rotation - entityTransformComponent->rotation;

							entityTransformComponent->position = translation;
							entityTransformComponent->rotation += deltaRotation;
							entityTransformComponent->scale = scale;
						}						
					}
				}
			}
			
			ImGuizmo::ViewManipulate((float*)&cameraView, 1.0f, ImVec2{ ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - 128, ImGui::GetWindowPos().y + 10 }, ImVec2(128, 128), 0x10101010);
			
			


			ImGui::End();
		}

	}
}
