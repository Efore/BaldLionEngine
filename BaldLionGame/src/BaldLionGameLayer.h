#pragma once
#include <BaldLion.h>

namespace BaldLion
{
	namespace Game
	{
		class BaldLionGameLayer : public GameStateLayer
		{

		public:
			BaldLionGameLayer();
			virtual ~BaldLionGameLayer();

			virtual void OnActivate() override;
			virtual void OnDeactivate() override;

			virtual void OnUpdate() override;
			virtual void OnImGuiRender() override;
			virtual void OnEvent(Event& e) override;			

		private:

			bool OnWindowResizeEvent(WindowResizeEvent& e);
			bool OnKeyPressedEvent(KeyPressedEvent& e);

			
		private:

			ECS::ECSManager* m_ecsManager = nullptr;
		};
	}
}