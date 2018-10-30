#pragma once

#ifdef BL_PLATFORM_WINDOWS

extern BaldLion::Application* BaldLion::CreateApplication();

int main(int argc, char** argv)
{
	printf("Hi there");
	auto app = BaldLion::CreateApplication();	
	app->Run();
	delete app;
}

#endif
