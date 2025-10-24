#pragma once
#include "ComponentInspector.h"
#include "BaldLion/ECS/Components/ECSHTNAgentComponent.h"

namespace BaldLion
{
	namespace Editor {

		class HTNAgentComponentInspector : public ComponentInspector {

		public:

			static void OnImGuiRender(ECS::ECSComponent* component, SceneHierarchyPanel* sceneHierarchyPanel) {

				ECS::ECSHTNAgentComponent* htnAgentComponent = (ECS::ECSHTNAgentComponent*)component;

				ComponentInspector::BeginComponentRender("HTN Agent", component->GetComponentType(), sceneHierarchyPanel, (GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f) * 5.0f);

				if (AI::HTN::HTNManager::s_definedDomains.Size() > 0)
				{
					DynamicArray<StringId> domainNames = DynamicArray<StringId>(AllocationType::Linear_Frame, AI::HTN::HTNManager::s_definedDomains.Size());
					BL_HASHMAP_FOR(AI::HTN::HTNManager::s_definedDomains, it)
					{
						domainNames.EmplaceBack(it.GetKey());
					}

					static int domainName_idx = 0;

					if (domainName_idx >= domainNames.Size())
					{
						domainName_idx = 0;
					}

					const char* domainName_value = BL_STRINGID_TO_STR_C(domainNames[domainName_idx]);					

					if (ImGui::BeginCombo("Domain", domainName_value, ImGuiComboFlags_PopupAlignLeft))
					{
						for (int n = 0; n < domainNames.Size(); n++)
						{
							const bool is_selected = (domainName_idx == n);
							if (ImGui::Selectable(BL_STRINGID_TO_STR_C(domainNames[n]), is_selected))
							{
								htnAgentComponent->initialDomainID = domainNames[n];
								domainName_idx = n;
							}

							// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}

				}

				if (AI::HTN::HTNManager::s_definedWorldStateBlackboards.Size() > 0)
				{
					DynamicArray<StringId> blackboardNames = DynamicArray<StringId>(AllocationType::Linear_Frame, AI::HTN::HTNManager::s_definedWorldStateBlackboards.Size());
					BL_HASHMAP_FOR(AI::HTN::HTNManager::s_definedWorldStateBlackboards, it)
					{
						blackboardNames.EmplaceBack(it.GetKey());
					}

					static int blackboardName_idx = 0;

					if (blackboardName_idx >= blackboardNames.Size())
					{
						blackboardName_idx = 0;
					}

					const char* blackboardName_value = BL_STRINGID_TO_STR_C(blackboardNames[blackboardName_idx]);

					if (ImGui::BeginCombo("World State Blackboard", blackboardName_value, ImGuiComboFlags_PopupAlignLeft))
					{
						for (int n = 0; n < blackboardNames.Size(); n++)
						{
							const bool is_selected = (blackboardName_idx == n);
							if (ImGui::Selectable(BL_STRINGID_TO_STR_C(blackboardNames[n]), is_selected))
							{
								htnAgentComponent->worldStateBlackboardID = blackboardNames[n];
								blackboardName_idx = n;
							}

							// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
				}

				ComponentInspector::EndComponentRender();
			}

		};

	}
}
