#pragma once
#include <BaldLion.h>
#include <imgui/imgui.h>
#include "RenderingDataPanel.h"


namespace BaldLion {

	namespace Editor {

		void RenderingDataPanel::OnImGuiRender(TimeStep timeStep)
		{
			ImGui::Begin("Rendering");
			ImGui::Text("Performance: %f", 1.0f / timeStep.GetSeconds());
			ImGui::Text("Draw calls: %zu", Renderer::GetRenderStats().drawCalls);
			ImGui::Text("Vertices: %zu", Renderer::GetRenderStats().vertices);
			ImGui::End();
		}

	}
}
