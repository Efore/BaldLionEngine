#pragma once
#include <BaldLion.h>
#include <imgui/imgui.h>
#include "RenderingDataPanel.h"


namespace BaldLion {

	namespace Editor {

		void RenderingDataPanel::OnImGuiRender()
		{
			ImGui::Begin("Rendering");
			ImGui::Text("Performance: %f", 1.0f / Time::GetDeltaTime());
			ImGui::Text("Draw calls: %zu", Renderer::GetRenderStats().drawCalls);
			ImGui::Text("Vertices: %zu", Renderer::GetRenderStats().vertices);
			ImGui::End();
		}

	}
}
