
#include "BaldLionEditorLayer.h"
#include <BaldLion/Core/EntryPoint.h>

namespace BaldLion
{
	namespace Editor
	{
		class BaldLionEditorApp : public Application
		{
		public:

			~BaldLionEditorApp()
			{
				m_layerStack.PopOverlay(m_baldLionEditorLayer);
				Animation::AnimationManager::Stop();
			}

			static Application& GetInstance(const std::string& applicationName)
			{
				if (s_instance == nullptr)
					s_instance = new BaldLionEditorApp(applicationName);

				return *s_instance;
			}			

		private:

			BaldLionEditorApp(const std::string& applicationName) : Application(applicationName)
			{
				Animation::AnimationManager::Init();
				m_baldLionEditorLayer = MemoryManager::New<BaldLionEditorLayer>("BaldLionEditorLayer", AllocationType::FreeList_Main);
				PushLayer(m_baldLionEditorLayer);
			}

			BaldLionEditorLayer* m_baldLionEditorLayer = nullptr;

		};		
	}

	Application& CreateApplication(const std::string& applicationName)
	{
		return Editor::BaldLionEditorApp::GetInstance(applicationName);
	}
}
