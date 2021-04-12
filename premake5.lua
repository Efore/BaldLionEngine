workspace "BaldLionEngine"
	architecture "x64"
	startproject "BaldLionEditor"
	
	configurations
	{
		"Debug","Release","Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "BaldLionEngine/vendor/GLFW/include"
IncludeDir["Glad"] = "BaldLionEngine/vendor/Glad/include"
IncludeDir["ImGui"] = "BaldLionEngine/vendor/imgui"
IncludeDir["glm"] = "BaldLionEngine/vendor/glm/"
IncludeDir["stb_image"] = "BaldLionEngine/vendor/stb_image/"
IncludeDir["assimp"] = "BaldLionEngine/vendor/assimp/include"
IncludeDir["optick"] = "BaldLionEngine/vendor/optick/include"

group "Dependencies"
	include "BaldLionEngine/vendor/GLFW"
	include "BaldLionEngine/vendor/Glad"
	include "BaldLionEngine/vendor/imgui"
group ""

project "BaldLionEngine"
	location "BaldLionEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "blpch.h"
	pchsource "BaldLionEngine/src/blpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}
	
	
	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.optick}"		
	}	
	
	libdirs 
	{ 
		"BaldLionEngine/vendor/assimp/lib",
		"BaldLionEngine/vendor/optick/lib/x64/%{cfg.shortname}",
	}
	
	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"assimp-vc141-mtd.lib",
		"OptickCore.lib"
	}

	filter "system:windows"		
		systemversion "latest"

		defines
		{
			"BL_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "BL_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "BL_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "BL_DIST"
		runtime "Release"
		optimize "on"

project "BaldLionEditor"
	location "BaldLionEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	debugenvs { 
		"PATH=%PATH%;$(ProjectDir)lib"
	}
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"BaldLionEngine/vendor/spdlog/include",
		"BaldLionEngine/src",
		"BaldLionEngine/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.optick}"
	}

	links
	{
		"BaldLionEngine"
	}

	filter "system:windows"	
		systemversion "latest"

		defines
		{
			"BL_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "BL_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "BL_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "BL_DIST"
		runtime "Release"
		optimize "on"
		
project "BaldLionSandbox"
	location "BaldLionSandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	debugenvs { 
		"PATH=%PATH%;$(ProjectDir)lib"
	}
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"BaldLionEngine/vendor/spdlog/include",
		"BaldLionEngine/src",
		"BaldLionEngine/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.optick}"
	}

	links
	{
		"BaldLionEngine"
	}

	filter "system:windows"	
		systemversion "latest"

		defines
		{
			"BL_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "BL_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "BL_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "BL_DIST"
		runtime "Release"
		optimize "on"
		