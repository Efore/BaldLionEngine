#pragma once
#include "BaldLion/ResourceManagement/ResourceManager.h"
#include "BaldLion/SceneManagement/SceneManager.h"
#include "BaldLion/ECS/Components/ECSHierarchyComponent.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/Core/Variant.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/glm.hpp>

namespace BaldLion
{
	namespace Editor
	{
		#define BL_IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))

		namespace EditorUtils
		{
			const glm::vec3 ColorRed	= glm::vec3(1.0f, 0.0f, 0.0f);
			const glm::vec3 ColorGreen	= glm::vec3(0.0f, 1.0f, 0.0f);
			const glm::vec3 ColorBlue	= glm::vec3(0.0f, 0.0f, 1.0f);

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
								ImGui::CloseCurrentPopup();
							}
						}
					}

					ImGui::Separator();

					if (ImGui::Button("Close")) {
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

				return result;
			}
			
			static bool DrawVec3Handler(char const* label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f, bool editable = true)
			{
				bool isActive = false;

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
					if (editable)
					{
						values.x = resetValue;
						isActive = true;
					}
				}				

				ImGui::PopFont();
				ImGui::PopStyleColor(3);

				ImGui::SameLine();

				if (editable)
				{
					ImGui::DragFloat("##x", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
					if (ImGui::IsItemActive())
					{
						isActive = true;
					}
				}
				else 
				{
					ImGui::Text(" %.2f ", values.x);
				}

				ImGui::PopItemWidth();
				ImGui::SameLine();

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
				ImGui::PushFont(boldFont);

				if (ImGui::Button("Y", buttonSize))
				{
					if (editable)
					{
						values.y = resetValue;
						isActive = true;
					}
				}

				ImGui::PopFont();
				ImGui::PopStyleColor(3);

				ImGui::SameLine();

				if (editable)
				{
					ImGui::DragFloat("##y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
					if (ImGui::IsItemActive())
					{
						isActive = true;
					}
				}
				else 
				{
					ImGui::Text(" %.2f ", values.y);
				}

				ImGui::PopItemWidth();
				ImGui::SameLine();

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
				ImGui::PushFont(boldFont);

				if (ImGui::Button("Z", buttonSize))
				{
					if (editable)
					{
						values.z = resetValue;
						isActive = true;
					}
				}

				ImGui::PopFont();
				ImGui::PopStyleColor(3);

				ImGui::SameLine();

				if (editable)
				{
					ImGui::DragFloat("##z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
					if (ImGui::IsItemActive())
					{
						isActive = true;
					}
				}
				else
				{
					ImGui::Text(" %.2f ", values.z);
				}

				ImGui::PopItemWidth();

				ImGui::PopStyleVar();

				ImGui::Columns(1);

				ImGui::PopID();

				return isActive;
			}

			static bool DrawVec2Handler(char const* label, glm::vec2& values, float resetValue = 0.0f, float columnWidth = 100.0f, bool editable = true)
			{
				bool isActive = false;

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
					if (editable)
					{
						values.x = resetValue;
						isActive = true;
					}
				}

				ImGui::PopFont();
				ImGui::PopStyleColor(3);

				ImGui::SameLine();

				if (editable)
				{
					ImGui::DragFloat("##x", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
					if (ImGui::IsItemActive())
					{
						isActive = true;
					}
				}
				else
				{
					ImGui::Text(" %.2f ", values.x);
				}

				ImGui::PopItemWidth();
				ImGui::SameLine();

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
				ImGui::PushFont(boldFont);

				if (ImGui::Button("Y", buttonSize))
				{
					if (editable)
					{
						values.y = resetValue;
						isActive = true;
					}
				}

				ImGui::PopFont();
				ImGui::PopStyleColor(3);

				ImGui::SameLine();

				if (editable)
				{
					ImGui::DragFloat("##y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
					if (ImGui::IsItemActive())
					{
						isActive = true;
					}
				}
				else
				{
					ImGui::Text(" %.2f ", values.y);
				}

				ImGui::PopItemWidth();

				ImGui::PopStyleVar();

				ImGui::Columns(1);

				ImGui::PopID();

				return isActive;
			}

			static bool GetMousePosInWindow( ui32 windowID, glm::vec2& result)
			{
				ImGuiWindow* window = ImGui::FindWindowByID(windowID);

				if (window != nullptr)
				{
					auto[pos_x, pos_y] = window->Pos;
					auto[size_x, size_y] = window->Size;

					auto[mx, my] = ImGui::GetMousePos();

					mx -= pos_x;
					my = (pos_y + size_y) - my;

					if (mx >= 0.0f && mx < size_x && my >= 0.0f && my < size_y)
					{
						result.x = mx;
						result.y = my;	

						return true;
					}
				}

				return false;
			}

			static bool GetMouseRelativePosInWindow(ui32 windowID, glm::vec2& result)
			{
				ImGuiWindow* window = ImGui::FindWindowByID(windowID);

				if (window != nullptr)
				{
					auto[pos_x, pos_y] = window->Pos;
					auto[size_x, size_y] = window->Size;

					auto[mx, my] = ImGui::GetMousePos();

					mx -= pos_x;
					my = (pos_y + size_y) - my;

					if (mx >= 0.0f && mx < size_x && my >= 0.0f && my < size_y)
					{
						result.x = mx;
						result.y = my;

						result.x = (2.0f * result.x) / size_x - 1.0f;
						result.y = (2.0f * result.y) / size_y - 1.0f;

						return true;
					}
				}

				return false;
			}
			
			static bool DrawVariant(char const* label, Variant& variant, bool editing)
			{
				bool modified = false;

				ImGui::Columns(3);
				
				ImGui::Text(label);				
				ImGui::NextColumn();

				const char* variantTypes[] = {
						"Unknown",
						"Bool",
						"Int8",
						"UInt8",
						"Int16",
						"UInt16",
						"Int32",
						"UInt32",
						"Int64",
						"UInt64",
						"Float",
						"Vector2",
						"Vector3",
						"Quaternion2",
						"StringID"
					};

				if (editing)
				{
					ImGui::Text(variantTypes[(ui32)variant.m_valueType]);					
				}
				else
				{
					static int item_current_idx = 0;
					const char* combo_preview_value = variantTypes[item_current_idx];
					ImGuiComboFlags flags = ImGuiComboFlags_PopupAlignLeft;

					if (ImGui::BeginCombo("Variant Type", combo_preview_value, flags))
					{
						for (int n = 0; n < IM_ARRAYSIZE(variantTypes); n++)
						{
							const bool is_selected = (item_current_idx == n);
							if (ImGui::Selectable(variantTypes[n], is_selected))
							{
								variant.m_valueType = (VariantType)n;
								item_current_idx = n;
								modified = true;
							}

							// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
				}

				ImGui::NextColumn();

				if (editing)
				{
					switch (variant.m_valueType)
					{
					case VariantType::Bool:
					{
						static bool data = false;
						if (ImGui::Checkbox("", &data))
						{
							modified = true;
							variant = data;
						}
					}
					break;
					case VariantType::Int8:
					{
						static int data = 0;
						if (ImGui::InputInt("", &data))
						{
							modified = true;
							variant = (i8)data;
						}
					}
					break;
					case VariantType::UInt8:
					{
						static int data = 0;
						if (ImGui::InputInt("", &data))
						{
							if (data < 0)
							{
								data = 0;
							}
							modified = true;
							variant = (ui8)data;
						}
					}
					break;
					case VariantType::Int16:
					{
						static int data = 0;
						if (ImGui::InputInt("", &data))
						{
							modified = true;
							variant = (i16)data;
						}
					}
					break;
					case VariantType::UInt16:
					{
						static int data = 0;
						if (ImGui::InputInt("", &data))
						{
							if (data < 0)
							{
								data = 0;
							}
							modified = true;
							variant = (ui16)data;
						}
					}
					break;
					case VariantType::Int32:
					{
						static int data = 0;
						if (ImGui::InputInt("", &data))
						{
							modified = true;
							variant = (i32)data;
						}
					}
					break;
					case VariantType::UInt32:
					{
						static int data = 0;
						if (ImGui::InputInt("", &data))
						{
							if (data < 0)
							{
								data = 0;
							}
							modified = true;
							variant = (ui32)data;
						}
					}
					break;
					case VariantType::Int64:
					{
						static int data = 0;
						if (ImGui::InputInt("", &data))
						{
							modified = true;
							variant = (i64)data;
						}
					}
					break;
					case VariantType::UInt64:
					{
						static int data = 0;
						if (ImGui::InputInt("", &data))
						{
							if (data < 0)
							{
								data = 0;
							}
							modified = true;
							variant = (ui64)data;
						}
					}
					break;
					case VariantType::Float:
					{
						static float data = 0;
						if (ImGui::InputFloat("", &data))
						{
							modified = true;
							variant = data;
						}
					}
					break;
					case VariantType::Vector2:
					{
						static glm::vec2 data = glm::vec2(0.0f);
						if (DrawVec2Handler("", data))
						{
							modified = true;
							variant = data;
						}
					}
					break;
					case VariantType::Vector3:
					{
						static glm::vec3 data = glm::vec3(0.0f);
						if (DrawVec3Handler("", data))
						{
							modified = true;
							variant = data;
						}
					}
					break;
					case VariantType::StringID:
					{
						static char data[64];

						if (ImGui::InputText("", data, IM_ARRAYSIZE(data)))
						{
							modified = true;
							variant = BL_STRING_TO_STRINGID(data);
						}
					}
					break;
					default:
						break;

					}
				}


				ImGui::Columns(1);

				return modified;
			}
		}
	}
}
