#pragma once
#include "SceneHierarchyPanel.h"
#include "BaldLion/ECS/Components/ECSHierarchyComponent.h"
#include <BaldLion.h>
#include <imgui/imgui.h>

namespace BaldLion {

	namespace Editor {

		SceneHierarchyPanel::SceneHierarchyPanel(Scene* sceneContext) : m_sceneContext(sceneContext), m_selectedEntityID(0)
		{

		}

		void SceneHierarchyPanel::SetSceneContext(Scene* sceneContext)
		{
			m_sceneContext = sceneContext;
		}

		void SceneHierarchyPanel::OnImGuiRender()
		{
			ImGui::Begin("Scene Hierarchy");
			
			if (ImGui::Button("Add Entity"))
			{
				ImGui::OpenPopup("Create Entity");
			}

			ImGui::Separator();			

			if (ImGui::BeginPopupModal("Create Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				static char entityName[64] = "";
				ImGui::InputText("Entity Name", entityName, 64);

				if (entityName != "" && ImGui::Button("Create")) 
				{
					m_sceneContext->GetECSManager()->AddEntity(entityName);
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				if (ImGui::Button("Close")) 
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}			

			for (ui32 i = 0; i < m_sceneContext->GetECSManager()->GetEntities().Size(); ++i) 
			{				
				DrawEntityElement(m_sceneContext->GetECSManager()->GetEntities()[i], true);
			}

			ImGui::End();

			HandleInput();
		}

		void SceneHierarchyPanel::DrawEntityElement(const ECS::ECSEntity& entity, bool firstCall)
		{
			ImGuiTreeNodeFlags flags = (m_selectedEntityID == entity.GetEntityID() ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;			

			const ECS::ECSHierarchyComponent* hierarchyComponent = m_sceneContext->GetECSManager()->GetEntityComponents().Get(entity.GetEntityID()).Read<ECS::ECSHierarchyComponent>(ECS::ECSComponentType::Hierarchy);

			if (hierarchyComponent != nullptr && hierarchyComponent->parentEntityID > 0 && firstCall) 
			{
				return;
			}

			if (hierarchyComponent == nullptr || hierarchyComponent->childEntitiesIDs.Size() == 0) 
			{
				flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
				ImGui::TreeNodeEx((void*)(uint32_t)entity.GetEntityID(), flags, STRINGID_TO_STR_C(entity.GetEntityName()));
			}
			else 
			{
				bool open = ImGui::TreeNodeEx((void*)(uint32_t)entity.GetEntityID(), flags, STRINGID_TO_STR_C(entity.GetEntityName()));

				if (open) 
				{
					for (ui32 i = 0; i < hierarchyComponent->childEntitiesIDs.Size(); ++i) 
					{
						DrawEntityElement(*m_sceneContext->GetECSManager()->GetEntityMap().Get(hierarchyComponent->childEntitiesIDs[i]), false);
					}	
					ImGui::TreePop();
				}
			}
			
			if (ImGui::IsItemClicked()) 
			{
				m_selectedEntityID = entity.GetEntityID();
			}		
		}

		void SceneHierarchyPanel::HandleInput()
		{
			if (m_selectedEntityID && BaldLion::Input::IsKeyPressed(BL_KEY_DELETE))
			{
				m_sceneContext->GetECSManager()->RemoveEntity(m_selectedEntityID);
				m_selectedEntityID = 0;
			}
		}

	}
}
