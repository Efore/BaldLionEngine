#pragma once

#ifdef BL_PLATFORM_WINDOWS

#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  


extern BaldLion::Application& BaldLion::CreateApplication(const std::string& applicationName);

int main(int argc, char** argv)
{
	BaldLion::Log::Init();

	BL_PROFILE_BEGIN_SESSION("Startup", "BaldLionProfile-Startup.json");
	auto &app = BaldLion::CreateApplication("BaldLion Editor");	
	BL_PROFILE_END_SESSION();

	BL_PROFILE_BEGIN_SESSION("Runtime", "BaldLionProfile-Runtime.json");
	app.Run();
	BL_PROFILE_END_SESSION();

	BL_PROFILE_BEGIN_SESSION("Shutdown", "BaldLionProfile-Shutdown.json");
	delete &app;
	BL_PROFILE_END_SESSION();

	_CrtDumpMemoryLeaks();
}

#endif
