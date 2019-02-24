#include <BaldLion.h>


class Sandbox : public BaldLion::Application
{
public:
	Sandbox()
	{		
		
	}

	~Sandbox()
	{

	}
};

BaldLion::Application* BaldLion::CreateApplication()
{
	return new Sandbox();
}