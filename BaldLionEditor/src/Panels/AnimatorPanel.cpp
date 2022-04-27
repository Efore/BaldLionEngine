#include <BaldLion.h>
#include <imgui/imgui.h>
#include "AnimatorPanel.h"
#include "UtilsEditor.h"

namespace BaldLion {

	namespace Editor {

		void AnimatorPanel::OnImGuiRender()
		{
			ImGui::Begin("Animator");
			m_viewportFocused = ImGui::IsWindowFocused();

			if (ImGui::Button("Open Animator"))
			{
				ImGui::OpenPopup("load_animator");
			}

			Animator* animatorToLoad = UtilsEditor::RenderResourceInspectorPopup<Animator>("load_animator", ResourceManagement::ResourceType::Animator);

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
			ImGui::Text("Animations"); ImGui::NextColumn();
			ImGui::Separator();

			//Parameters
			{
				if (ImGui::Button("Add Parameter")) {
					ImGui::OpenPopup("add_parameter");
				}

				ImGui::BeginChild("parameter column");
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
						ImGui::Text(BL_STRINGID_TO_STR_C(it.GetKey())); ImGui::NextColumn();

						switch (it.GetValue().Type)
						{
						case AnimatorParameter::ValueType::Bool:
							ImGui::Text("Bool"); ImGui::NextColumn();
							ImGui::Text(it.GetValue().Value.boolean ? "True" : "False"); ImGui::NextColumn();
							break;

						case AnimatorParameter::ValueType::Int:
							ImGui::Text("Int"); ImGui::NextColumn();
							ImGui::Text("%d", it.GetValue().Value.integer); ImGui::NextColumn();
							break;

						case AnimatorParameter::ValueType::Float:
							ImGui::Text("Float"); ImGui::NextColumn();
							ImGui::Text("%f", it.GetValue().Value.floating); ImGui::NextColumn();
							break;

						default:
							ImGui::Text("Float"); ImGui::NextColumn();
							ImGui::Text("%f", it.GetValue().Value.floating); ImGui::NextColumn();
							break;
						}

						if (ImGui::Button("-")) {
							parametersToDestroy.PushBack(it.GetKey());
						}

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

				ImGui::Separator();
			}
			
			//Animations
			{
				if (ImGui::Button("Add animation"))
				{
					ImGui::OpenPopup("add_animation");
				}
				AnimationData* animationDataToLoad = UtilsEditor::RenderResourceInspectorPopup<AnimationData>("add_animation", ResourceManagement::ResourceType::Animation);

				if (animationDataToLoad != nullptr)
				{					
					m_currentAnimator->AddAnimation(animationDataToLoad);
				}

				ImGui::Separator();


			}

			ImGui::End();
		}
	}
}
