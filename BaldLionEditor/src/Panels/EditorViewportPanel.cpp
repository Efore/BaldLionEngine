#pragma once
#include "EditorViewportPanel.h"
#include <BaldLion.h>
#include <imgui/imgui.h>
#include "ImGuizmo.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

namespace BaldLion {

	namespace Editor {

		EditorViewportPanel::EditorViewportPanel(SceneHierarchyPanel* sceneHierarchyPanel) : ViewportPanel(sceneHierarchyPanel), m_viewportFocused(false), m_imGuizmoOperation(0)
		{
		
		}

		EditorViewportPanel::EditorViewportPanel() : m_viewportFocused(false), m_imGuizmoOperation(ImGuizmo::OPERATION::TRANSLATE)
		{

		}

		EditorViewportPanel::~EditorViewportPanel()
		{
			if (m_viewportCameraTransform) 
			{
				BaldLion::Memory::MemoryManager::Delete(m_viewportCameraTransform);
			}

			if (m_viewportCamera)
			{
				BaldLion::Memory::MemoryManager::Delete(m_viewportCamera);
			}
		}

		void EditorViewportPanel::SetupViewportCamera()
		{
			m_viewportCameraTransform = BaldLion::Memory::MemoryManager::New<ECS::ECSTransformComponent>("Editor camera transform", AllocationType::FreeList_ECS,
				glm::vec3(0, 5, 180),
				MathUtils::Vector3Zero,
				glm::vec3(1.0f));

			m_viewportCamera = BaldLion::Memory::MemoryManager::New<ECS::ECSProjectionCameraComponent>("Editor camera", AllocationType::FreeList_ECS,
				45.0f,
				(float)Application::GetInstance().GetWindow().GetWidth(),
				(float)Application::GetInstance().GetWindow().GetHeight(),
				0.1f,
				50000.0f);

			m_cameraMovementSpeed = 50.0f;
			m_cameraRotationSpeed = 1.0f;

			m_prevX = 0.0f;
			m_prevY = 0.0f;
			m_cameraYaw = 0.0f;
			m_cameraPitch = 0.0f;

			ECS::SingletonComponents::ECSProjectionCameraSingleton::Init();
			ECS::SingletonComponents::ECSProjectionCameraSingleton::SetMainCamera(m_viewportCamera, m_viewportCameraTransform);
		}

		void EditorViewportPanel::OnImGuiRender()
		{
			ImGui::Begin("Viewport");

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
			ECS::ECSComponentLookUp selectedEntityComponents;
			
			glm::mat4 cameraView = glm::inverse(m_viewportCameraTransform->GetTransformMatrix());
			const glm::mat4 cameraProjection = glm::perspective(glm::radians(m_viewportCamera->fov), m_viewportCamera->width / m_viewportCamera->height, m_viewportCamera->nearPlane, m_viewportCamera->farPlane);

			ImGuizmo::SetOrthographic(false);

			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

			if (SceneManager::GetECSManager()->GetEntityComponents().TryGet(selectedEntityID, selectedEntityComponents))
			{
				ECS::ECSTransformComponent* entityTransformComponent = selectedEntityComponents.Write<ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform);
				if (entityTransformComponent)
				{
					glm::mat4 entityTransformMat = entityTransformComponent->GetTransformMatrix();
					
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

					const ECS::ECSProjectionCameraComponent* projectionCameraComponent = selectedEntityComponents.Read<ECS::ECSProjectionCameraComponent>(ECS::ECSComponentType::ProjectionCamera);

					if (projectionCameraComponent)
					{
						const glm::mat4 viewMatrix = glm::inverse(entityTransformComponent->GetTransformMatrix());

						const glm::mat4 projectionMatrix = glm::perspective(glm::radians(projectionCameraComponent->fov), projectionCameraComponent->width / projectionCameraComponent->height, projectionCameraComponent->nearPlane, projectionCameraComponent->farPlane);

						const glm::mat4 frustrum = glm::inverse(projectionMatrix * viewMatrix);

						Renderer::DrawDebugFrustrum(frustrum, glm::vec3(0.3f, 1.0f, 0.3f), 0.0f, false);
					}
				}

			}
			
			//ImGuizmo::ViewManipulate((float*)&cameraView, 1.0f, ImVec2{ ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - 128, ImGui::GetWindowPos().y + 10 }, ImVec2(128, 128), 0x10101010);
			ImGui::End();			
		
			HandleInput();			
		}

		void EditorViewportPanel::HandleInput()
		{
			if (!m_viewportFocused)
				return;

			if (!BaldLion::Input::IsMouseButtonPress(BL_MOUSE_BUTTON_2))
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
			}

			MoveViewportCamera();			
		}

		void EditorViewportPanel::MoveViewportCamera()
		{
			BL_PROFILE_FUNCTION();

			glm::mat4 cameraMatrixTransform = m_viewportCameraTransform->GetTransformMatrix();

			glm::vec3 cameraMovement;

			CalculateCameraMovement(m_cameraMovementSpeed, cameraMatrixTransform, cameraMovement);
			CalculateCameraRotation(m_cameraMovementSpeed,m_prevX, m_prevY, m_cameraYaw, m_cameraPitch);

			m_viewportCameraTransform->position += cameraMovement;
			m_viewportCameraTransform->rotation = glm::vec3(glm::radians(m_cameraPitch), glm::radians(m_cameraYaw), 0.0f);

			cameraMatrixTransform = m_viewportCameraTransform->GetTransformMatrix();

			const glm::mat4 viewMatrix = glm::inverse(cameraMatrixTransform);
			const glm::mat4 projectionMatrix = glm::perspective(glm::radians(m_viewportCamera->fov), m_viewportCamera->width / m_viewportCamera->height, m_viewportCamera->nearPlane, m_viewportCamera->farPlane);

			m_viewportCamera->viewProjectionMatrix = projectionMatrix * viewMatrix;
		}

		void EditorViewportPanel::CalculateCameraMovement(const float cameraMovementSpeed, const glm::mat4& cameraTransform, glm::vec3& cameraMovement)
		{
			cameraMovement = glm::vec3(0, 0, 0);

			if (BaldLion::Input::IsMouseButtonPress(BL_MOUSE_BUTTON_2))
			{
				if (BaldLion::Input::IsKeyPressed(BL_KEY_W))
					cameraMovement -= MathUtils::GetTransformForwardDirection(cameraTransform) * Time::GetDeltaTime() * cameraMovementSpeed;
				else if (BaldLion::Input::IsKeyPressed(BL_KEY_S))
					cameraMovement += MathUtils::GetTransformForwardDirection(cameraTransform) * Time::GetDeltaTime() * cameraMovementSpeed;

				if (BaldLion::Input::IsKeyPressed(BL_KEY_A))
					cameraMovement -= MathUtils::GetTransformRightDirection(cameraTransform) * Time::GetDeltaTime() * cameraMovementSpeed;
				else if (BaldLion::Input::IsKeyPressed(BL_KEY_D))
					cameraMovement += MathUtils::GetTransformRightDirection(cameraTransform) * Time::GetDeltaTime() * cameraMovementSpeed;

				if (BaldLion::Input::IsKeyPressed(BL_KEY_LEFT_SHIFT))
					cameraMovement *= 2;
			}
		}

		void EditorViewportPanel::CalculateCameraRotation( const float cameraRotationSpeed, float& prevX, float& prevY, float& cameraYaw, float& cameraPitch)
		{
			if (BaldLion::Input::IsMouseButtonPress(BL_MOUSE_BUTTON_2))
			{
				float deltaX = BaldLion::Input::GetMouseX() - prevX;
				float deltaY = BaldLion::Input::GetMouseY() - prevY;

				cameraYaw -= deltaX * cameraRotationSpeed * Time::GetDeltaTime();
				cameraPitch -= deltaY * cameraRotationSpeed * Time::GetDeltaTime();
			}

			prevX = BaldLion::Input::GetMouseX();
			prevY = BaldLion::Input::GetMouseY();
		}
	}
}
