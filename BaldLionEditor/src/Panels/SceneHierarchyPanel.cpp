#pragma once
#include "SceneHierarchyPanel.h"
#include "EditorUtils.h"

#include <BaldLion.h>
#include <imgui/imgui.h>

namespace BaldLion {

	namespace Editor {

		SceneHierarchyPanel::SceneHierarchyPanel() : EditorPanel("Scene Hierarchy"){}

		SceneHierarchyPanel::SceneHierarchyPanel(Scene* sceneContext) : EditorPanel("Scene Hierarchy"), m_sceneContext(sceneContext), m_selectedEntityID(0)
		{

		}

		void SceneHierarchyPanel::SetSceneContext(Scene* sceneContext)
		{
			m_sceneContext = sceneContext;
		}

		void SceneHierarchyPanel::OnImGuiRender()
		{
			ImGui::Begin(BL_STRINGID_TO_STR_C(m_panelName));
			m_panelID = ImGui::GetCurrentWindow()->ID;

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

			if (ImGui::Button("Create Entity from Mesh"))
			{
				ImGui::OpenPopup("create_mesh_entity");
			}

			ImGui::Separator();			

			if (ImGui::BeginPopupModal("Create Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				static char entityName[64] = "";
				BL_IMGUI_LEFT_LABEL(ImGui::InputText, "Entity Name", entityName, 64);				

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

			Rendering::Model* model = EditorUtils::RenderResourceInspectorPopup<Rendering::Model>("create_model_entity", ResourceManagement::ResourceType::Model);
			
			if (model)
			{
				model->GenerateEntities(m_sceneContext->GetECSManager(), true);
			}

			Rendering::Mesh* mesh = EditorUtils::RenderResourceInspectorPopup<Rendering::Mesh>("create_mesh_entity", ResourceManagement::ResourceType::Mesh);

			if (mesh)
			{
				mesh->GenerateEntity(m_sceneContext->GetECSManager(), true);
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
