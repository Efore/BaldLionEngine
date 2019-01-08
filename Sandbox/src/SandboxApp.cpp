#include <BaldLion.h>


class Sandbox : public BaldLion::Application
{
public:
	Sandbox()
	{
		PushOverlay(new BaldLion::ImGuiLayer());
	}

	~Sandbox()
	{

	}
};

BaldLion::Application* BaldLion::CreateApplication()
{
	return new Sandbox();
}