#include <BaldLion.h>
#include <imgui/imgui.h>
#include "ResourcesPanel.h"
#include "UtilsEditor.h"


namespace BaldLion {

	namespace Editor {

		void ResourcesPanel::OnImGuiRender()
		{
			ImGui::Begin("Resources");

			ImGui::Columns(3, "ResourceColums"); // 4-ways, with border
			ImGui::Separator();
			ImGui::Text("ID"); ImGui::NextColumn();			
			ImGui::Text("Path"); ImGui::NextColumn();
			ImGui::Text("Type"); ImGui::NextColumn();
			ImGui::Separator();

			static const char* s_resourceTypes[] = { "None", "Texture", "Model", "Mesh", "Skeleton", "Material", "Shader", "Animator", "Animation" };

			BL_HASHMAP_FOR(ResourceManagement::ResourceManager::GetResourceMap(), hashMapIterator)
			{
				ImGui::Text(std::to_string(hashMapIterator.GetValue()->GetResourceID()).c_str()); ImGui::NextColumn();
				ImGui::Text(BL_STRINGID_TO_STR_C(hashMapIterator.GetValue()->GetResourcePath())); ImGui::NextColumn();
				ImGui::Text(s_resourceTypes[(ui32)hashMapIterator.GetValue()->GetResourceType()]); ImGui::NextColumn();
				ImGui::Separator();
			}

			ImGui::End();
		}
	}
}
