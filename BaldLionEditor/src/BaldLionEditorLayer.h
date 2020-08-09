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
			virtual void OnImGuiRender() override;
			virtual void OnEvent(Event& e) override;

		private:
			void RenderDockSpace();
			bool OnWindowResizeEvent(WindowResizeEvent& e);

		private:

			std::vector<Ref<Rendering::AnimatedModel>> m_models;
			Ref<Rendering::Framebuffer> m_frameBuffer;

			glm::vec2 m_viewportSize;

			DirectionalLight directionalLight;
			std::vector<PointLight> pointLights;
		};
	}
}