#pragma once
#include "SceneHierarchyPanel.h"
#include "EditorUtils.h"

#include <BaldLion.h>
#include <imgui/imgui.h>

namespace BaldLion {

	namespace Editor {

		SceneHierarchyPanel::SceneHierarchyPanel() : EditorPanel("Scene Hierarchy"){}

		SceneHierarchyPanel::SceneHierarchyPanel(Scene* sceneContext) : EditorPanel("Scene Hierarchy"), m_selectedEntityID(0)
		{

		}

		void SceneHierarchyPanel::OnImGuiRender(float deltaTime)
		{			
			ImGui::Begin(BL_STRINGID_TO_STR_C(m_panelName));
			m_panelID = ImGui::GetCurrentWindow()->ID;

			m_viewportFocused = ImGui::IsWindowFocused();

			if (!SceneManagement::SceneManager::GetMainScene())
			{
				m_selectedEntityID = 0;
				return;
			}

			if (ImGui::Button("Add Entity"))
			{
				ImGui::OpenPopup("Create Entity");
			}

			ImGui::Separator();

			static bool isStatic = false;

			if (ImGui::Button("Create Static Entity from Model"))
			{
				isStatic = true;
				ImGui::OpenPopup("create_model_entity");
			}

			if (ImGui::Button("Create Static Entity from Mesh"))
			{
				isStatic = true;
				ImGui::OpenPopup("create_mesh_entity");
			}

			ImGui::Separator();			

			if (ImGui::Button("Create Dynamic Entity from Model"))
			{
				isStatic = false;
				ImGui::OpenPopup("create_model_entity");
			}

			if (ImGui::Button("Create Dynamic Entity from Mesh"))
			{
				isStatic = false;
				ImGui::OpenPopup("create_mesh_entity");
			}

			ImGui::Separator();

			if (ImGui::BeginPopupModal("Create Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				static char entityName[64] = "";
				BL_IMGUI_LEFT_LABEL(ImGui::InputText, "Entity Name", entityName, 64);				

				if (entityName != "" && ImGui::Button("Create")) 
				{
					SceneManagement::SceneManager::GetMainScene()->GetECSManager()->AddEntity(entityName);
					ImGui::CloseCurrentPopup();
				}				

				ImGui::SameLine();

				if (ImGui::Button("Close")) 
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			{
				Rendering::Model* model = EditorUtils::RenderResourceInspectorPopup<Rendering::Model>("create_model_entity", ResourceManagement::ResourceType::Model);
				
				if (model)
				{
					model->GenerateEntities(SceneManagement::SceneManager::GetMainScene()->GetECSManager(), isStatic);					
				}				
			}

			{
				Rendering::Mesh* mesh = EditorUtils::RenderResourceInspectorPopup<Rendering::Mesh>("create_mesh_entity", ResourceManagement::ResourceType::Mesh);
				
				if (mesh)
				{				
					mesh->GenerateEntity(SceneManagement::SceneManager::GetMainScene()->GetECSManager(), isStatic);				
				}
			}

			BL_DYNAMICARRAY_FOR(i, SceneManagement::SceneManager::GetMainScene()->GetECSManager()->GetEntities(), 0)
			{				
				bool selectedThisFrame = false;
				DrawEntityElement(SceneManagement::SceneManager::GetMainScene()->GetECSManager()->GetEntities()[i], true, selectedThisFrame);
			}

			ImGui::End();
		}

		void SceneHierarchyPanel::OnKeyPressed(int keyCode)
		{

			if (!SceneManagement::SceneManager::GetMainScene())
			{
				return;
			}

			if (m_viewportFocused) {

				switch (keyCode)
				{

				case BL_KEY_DELETE:

					if (m_selectedEntityID > 0)
					{
						SceneManagement::SceneManager::GetMainScene()->GetECSManager()->RemoveEntity(m_selectedEntityID);
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
						DrawEntityElement(*SceneManagement::SceneManager::GetMainScene()->GetECSManager()->GetEntityMap().Get(entity.GetChildrenIDs()[i]), false, selectedThisFrame);
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
