#pragma once

#ifdef BL_PLATFORM_WINDOWS

#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  

#include "Application.h"

extern BaldLion::Application& BaldLion::CreateApplication(const std::string& applicationName);

int main(int argc, char** argv)
{
	BaldLion::Log::Init();

	auto &app = BaldLion::CreateApplication("BaldLion Editor");	

	app.Run();

	delete &app;

	_CrtDumpMemoryLeaks();
}

#endif
