#include "HTNPanel.h"
#include "EditorUtils.h"
#include "BaldLion/AI/HTN/HTNManager.h"

namespace BaldLion::Editor
{
	HTNPanel::HTNPanel() : EditorPanel("HTN")
	{
		

	}

	void HTNPanel::OnImGuiRender(float deltaTime)
	{
		ImGui::Begin(BL_STRINGID_TO_STR_C(m_panelName));
		m_panelID = ImGui::GetCurrentWindow()->ID;

		m_viewportFocused = ImGui::IsWindowFocused();
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("Tabs", tab_bar_flags))
		{

			ImGui::PushID(imguiID++);
			if (ImGui::BeginTabItem("Domains"))
			{
				RenderDomains();
				ImGui::EndTabItem();
			}
			ImGui::PopID();
			ImGui::PushID(imguiID++);
			if (ImGui::BeginTabItem("Tasks"))
			{
				RenderTasks();
				ImGui::EndTabItem();
			}
			ImGui::PopID();
			ImGui::PushID(imguiID++);
			if (ImGui::BeginTabItem("World State Blackboards"))
			{
				RenderWorldStateBlackboards();
				ImGui::EndTabItem();
			}
			ImGui::PopID();
			ImGui::EndTabBar();
		}
		if (ImGui::Button("Save"))
		{
			HTNManager::Serialize();
		}
		ImGui::End();
		imguiID = 0;
	}

	void HTNPanel::RenderDomains()
	{
		ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		ImGuiStyle& style = ImGui::GetStyle();
		float child_w = ImGui::GetContentRegionAvail().x - style.ItemSpacing.x;
		if (child_w < 1.0f)
			child_w = 1.0f;

		ImGui::BeginChild("##Domains", ImVec2(child_w, 200.0f), true, 0);

		static StringId node_clicked = 0;	

		BL_HASHTABLE_FOR(HTNManager::s_definedDomains, it)
		{
			if (node_clicked == it.GetKey())
			{
				base_flags |= ImGuiTreeNodeFlags_Selected;
			}
			else
			{
				base_flags &= ~ImGuiTreeNodeFlags_Selected;
			}

			HTNDomain& domain = it.GetValue();
			ImGui::TreeNodeEx((void*)(uint32_t)(node_clicked), base_flags, BL_STRINGID_TO_STR_C(it.GetKey()));

			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			{
				node_clicked = it.GetKey();
			}
		}		

		HTNDomain* domain = nullptr;
		if (HTNManager::s_definedDomains.TryGet(node_clicked, domain))
		{
			if (ImGui::Button("Remove selected"))
			{
				HTNManager::s_definedDomains.Remove(node_clicked);
				domain = nullptr;
			}
		}

		static char newDomainName[64];
		ImGui::InputText("New Domain Name", newDomainName, IM_ARRAYSIZE(newDomainName));
		if (strlen(newDomainName) > 0 && ImGui::Button("+"))
		{
			HTNManager::s_definedDomains.Emplace(BL_STRING_TO_STRINGID(newDomainName), {});
			memset(newDomainName, 0, IM_ARRAYSIZE(newDomainName));
		}

		ImGui::EndChild();

		ImGui::Separator();

		ImGui::BeginChild("##EditDomains", ImVec2(child_w, 200.0f), true, 0);

		domain = nullptr;
		if (HTNManager::s_definedDomains.TryGet(node_clicked,domain))
		{			
			RenderEditDomain(node_clicked, *domain);
		}
		ImGui::EndChild();
	}

	void HTNPanel::RenderEditDomain(StringId domainID, HTNDomain& domain)
	{
		ImGui::Text(BL_STRINGID_TO_STR_C(domainID));

		DynamicArray<StringId> tasksIDs = DynamicArray<StringId>(AllocationType::Linear_Frame, HTNManager::s_definedTasks.Size());

		BL_DYNAMICARRAY_FOREACH(HTNManager::s_definedTasks)
		{
			tasksIDs.EmplaceBack(HTNManager::s_definedTasks[i].taskID);
		}
			
		if (tasksIDs.Size() > 0)
		{
			static int item_current_idx = 0;
			const char* task_preview_value = BL_STRINGID_TO_STR_C(tasksIDs[item_current_idx]);

			ImGuiComboFlags flags = ImGuiComboFlags_PopupAlignLeft;

			if (ImGui::BeginCombo("Initial Task", task_preview_value, flags))
			{
				BL_DYNAMICARRAY_FOREACH(tasksIDs)
				{
					const bool is_selected = (item_current_idx == i);
					if (ImGui::Selectable(BL_STRINGID_TO_STR_C(tasksIDs[i]), is_selected))
					{
						domain.mainTask = i;
						item_current_idx = i;
					}

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
	}

	void HTNPanel::RenderTasks()
	{
		ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		ImGuiStyle& style = ImGui::GetStyle();
		float child_w = ImGui::GetContentRegionAvail().x - style.ItemSpacing.x;
		if (child_w < 1.0f)
			child_w = 1.0f;

		ImGui::BeginChild("##RenderTasks", ImVec2(child_w, 200.0f), true, 0);
		static int node_clicked = -1;
		
		BL_DYNAMICARRAY_FOREACH(HTNManager::s_definedTasks)
		{
			if (node_clicked == i)
			{
				base_flags |= ImGuiTreeNodeFlags_Selected;
			}
			else
			{
				base_flags &= ~ImGuiTreeNodeFlags_Selected;
			}

			HTNTask& task = HTNManager::s_definedTasks[i];
			ImGui::TreeNodeEx((void*)(uint32_t)(i), base_flags, BL_STRINGID_TO_STR_C(task.taskID));			
			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			{
				node_clicked = i;
			}
		}		

		static char newTaskName[64];
		ImGui::InputText("New Task Name", newTaskName, IM_ARRAYSIZE(newTaskName));
		if (strlen(newTaskName) > 0 && ImGui::Button("+"))
		{			
			HTNTask* newTask = HTNManager::s_definedTasks.EmplaceBack();		
			newTask->taskID = BL_STRING_TO_STRINGID(newTaskName);
			newTask->methods = DynamicArray<HTNMethod>(AllocationType::FreeList_Main, 8);
			newTask->effects = DynamicArray<HTNWorldStateEffect>(AllocationType::FreeList_Main, 8);

			memset(newTaskName, 0, IM_ARRAYSIZE(newTaskName));
		}

		ImGui::Separator();

		if (ImGui::Button("Remove selected"))
		{
			HTNTask& task = HTNManager::s_definedTasks[node_clicked];			

			BL_DYNAMICARRAY_FOREACH(task.methods)
			{
				task.methods[i].conditions.Delete();
				task.methods[i].subtasksIndices.Delete();
			}

			task.methods.Delete();
			task.effects.Delete();

			HTNManager::s_definedTasks.RemoveAt(node_clicked);

			//fixing methods after removal
			BL_DYNAMICARRAY_FOREACH(HTNManager::s_definedTasks)
			{
				HTNTask& task = HTNManager::s_definedTasks[i];
				for (ui32 j = 0; j < task.methods.Size(); ++j)
				{
					HTNMethod& method = task.methods[j];
					for (i32 k = method.subtasksIndices.Size() - 1; k >= 0; --k)
					{
						if (method.subtasksIndices[k] > node_clicked)
						{
							method.subtasksIndices[k] = method.subtasksIndices[k] - 1;
						}
						else if (method.subtasksIndices[k] == node_clicked)
						{
							method.subtasksIndices.RemoveAt(k);
						}
					}
				}
			}
			DynamicArray<StringId> keysToDelete = DynamicArray<StringId>(AllocationType::Linear_Frame, HTNManager::s_definedDomains.Size());

			BL_HASHTABLE_FOR(HTNManager::s_definedDomains, it)
			{
				if (it.GetValue().mainTask > node_clicked)
				{
					it.GetValue().mainTask--;					
				}
				else if (it.GetValue().mainTask == node_clicked)
				{
					keysToDelete.EmplaceBack(it.GetKey());
				}
			}

			BL_DYNAMICARRAY_FOREACH(keysToDelete)
			{
				HTNManager::s_definedDomains.Remove(keysToDelete[i]);
			}
			
			node_clicked = -1;
		}

		ImGui::EndChild();

		ImGui::Separator();

		ImGui::BeginChild("##EditTasks", ImVec2(child_w, 200.0f), true, 0);
		if (node_clicked > -1) 
		{
			HTNTask& task = HTNManager::s_definedTasks[node_clicked];
			ImGui::PushID(imguiID++);
			RenderEditTask(task);
			ImGui::PopID();
		}
		ImGui::EndChild();
	}

	void HTNPanel::RenderEditTask(HTNTask& task)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		float child_w = ImGui::GetContentRegionAvail().x - style.ItemSpacing.x;
		if (child_w < 1.0f)
			child_w = 1.0f;

		ImGui::BeginChild("##EditTask", ImVec2(child_w, 200.0f), true, 0);

		int taskType = (int)task.taskType;
		if (ImGui::Combo("Task Type", &taskType, "Primitive Task\0Compound Task"))
		{
			if (task.taskType != (HTNTask::TaskType)taskType)
			{
				//Cleaning if changed type
				BL_DYNAMICARRAY_FOREACH(task.methods)
				{
					task.methods[i].conditions.Clear();
					task.methods[i].subtasksIndices.Clear();
				}

				task.methods.Clear();

				task.taskType = (HTNTask::TaskType)taskType;
			}
		}		

		ImGui::Separator();

		if (task.taskType == HTNTask::PrimitiveTask)
		{
			const char* operatorTypes[] = {
				"Operator Move To",
				"Operator Play Animation"
			};

			int operator_type_idx = (int)task.taskOperatorType;
			const char* operator_preview_value = operatorTypes[operator_type_idx];
			ImGuiComboFlags flags = ImGuiComboFlags_PopupAlignLeft;

			if (ImGui::BeginCombo("Operator Type", operator_preview_value, flags))
			{
				for (int n = 0; n < IM_ARRAYSIZE(operatorTypes); n++)
				{
					const bool is_selected = (operator_type_idx == n);
					if (ImGui::Selectable(operatorTypes[n], is_selected))
					{
						task.taskOperatorType = (HTNOperatorType)n;
						operator_type_idx = n;
					}

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			//TODO Render operator type
		}
		else
		{
			ImGui::Text("Methods");
			//Methods
			BL_DYNAMICARRAY_FOREACH(task.methods)
			{
				ImGui::PushID(imguiID++);
				if (ImGui::Button("Remove"))
				{
					task.methods[i].conditions.Delete();
					task.methods[i].subtasksIndices.Delete();
					task.methods.RemoveAt(i);
					ImGui::PopID();
					break;
				}

				RenderMethod(task.methods[i]);
				ImGui::PopID();
				ImGui::Separator();
			}
			
			ImGui::PushID(imguiID++);
			if (ImGui::Button("+"))
			{
				HTNMethod* newMethod = task.methods.EmplaceBack();				
				newMethod->conditions = DynamicArray<HTNWorldStateCondition>(AllocationType::FreeList_Main, 8);
				newMethod->subtasksIndices = DynamicArray<ui32>(AllocationType::FreeList_Main, 8);
			}
			ImGui::PopID();
		}

		ImGui::Separator();
		ImGui::Text("Effects");

		BL_DYNAMICARRAY_FOREACH(task.effects)
		{
			HTNWorldStateEffect& effect = task.effects[i];
			ImGui::PushID(imguiID++);
			EditorUtils::DrawVariant(BL_STRINGID_TO_STR_C(effect.blackboardKey), effect.blackboardValue, true);
			ImGui::SameLine();

			if (ImGui::Button("-"))
			{
				task.effects.RemoveAt(i);
				ImGui::PopID();
				break;
			}
			ImGui::PopID();
		}

		static char blackboardKey[64];		
		static Variant blackboardValue;
		ImGui::InputText("Blackboard Key", blackboardKey, IM_ARRAYSIZE(blackboardKey));
		EditorUtils::DrawVariant("", blackboardValue, false);
		ImGui::SameLine();
		ImGui::PushID(imguiID++);
		if (strlen(blackboardKey) > 0 && ImGui::Button("+"))
		{
			HTNWorldStateEffect* newEffect = task.effects.EmplaceBack();
			newEffect->blackboardKey = BL_STRING_TO_STRINGID(blackboardKey);
			newEffect->blackboardValue = blackboardValue;
			
			memset(blackboardKey, 0, IM_ARRAYSIZE(blackboardKey));
			memset(&blackboardValue, 0, sizeof(blackboardValue));
		}
		ImGui::PopID();

		ImGui::EndChild();
	}

	void HTNPanel::RenderMethod(HTNMethod& method)
	{		
		ImGui::Text("Conditions");		

		BL_DYNAMICARRAY_FOREACH(method.conditions)
		{
			ImGui::Separator();
			ImGui::PushID(imguiID++);
			ImGui::Text(BL_STRINGID_TO_STR_C(method.conditions[i].blackboardKey));

			int comparisonType = (int)method.conditions[i].comparisonType;
			ImGui::Combo("##ComparisonType", &comparisonType, "<\0<=\0==\0!=\0>=\0>");
			method.conditions[i].comparisonType = (VariantComparisonType)comparisonType;						

			EditorUtils::DrawVariant("", method.conditions[i].value, true);
			ImGui::SameLine();

			if (ImGui::Button("-"))
			{
				method.conditions.RemoveAt(i);
				ImGui::PopID();
				break;
			}
			ImGui::PopID();
		}

		static char blackboardKey[64];
		static Variant blackboardValue;
		static VariantComparisonType comparisonType = VariantComparisonType::Equal;

		ImGui::PushID(imguiID++);

		ImGui::InputText("##", blackboardKey, IM_ARRAYSIZE(blackboardKey));

		int comparisonInt = (int)comparisonType;

		ImGui::Combo("##NewComparisonType", &comparisonInt, "<\0<=\0==\0!=\0>=\0>");		
		comparisonType = (VariantComparisonType)comparisonInt;		

		EditorUtils::DrawVariant("", blackboardValue, false);
		ImGui::SameLine();
		if (strlen(blackboardKey) > 0 && ImGui::Button("+"))
		{
			HTNWorldStateCondition* newCondition = method.conditions.EmplaceBack();
			newCondition->blackboardKey = BL_STRING_TO_STRINGID(blackboardKey);
			newCondition->comparisonType = comparisonType;
			newCondition->value = blackboardValue;
						
			memset(blackboardKey, 0, IM_ARRAYSIZE(blackboardKey));
			memset(&blackboardValue, 0, sizeof(blackboardValue));
			comparisonType = VariantComparisonType::Equal;
		}

		ImGui::PopID();

		ImGui::Separator();
		ImGui::Separator();

		ImGui::PushID(imguiID++);

		ImGui::Text("Subtasks");

		BL_DYNAMICARRAY_FOREACH(method.subtasksIndices)
		{
			ImGui::Separator();
			ImGui::PushID(imguiID++);
			const HTNTask& subtask = HTNManager::s_definedTasks[method.subtasksIndices[i]];

			ImGui::Text(BL_STRINGID_TO_STR_C(subtask.taskID));

			ImGui::SameLine();

			if (ImGui::Button("-"))
			{
				method.subtasksIndices.RemoveAt(i);
				ImGui::PopID();
				break;
			}
			ImGui::PopID();
		}

		DynamicArray<StringId> tasksIDs = DynamicArray<StringId>(AllocationType::Linear_Frame, HTNManager::s_definedTasks.Size());

		BL_DYNAMICARRAY_FOREACH(HTNManager::s_definedTasks)
		{
			tasksIDs.EmplaceBack(HTNManager::s_definedTasks[i].taskID);
		}

		if (tasksIDs.Size() > 0)
		{
			static int item_current_idx = 0;
			const char* task_preview_value = BL_STRINGID_TO_STR_C(tasksIDs[item_current_idx]);

			ImGuiComboFlags flags = ImGuiComboFlags_PopupAlignLeft;

			if (ImGui::BeginCombo("##TasksIDs", task_preview_value, flags))
			{
				BL_DYNAMICARRAY_FOREACH(tasksIDs)
				{
					const bool is_selected = (item_current_idx == i);
					if (ImGui::Selectable(BL_STRINGID_TO_STR_C(tasksIDs[i]), is_selected))
					{
						item_current_idx = i;
					}

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::SameLine();
			if (ImGui::Button("+"))
			{
				method.subtasksIndices.EmplaceBack(item_current_idx);
				item_current_idx = 0;
			}
		}

		ImGui::PopID();
	}

	void HTNPanel::RenderWorldStateBlackboards()
	{
		ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
				
		ImGuiStyle& style = ImGui::GetStyle();
		float child_w = ImGui::GetContentRegionAvail().x - style.ItemSpacing.x;
		if (child_w < 1.0f)
			child_w = 1.0f;

		ImGui::BeginChild("##RenderWorldStateBlackboards", ImVec2(child_w, 200.0f), true, 0);
		static StringId node_clicked = 0;
		
		BL_HASHMAP_FOR(HTNManager::s_definedWorldStateBlackboards,it)
		{
			if (node_clicked == it.GetKey())
			{
				base_flags |= ImGuiTreeNodeFlags_Selected;
			}
			else
			{
				base_flags &= ~ImGuiTreeNodeFlags_Selected;
			}

			HTNWorldStateBlackboard& blackboard = it.GetValue();
			ImGui::TreeNodeEx((void*)(intptr_t)it.GetKey(), base_flags, BL_STRINGID_TO_STR_C(it.GetKey()));
			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			{
				node_clicked = it.GetKey();
			}
		}			

		HTNWorldStateBlackboard* blackboard = nullptr;
		if (HTNManager::s_definedWorldStateBlackboards.TryGet(node_clicked, blackboard))
		{
			if (ImGui::Button("Remove Selected WorldStateBlackboard"))
			{
				blackboard->Delete();
				HTNManager::s_definedWorldStateBlackboards.Remove(node_clicked);
				node_clicked = 0;
			}
		}

		static char newBlackboardName[64];
		ImGui::InputText("New WorldStateBlackboard Name", newBlackboardName, IM_ARRAYSIZE(newBlackboardName));
		if (strlen(newBlackboardName) > 0 && ImGui::Button("+"))
		{		
			const StringId blackboardName = BL_STRING_TO_STRINGID(newBlackboardName);
			HTNManager::s_definedWorldStateBlackboards.Emplace(blackboardName, HashTable<StringId, Variant>(AllocationType::FreeList_Main,16));
			memset(newBlackboardName, 0, 64);
		}

		ImGui::EndChild();

		ImGui::Separator();		

		ImGui::BeginChild("##Blackboards", ImVec2(child_w, 200.0f), true, 0);

		blackboard = nullptr;
		if(HTNManager::s_definedWorldStateBlackboards.TryGet(node_clicked, blackboard))
		{			
			DynamicArray<StringId> blackboardKeysToRemove(AllocationType::Linear_Frame, blackboard->Size());
			BL_HASHTABLE_FOR(*blackboard, it)
			{
				ImGui::PushID(imguiID++);
				EditorUtils::DrawVariant(BL_STRINGID_TO_STR_C(it.GetKey()), it.GetValue(),true);
				ImGui::SameLine();
				if (ImGui::Button("-"))
				{
					blackboard->Remove(it.GetKey());
					ImGui::PopID();
					break;
				}
				ImGui::PopID();
			}

			static char blackboardKey[64];
			static Variant blackboardValue;

			ImGui::PushID(imguiID++);

			ImGui::InputText("##BlackboardKey", blackboardKey, IM_ARRAYSIZE(blackboardKey));			
			EditorUtils::DrawVariant("", blackboardValue, false);
			ImGui::SameLine();

			if (strlen(blackboardKey) > 0 && ImGui::Button("+"))
			{
				blackboard->Emplace(BL_STRING_TO_STRINGID(blackboardKey), blackboardValue);
				memset(blackboardKey, 0, 64);
				memset(&blackboardValue, 0, sizeof(blackboardValue));
			}

			ImGui::PopID();

		}

		ImGui::EndChild();
	}

}
