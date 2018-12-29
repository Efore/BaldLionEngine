#include <BaldLion.h>

class ExampleLayer : public BaldLion::Layer
{
public:
	ExampleLayer() : Layer("Example")
	{

	}

	void OnUpdate() override
	{
		BL_LOG_INFO("ExampleLayer::Update");
	}

	void OnEvent(BaldLion::Event& event) override
	{
		BL_LOG_TRACE("{0}", event);
	}
};



class Sandbox : public BaldLion::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}
};

BaldLion::Application* BaldLion::CreateApplication()
{
	return new Sandbox();
}