#pragma once

#ifdef BL_PLATFORM_WINDOWS

extern BaldLion::Application& BaldLion::CreateApplication();

int main(int argc, char** argv)
{
	BaldLion::Log::Init();

	BL_PROFILE_BEGIN_SESSION("Startup", "BaldLionProfile-Startup.json");
	auto &app = BaldLion::CreateApplication();	
	BL_PROFILE_END_SESSION();

	BL_PROFILE_BEGIN_SESSION("Runtime", "BaldLionProfile-Runtime.json");
	app.Run();
	BL_PROFILE_END_SESSION();

	BL_PROFILE_BEGIN_SESSION("Shutdown", "BaldLionProfile-Shutdown.json");
	delete &app;
	BL_PROFILE_END_SESSION();
}

#endif
