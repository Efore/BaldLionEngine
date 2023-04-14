#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/glm.hpp>

namespace BaldLion {

	namespace Editor {

		class EditorPanel {

		public:
			EditorPanel(const char* panelName) {
				m_panelName = BL_STRING_TO_STRINGID(panelName);
			}

			virtual ~EditorPanel(){}
			virtual void OnImGuiRender(){}
			virtual void OnKeyPressed(int keycode){}

			glm::vec2 GetPanelCoords() 
			{
				ImGuiWindow* window = ImGui::FindWindowByID(m_panelID);				
				return glm::vec2(window->Pos.x, window->Pos.y);				
			}

			glm::vec2 GetPanelSize() const 
			{
				ImGuiWindow* window = ImGui::FindWindowByID(m_panelID);
				return glm::vec2(window->Size.x, window->Size.y);
			}

			ui32 GetPanelID() const
			{
				return m_panelID;
			}

		protected:

			bool m_viewportFocused;
			StringId m_panelName;
			ui32 m_panelID;
		};
	}
}
#pragma once

