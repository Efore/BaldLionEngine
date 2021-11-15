#pragma once
#include "SceneHierarchyPanel.h"
#include "BaldLion/ECS/Components/ECSHierarchyComponent.h"
#include <BaldLion.h>
#include <imgui/imgui.h>

namespace BaldLion {

	namespace Editor {

		SceneHierarchyPanel::SceneHierarchyPanel(Scene* sceneContext) : m_sceneContext(sceneContext)
		{

		}

		void SceneHierarchyPanel::SetSceneContext(Scene* sceneContext)
		{
			m_sceneContext = sceneContext;
		}

		void SceneHierarchyPanel::OnImGuiRender()
		{
			ImGui::Begin("Scene Hierarchy");

			for (ui32 i = 0; i < m_sceneContext->GetECSManager()->GetEntities().Size(); ++i) 
			{				
				DrawEntityElement(m_sceneContext->GetECSManager()->GetEntities()[i], true);
			}

			ImGui::End();
		}

		void SceneHierarchyPanel::DrawEntityElement(const ECS::ECSEntity& entity, bool firstCall)
		{
			ImGuiTreeNodeFlags flags = (m_selectedEntityID == entity.GetEntityID() ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;			

			const ECS::ECSHierarchyComponent* hierarchyComponent = m_sceneContext->GetECSManager()->GetEntityComponents().Get(entity.GetEntityID()).Read<ECS::ECSHierarchyComponent>(ECS::ECSComponentID::Hierarchy);

			if (hierarchyComponent != nullptr && hierarchyComponent->hasParent && firstCall) 
			{
				return;
			}

			if (hierarchyComponent == nullptr || hierarchyComponent->childEntitiesIDs.Size() == 0) 
			{
				flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
				ImGui::TreeNodeEx((void*)(uint32_t)entity.GetEntityID(), flags, STRINGID_TO_STRING(entity.GetEntityName()));
			}
			else 
			{
				bool open = ImGui::TreeNodeEx((void*)(uint32_t)entity.GetEntityID(), flags, STRINGID_TO_STRING(entity.GetEntityName()));

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

	}
}
