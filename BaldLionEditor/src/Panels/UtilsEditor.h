#pragma once
#include "BaldLion/ResourceManagement/ResourceManager.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/glm.hpp>

namespace BaldLion
{
	namespace Editor
	{
		#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))

		namespace UtilsEditor
		{
			template<typename T>
			static T* RenderResourceInspectorPopup(const char* popupName, ResourceManagement::ResourceType resourceType)
			{
				static_assert(std::is_base_of<ResourceManagement::Resource, T>::value, "T must inherit from Resource");

				T* result = nullptr;

				if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
				{
					BL_HASHMAP_FOR(ResourceManagement::ResourceManager::GetResourceMap(), hashMapIterator)
					{
						if (hashMapIterator.GetValue()->GetResourceType() == resourceType)
						{
							if (ImGui::Selectable(BL_STRINGID_TO_STR_C(hashMapIterator.GetValue()->GetResourceID())))
							{
								result = (T*)hashMapIterator.GetValue();
							}
						}
					}

					ImGui::EndPopup();
				}

				return result;
			}

			static void DrawVec3Handler(char const* label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
			{
				ImGuiIO& io = ImGui::GetIO();
				auto boldFont = io.Fonts->Fonts[0];

				ImGui::PushID(label);

				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, columnWidth);
				ImGui::Text(label);
				ImGui::NextColumn();

				ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });

				float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
				ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
				ImGui::PushFont(boldFont);

				if (ImGui::Button("X", buttonSize))
				{
					values.x = resetValue;
				}

				ImGui::PopFont();
				ImGui::PopStyleColor(3);

				ImGui::SameLine();
				ImGui::DragFloat("##x", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
				ImGui::PopItemWidth();
				ImGui::SameLine();

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
				ImGui::PushFont(boldFont);

				if (ImGui::Button("Y", buttonSize))
				{
					values.y = resetValue;
				}

				ImGui::PopFont();
				ImGui::PopStyleColor(3);

				ImGui::SameLine();
				ImGui::DragFloat("##y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
				ImGui::PopItemWidth();
				ImGui::SameLine();

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
				ImGui::PushFont(boldFont);

				if (ImGui::Button("Z", buttonSize))
				{
					values.z = resetValue;
				}

				ImGui::PopFont();
				ImGui::PopStyleColor(3);

				ImGui::SameLine();
				ImGui::DragFloat("##z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
				ImGui::PopItemWidth();

				ImGui::PopStyleVar();

				ImGui::Columns(1);

				ImGui::PopID();
			}
		}
	}
}