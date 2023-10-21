
#include "BaldLionGameLayer.h"
#include <BaldLion/Core/EntryPoint.h>

namespace BaldLion
{
	namespace Game
	{
		class BaldLionGameApp : public Application
		{
		public:

			~BaldLionGameApp()
			{
				m_layerStack.PopLayer(m_baldLionEditorLayer);				
			}

			static Application& GetInstance(const std::string& applicationName)
			{
				if (s_instance == nullptr)
					s_instance = new BaldLionGameApp(applicationName);

				return *s_instance;
			}			

		private:

			BaldLionGameApp(const std::string& applicationName) : Application(applicationName)
			{
				m_baldLionEditorLayer = MemoryManager::New<BaldLionGameLayer>("BaldLionGameLayer", AllocationType::FreeList_Main);
				PushLayer(m_baldLionEditorLayer);
			}

			BaldLionGameLayer* m_baldLionEditorLayer = nullptr;

		};		
	}

	Application& CreateApplication(const std::string& applicationName)
	{
		return Game::BaldLionGameApp::GetInstance(applicationName);
	}
}
