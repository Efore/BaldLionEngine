#pragma once
#include "BaldLion/ResourceManagement/ResourceManager.h"

namespace BaldLion
{
	namespace Editor
	{
		namespace UtilsEditor
		{
			template<typename T>
			static T* RenderResourceInspectorPopup(const char* popupName, ResourceManagement::ResourceType resourceType)
			{
				static_assert(std::is_base_of<ResourceManagement::Resource, T>::value, "T must inherit from Resource");

				T* result = nullptr;

				if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
				{
					for (auto hashMapIterator = ResourceManagement::ResourceManager::GetResourceMap().Begin(); hashMapIterator != ResourceManagement::ResourceManager::GetResourceMap().End(); ++hashMapIterator)
					{
						if (hashMapIterator.GetValue()->GetResourceType() == resourceType)
						{
							if (ImGui::Selectable(STRINGID_TO_STR_C(hashMapIterator.GetValue()->GetResourceID())))
							{
								result = (T*)hashMapIterator.GetValue();
							}
						}
					}

					ImGui::EndPopup();
				}

				return result;
			}
		}
	}
}
