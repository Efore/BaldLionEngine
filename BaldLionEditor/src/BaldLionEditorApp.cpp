
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
				PushLayer(new BaldLionEditorLayer());
			}

		};		
	}

	Application& CreateApplication(const std::string& applicationName)
	{
		return Editor::BaldLionEditorApp::GetInstance(applicationName);
	}
}