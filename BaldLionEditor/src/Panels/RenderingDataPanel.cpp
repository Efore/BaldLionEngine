#pragma once
#include <BaldLion.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "RenderingDataPanel.h"


namespace BaldLion {

	namespace Editor {

		void RenderingDataPanel::OnImGuiRender(float deltaTime)
		{
			ImGui::Begin(BL_STRINGID_TO_STR_C(m_panelName));
			m_panelID = ImGui::GetCurrentWindow()->ID;

			ImGui::Text("Performance: %f", 1.0f / deltaTime);
			ImGui::Text("Draw calls: %zu", Renderer::GetRenderStats().drawCalls);
			ImGui::Text("Vertices: %zu", Renderer::GetRenderStats().vertices);
			ImGui::End();
		}

	}
}
