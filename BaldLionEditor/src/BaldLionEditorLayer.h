#pragma once
#include <BaldLion.h>
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/EntityPropertiesPanel.h"
#include "Panels/EditorViewportPanel.h"
#include "Panels/MemoryAllocationPanel.h"
#include "Panels/RenderingDataPanel.h"

namespace BaldLion
{
	namespace Editor
	{
		class BaldLionEditorLayer : public Layer
		{

		public:
			BaldLionEditorLayer();
			virtual ~BaldLionEditorLayer() = default;

			virtual void OnAttach() override;
			virtual void OnDetach() override;

			virtual void OnUpdate() override;
			virtual void OnImGuiRender() override;
			virtual void OnEvent(Event& e) override;

		private:
			void RenderDockSpace();
			bool OnWindowResizeEvent(WindowResizeEvent& e);

		private:

			ECS::ECSManager* m_ecsManager;

			//Panels
			SceneHierarchyPanel m_sceneHierarchyPanel;
			EntityPropertiesPanel m_entityPropertiesPanel;
			EditorViewportPanel m_editorViewportPanel;
			RenderingDataPanel m_renderingDataPanel;
			MemoryAllocationPanel m_memoryAllocationPanel;
		};
	}
}