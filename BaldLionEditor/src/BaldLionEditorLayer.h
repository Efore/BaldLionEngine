#pragma once
#include <BaldLion.h>

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

			virtual void OnUpdate(TimeStep timeStep) override;
			virtual void OnImGuiRender(TimeStep timeStep) override;
			virtual void OnEvent(Event& e) override;

		private:
			void RenderDockSpace();
			bool OnWindowResizeEvent(WindowResizeEvent& e);

		private:
			Rendering::Framebuffer* m_frameBuffer;

			glm::vec2 m_viewportSize;
			bool m_viewPortFocused;

			DirectionalLight m_directionalLight;
		};
	}
}