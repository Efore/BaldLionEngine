#pragma once

#ifdef BL_PLATFORM_WINDOWS

extern BaldLion::Application* BaldLion::CreateApplication();

int main(int argc, char** argv)
{
	BaldLion::Log::Init();
	BL_LOG_CORE_WARN("Initialized log!");
	int as = 5;
	BL_LOG_INFO("Hello! Var={0}",as);
	
	auto app = BaldLion::CreateApplication();	
	app->Run();
	delete app;
}

#endif
