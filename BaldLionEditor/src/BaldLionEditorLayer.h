#pragma once
#include <BaldLion.h>
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/EntityPropertiesPanel.h"
#include "Panels/EditorViewportPanel.h"
#include "Panels/MemoryAllocationPanel.h"
#include "Panels/RenderingDataPanel.h"
#include "Panels/ResourcesPanel.h"
#include "Panels/AnimatorPanel.h"
#include "Panels/NavigationPanel.h"

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
			bool OnKeyPressedEvent(KeyPressedEvent& e);

			void OpenScene();
			void SaveScene();
			void SaveSceneAs();
			void NewScene();
			
		private:

			ECS::ECSManager* m_ecsManager;

			//Panels
			SceneHierarchyPanel m_sceneHierarchyPanel;
			EntityPropertiesPanel m_entityPropertiesPanel;
			EditorViewportPanel m_editorViewportPanel;
			RenderingDataPanel m_renderingDataPanel;
			MemoryAllocationPanel m_memoryAllocationPanel;
			ResourcesPanel m_resourcesPanel;
			AnimatorPanel m_animatorPanel;
			NavigationPanel m_navigationPanel;

			std::string m_currentScenePathFile;
		};
	}
}