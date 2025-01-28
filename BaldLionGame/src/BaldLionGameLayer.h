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

		private:

			bool OnWindowResizedEvent(const EventEntry& e);
			bool OnKeyPressedEvent(const EventEntry& e);

			
		private:

			ECS::ECSManager* m_ecsManager = nullptr;
		};
	}
}