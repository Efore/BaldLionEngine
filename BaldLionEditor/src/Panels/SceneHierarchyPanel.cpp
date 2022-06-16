#pragma once
#include "SceneHierarchyPanel.h"
#include "UtilsEditor.h"

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
			
			m_viewportFocused = ImGui::IsWindowFocused();

			if (ImGui::Button("Add Entity"))
			{
				ImGui::OpenPopup("Create Entity");
			}

			ImGui::SameLine();

			if (ImGui::Button("Create Entity from Model"))
			{
				ImGui::OpenPopup("create_model_entity");
			}

			ImGui::Separator();			

			if (ImGui::BeginPopupModal("Create Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				static char entityName[64] = "";
				IMGUI_LEFT_LABEL(ImGui::InputText, "Entity Name", entityName, 64);				

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

			Rendering::Model* model = UtilsEditor::RenderResourceInspectorPopup<Rendering::Model>("create_model_entity", ResourceManagement::ResourceType::Model);
			
			if (model)
			{
				model->GenerateEntities(m_sceneContext->GetECSManager(), true);
			}

			BL_DYNAMICARRAY_FOR(i, m_sceneContext->GetECSManager()->GetEntities(), 0)			
			{				
				bool selectedThisFrame = false;
				DrawEntityElement(m_sceneContext->GetECSManager()->GetEntities()[i], true, selectedThisFrame);
			}

			ImGui::End();
		}

		void SceneHierarchyPanel::OnKeyPressed(int keyCode)
		{
			if (m_viewportFocused) {

				switch (keyCode)
				{

				case BL_KEY_DELETE:

					if (m_selectedEntityID > 0)
					{
						m_sceneContext->GetECSManager()->RemoveEntity(m_selectedEntityID);
						m_selectedEntityID = 0;
					}
					break;

				default:
					break;
				}
			}
		}

		void SceneHierarchyPanel::DrawEntityElement(const ECS::ECSEntity& entity, bool firstCall, bool& selectedThisFrame)
		{
			ImGuiTreeNodeFlags flags = (m_selectedEntityID == entity.GetEntityID() ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;			

			if (entity.GetParentID() > 0 && firstCall)
			{
				return;
			}

			if (entity.GetChildrenIDs().Size() == 0)
			{
				flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
				ImGui::TreeNodeEx((void*)(uint32_t)entity.GetEntityID(), flags, BL_STRINGID_TO_STR_C(entity.GetEntityName()));

				if (ImGui::IsItemClicked())
				{
					m_selectedEntityID = entity.GetEntityID();
					selectedThisFrame = true;
				}
			}
			else 
			{
				bool open = ImGui::TreeNodeEx((void*)(uint32_t)entity.GetEntityID(), flags, BL_STRINGID_TO_STR_C(entity.GetEntityName()));
				bool selectNode = ImGui::IsItemClicked();
				bool selectedThisFrame = false;

				if (open) 
				{
					BL_DYNAMICARRAY_FOREACH(entity.GetChildrenIDs())
					{
						DrawEntityElement(*m_sceneContext->GetECSManager()->GetEntityMap().Get(entity.GetChildrenIDs()[i]), false, selectedThisFrame);
					}	
					ImGui::TreePop();
				}

				if(!selectedThisFrame && selectNode)
				{
					m_selectedEntityID = entity.GetEntityID();
					selectedThisFrame = true;
				}
			}		
		}

	}
}
