#include <BaldLion.h>
#include <imgui/imgui.h>
#include "AnimatorPanel.h"
#include "EditorUtils.h"
#include "ImNodesEz.h"

namespace BaldLion {

	namespace Editor {

		AnimatorPanel::AnimatorPanel() : EditorPanel("Animator")
		{
			static ImNodes::Ez::Context* context = ImNodes::Ez::CreateContext();
		}

		void AnimatorPanel::OnImGuiRender()
		{
			ImGui::Begin(BL_STRINGID_TO_STR_C(m_panelName));
			m_panelID = ImGui::GetCurrentWindow()->ID;

			m_viewportFocused = ImGui::IsWindowFocused();

			if (ImGui::Button("Open Animator"))
			{
				ImGui::OpenPopup("load_animator");
			}

			Animator* animatorToLoad = EditorUtils::RenderResourceInspectorPopup<Animator>("load_animator", ResourceManagement::ResourceType::Animator);

			if (animatorToLoad != nullptr)
			{
				m_currentAnimator = animatorToLoad;
			}

			if (m_currentAnimator != nullptr)
			{
				ImGui::Text("Current Animator: %s", BL_STRINGID_TO_STR_C(m_currentAnimator->GetResourcePath()));

				ImGui::SameLine();

				if (ImGui::Button("Save Changes"))
				{
					AnimationManager::SerializeAnimator(m_currentAnimator->GetResourceID());
				}
			}

			ImGui::Separator();

			if (m_currentAnimator == nullptr)
			{
				ImGui::End();
				return;
			}

			ImGui::Columns(2);

			ImGui::SetColumnWidth(0, 400);
			ImGui::Text("Parameters"); ImGui::NextColumn();
			ImGui::Text(""); ImGui::NextColumn();
			ImGui::Separator();

			//Parameters
			RenderParameters();
			
			ImGui::NextColumn();

			//Animations
			RenderAnimationNodes();
		
			ImGui::End();
		}

		void AnimatorPanel::RenderParameter(AnimatorParameter& parameter, StringId parameterName)
		{			
			ImGui::Text(BL_STRINGID_TO_STR_C(parameterName)); ImGui::NextColumn();

			switch (parameter.Type)
			{
			case AnimatorParameter::ValueType::Bool:
				ImGui::Text("Bool"); ImGui::NextColumn();
				IMGUI_LEFT_LABEL(ImGui::Checkbox, "", &(parameter.Value.boolean)); ImGui::NextColumn();
				break;

			case AnimatorParameter::ValueType::Int:
				ImGui::Text("Int"); ImGui::NextColumn();
				static int inputValue = parameter.Value.integer;
				IMGUI_LEFT_LABEL(ImGui::InputInt, "", &inputValue); ImGui::NextColumn();
				if (inputValue < 0) inputValue = 0;
				parameter.Value.integer = inputValue;
				break;

			case AnimatorParameter::ValueType::Float:
				ImGui::Text("Float"); ImGui::NextColumn();
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "", &parameter.Value.floating); ImGui::NextColumn();				
				break;

			default:
				ImGui::Text("Float"); ImGui::NextColumn();
				ImGui::Text("%f", parameter.Value.floating); ImGui::NextColumn();
				break;
			}			
		}

		void AnimatorPanel::RenderCondition(AnimatorCondition* condition)
		{
			ImGui::Text(BL_STRINGID_TO_STR_C(condition->ParameterAName)); ImGui::NextColumn();
			int selectIndex = (int)condition->Comparison;

			switch (condition->ParameterB.Type)
			{
			case AnimatorParameter::ValueType::Bool:				
				IMGUI_LEFT_LABEL(ImGui::Checkbox, "", &(condition->ParameterB.Value.boolean)); ImGui::NextColumn();
				break;

			case AnimatorParameter::ValueType::Int:			
			{
				ImGui::Combo("##", &selectIndex, "<\0<=\0==\0!=\0>=\0>"); ImGui::NextColumn();
				condition->Comparison = (AnimatorCondition::ComparisonType)selectIndex;

				int inputValue = condition->ParameterB.Value.integer;
				IMGUI_LEFT_LABEL(ImGui::InputInt, "##", &inputValue); ImGui::NextColumn();

				if (inputValue < 0) inputValue = 0;
				condition->ParameterB.Value.integer = inputValue;
			}
				break;

			case AnimatorParameter::ValueType::Float:
				ImGui::Combo("##", &selectIndex, "<\0<=\0==\0!=\0>=\0>"); ImGui::NextColumn();
				condition->Comparison = (AnimatorCondition::ComparisonType)selectIndex;

				IMGUI_LEFT_LABEL(ImGui::InputFloat, "##", &condition->ParameterB.Value.floating); ImGui::NextColumn();
				break;
			}
		}

		void AnimatorPanel::RenderParameters()
		{
			if (ImGui::Button("Add Parameter")) {
				ImGui::OpenPopup("add_parameter");
			}

			float ySize = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f * (m_currentAnimator->GetAllParameters().Size() + 4);
			ImGui::BeginChild("parameter column", ImVec2(0, ySize));
			{

				ImGui::Columns(4);
				ImGui::SetColumnWidth(0, 150);
				ImGui::Text("Name"); ImGui::NextColumn();
				ImGui::Text("Type"); ImGui::NextColumn();
				ImGui::Text("Value"); ImGui::NextColumn();
				ImGui::NextColumn();
				ImGui::Separator();

				DynamicArray<StringId> parametersToDestroy(Memory::AllocationType::Linear_Frame, m_currentAnimator->GetAllParameters().Size());

				BL_HASHTABLE_FOR(m_currentAnimator->GetAllParameters(), it)
				{
					RenderParameter(it.GetValue(), it.GetKey());	

					if (ImGui::Button("-")) {
						parametersToDestroy.PushBack(it.GetKey());
					}

					ImGui::NextColumn();

					ImGui::Separator();
				}

				BL_DYNAMICARRAY_FOR(i, parametersToDestroy, 0)
				{
					m_currentAnimator->RemoveParameter(parametersToDestroy[i]);
				}
			}

			ImGui::EndChild();

			if (ImGui::BeginPopupModal("add_parameter", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				static char parameterName[64] = "";
				IMGUI_LEFT_LABEL(ImGui::InputText, "Parameter Name", parameterName, 64);

				ImGui::Separator();

				const char* parameterTypeNames[] = {
							"Int",
							"Float",
							"Bool"
				};

				static AnimatorParameter::ValueType parameterValueType = AnimatorParameter::ValueType::Int;

				for (int i = 0; i < IM_ARRAYSIZE(parameterTypeNames); i++)
				{
					if (ImGui::Button(parameterTypeNames[i]))
					{
						parameterValueType = (AnimatorParameter::ValueType)i;
					}
				}

				ImGui::Separator();

				static int integerValue = 0;
				static float floatValue = 0.0f;
				static bool boolValue = true;

				switch (parameterValueType)
				{
				case BaldLion::Animation::AnimatorParameter::ValueType::Int:
					IMGUI_LEFT_LABEL(ImGui::InputInt, "Value: ", &integerValue);
					break;

				case BaldLion::Animation::AnimatorParameter::ValueType::Float:
					IMGUI_LEFT_LABEL(ImGui::InputFloat, "Value: ", &floatValue);
					break;

				case BaldLion::Animation::AnimatorParameter::ValueType::Bool:
					IMGUI_LEFT_LABEL(ImGui::Checkbox, "Value: ", &boolValue);
					break;

				default:
					break;
				}

				ImGui::Separator();

				if (parameterName != "" && ImGui::Button("Create"))
				{
					AnimatorParameter parameter;
					parameter.Type = parameterValueType;

					switch (parameterValueType)
					{
					case BaldLion::Animation::AnimatorParameter::ValueType::Int:
						parameter.Value.integer = integerValue;
						break;
					case BaldLion::Animation::AnimatorParameter::ValueType::Float:
						parameter.Value.floating = floatValue;
						break;
					case BaldLion::Animation::AnimatorParameter::ValueType::Bool:
						parameter.Value.boolean = boolValue;
						break;
					default:
						break;
					}

					m_currentAnimator->AddParameter(BL_STRING_TO_STRINGID(parameterName), parameter);

					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				if (ImGui::Button("Close"))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}

		void AnimatorPanel::RenderAnimationNodes()
		{
			if (ImGui::Button("Add animation"))
			{
				ImGui::OpenPopup("add_animation");
			}

			AnimationClip* animationDataToLoad = EditorUtils::RenderResourceInspectorPopup<AnimationClip>("add_animation", ResourceManagement::ResourceType::Animation);

			if (animationDataToLoad != nullptr)
			{
				m_currentAnimator->AddAnimation(animationDataToLoad);
			}			
			ImGui::BeginChild("animations column");
			{
				ImGui::Columns(2);
				ImGui::Text("Animations"); ImGui::NextColumn();
				ImGui::Text("Transitions"); ImGui::NextColumn();
				ImGui::Separator();

				ui32 animationIndex = 0;
				BL_HASHTABLE_FOR(m_currentAnimator->GetAllAnimations(), it)
				{
					const char* initialAnimationPath = BL_STRINGID_TO_STR_C(it.GetValue()->GetResourcePath());
					ImGui::BulletText(initialAnimationPath);
					ImGui::NextColumn();

					DynamicArray<AnimatorTransition*>* transitions = m_currentAnimator->GetTransitionsOfAnimation(it.GetKey());

					if (transitions != nullptr)
					{
						DynamicArray<ui32> transitionIndexToRemove(Memory::AllocationType::Linear_Frame, transitions->Size());
						BL_DYNAMICARRAY_FOR(i, *transitions, 0)
						{
							const char* finalAnimationPath = BL_STRINGID_TO_STR_C(m_currentAnimator->GetAnimationClip((*transitions)[i]->GetFinalAnimationID())->GetResourcePath());
							std::string popupId = "Setup Transition to ";
							popupId.append(finalAnimationPath);

							if (ImGui::Button(finalAnimationPath))
							{
								ImGui::OpenPopup(popupId.c_str());
							}

							RenderSetupTransitionPopup(popupId.c_str(), initialAnimationPath, finalAnimationPath, (*transitions)[i]);

							ImGui::SameLine();
							if (ImGui::Button("-"))
							{
								transitionIndexToRemove.EmplaceBack(i);								
							}
						}

						BL_DYNAMICARRAY_FOR(i, transitionIndexToRemove, 0)
						{
							transitions->RemoveAt(transitionIndexToRemove[i]);
						}
					}

					std::string popupId = "Add Transition " + std::to_string(animationIndex);
					if (ImGui::Button(popupId.c_str()))
					{
						ImGui::OpenPopup(popupId.c_str());
					}

					RenderAddTransitionPopup(popupId.c_str(), initialAnimationPath, it.GetValue()->GetResourceID());

					ImGui::NextColumn();
					ImGui::Separator();

					animationIndex++;
				}
			}
			ImGui::EndChild();

			ImGui::Separator();
		}

		void AnimatorPanel::RenderAddTransitionPopup(const char* popupName, const char* initialAnimationPath, ui32 initialAnimationID)
		{
			if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Transition from %", initialAnimationPath);
				ImGui::Separator();
				
				static ui32 finalAnimationID = initialAnimationID;

				const char* combo_preview_value = finalAnimationID == initialAnimationID ? "" : BL_STRINGID_TO_STR_C(finalAnimationID);

				if (ImGui::BeginCombo("Final animation", combo_preview_value, ImGuiComboFlags_PopupAlignLeft))
				{
					BL_HASHTABLE_FOR(m_currentAnimator->GetAllAnimations(), it)
					{
						if(it.GetKey() == initialAnimationID)
							continue;

						const bool is_selected = (finalAnimationID == it.GetKey());

						if (ImGui::Selectable(BL_STRINGID_TO_STR_C(it.GetValue()->GetResourcePath()), is_selected))
						{
							finalAnimationID = it.GetKey();
						}

						// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				static float transitionExitTime = 0.0f;
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "Exit Time", &transitionExitTime);

				static float transitionTime = 0.0f;
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "Transition Time", &transitionTime);

				ImGui::Separator();

				if (ImGui::Button("Add"))
				{
					AnimatorTransition* transition = MemoryManager::New<AnimatorTransition>("Animator Transition", MemoryManager::GetAllocatorType(m_currentAnimator), initialAnimationID, finalAnimationID, transitionExitTime, transitionTime);
					m_currentAnimator->AddAnimationTransition(transition);

					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				if (ImGui::Button("Close"))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}

		void AnimatorPanel::RenderSetupTransitionPopup(const char* popupID, const char* initialAnimationPath, const char* finalAnimationPath, AnimatorTransition* transition)
		{
			if (ImGui::BeginPopupModal(popupID, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Transition %s to %s", initialAnimationPath, finalAnimationPath);
				ImGui::Separator();

				float transitionExitTime = transition->GetExitTime();
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "Exit Time", &transitionExitTime);
				transition->SetExitTime(transitionExitTime);

				float transitionTime = transition->GetTransitionTime();
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "Transition Time", &transitionTime);
				transition->SetTransitionTime(transitionTime);

				ImGui::Separator();

				std::string childId = "transition columns";
				childId.append(popupID);

				float ySize = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f * (transition->GetConditions().Size() + 4);
				ImGui::BeginChild(childId.c_str(), ImVec2(0, ySize));
				{
					ImGui::Columns(4);
					
					DynamicArray<ui32> parametersToDestroy(Memory::AllocationType::Linear_Frame, transition->GetConditions().Size());

					BL_DYNAMICARRAY_FOR(i, transition->GetConditions(), 0)
					{
						RenderCondition(&transition->GetConditions()[i]);

						if (ImGui::Button("-")) {
							parametersToDestroy.PushBack(i);
						}

						ImGui::NextColumn();

						ImGui::Separator();
					}

					BL_DYNAMICARRAY_FOR(i, parametersToDestroy, 0)
					{
						transition->RemoveCondition(parametersToDestroy[i]);
					}
				}

				ImGui::EndChild();

				ImGui::Separator();

				std::string popupID = "add_condition";
				childId.append(popupID);

				if (ImGui::Button("+"))
				{
					ImGui::OpenPopup(popupID.c_str());
				}

				RenderAddCondition(popupID.c_str(), transition);

				ImGui::Separator();

				if (ImGui::Button("Close"))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}

		void AnimatorPanel::RenderAddCondition(const char* popupID, AnimatorTransition* transition)
		{
			if (ImGui::BeginPopupModal(popupID, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				static StringId parameterName = 0;

				const char* combo_preview_value = parameterName != 0 ? BL_STRINGID_TO_STR_C(parameterName) : "";
				if (ImGui::BeginCombo("Condition Parameter", combo_preview_value, ImGuiComboFlags_PopupAlignLeft))
				{
					BL_HASHTABLE_FOR(m_currentAnimator->GetAllParameters(), it)
					{
						const bool is_selected = (parameterName == it.GetKey());

						if (ImGui::Selectable(BL_STRINGID_TO_STR_C(it.GetKey()), is_selected))
							parameterName = it.GetKey();

						// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				
				if (parameterName != 0)
				{
					AnimatorParameter parameter = m_currentAnimator->GetParameter(parameterName);
					AnimatorParameter parameterB;
					AnimatorCondition::ComparisonType comparison = AnimatorCondition::ComparisonType::Equal;

					static int selectIndex = 0;
					static int intValue = 0;
					static float floatValue = 0;

					switch (parameter.Type)
					{

					case AnimatorParameter::ValueType::Bool:
						
						ImGui::Combo("combo bool", &selectIndex, "False\0True");

						parameterB.Type = AnimatorParameter::ValueType::Bool;
						parameterB.Value.boolean = selectIndex;
						break;

					case AnimatorParameter::ValueType::Int:

						ImGui::Combo("combo int", &selectIndex, "<\0<=\0==\0!=\0>=\0>");
						comparison = (AnimatorCondition::ComparisonType)selectIndex;

						
						IMGUI_LEFT_LABEL(ImGui::InputInt, "Value", &intValue);

						parameterB.Type = AnimatorParameter::ValueType::Int;
						parameterB.Value.integer = intValue;

						break;

					case AnimatorParameter::ValueType::Float:

						ImGui::Combo("combo float", &selectIndex, "<\0<=\0==\0!=\0>=\0>");
						comparison = (AnimatorCondition::ComparisonType)selectIndex;
						
						IMGUI_LEFT_LABEL(ImGui::InputFloat, "Value", &floatValue);

						parameterB.Type = AnimatorParameter::ValueType::Float;
						parameterB.Value.floating = floatValue;

						break;

					default:
						break;
					}

					AnimatorCondition condition { comparison, parameterName , parameterB };

					if (ImGui::Button("Add"))
					{
						transition->AddCondition(condition);
						ImGui::CloseCurrentPopup();
					}
					ImGui::SameLine();

				}

				if (ImGui::Button("Close"))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}

	}
}
