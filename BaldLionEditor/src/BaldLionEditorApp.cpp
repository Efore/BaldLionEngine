
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

			static Application& GetInstance()
			{
				if (s_instance == nullptr)
					s_instance = new BaldLionEditorApp();

				return *s_instance;
			}

		private:

			BaldLionEditorApp()
			{
				PushLayer(new BaldLionEditorLayer());
			}

		};		
	}

	Application& CreateApplication()
	{
		return Editor::BaldLionEditorApp::GetInstance();
	}
}
