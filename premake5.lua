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
IncludeDir["ImGuizmo"] = "BaldLionEditor/vendor/ImGuizmo"
IncludeDir["debug_draw"] = "BaldLionEngine/vendor/debug-draw"
IncludeDir["yaml"] = "BaldLionEngine/vendor/yaml/include"
IncludeDir["ReactPhysics3D"] = "BaldLionEngine/vendor/ReactPhysics3D/include"
IncludeDir["RecastDetour"] = "BaldLionEngine/vendor/Recast/Detour/include"
IncludeDir["RecastDetourCrowd"] = "BaldLionEngine/vendor/Recast/DetourCrowd/include"
IncludeDir["RecastDetourTileCache"] = "BaldLionEngine/vendor/Recast/DetourTileCache/include"
IncludeDir["RecastRecast"] = "BaldLionEngine/vendor/Recast/Recast/include"

group "Dependencies"
	include "BaldLionEngine/vendor/GLFW"
	include "BaldLionEngine/vendor/Glad"
	include "BaldLionEngine/vendor/imgui"
	include "BaldLionEngine/vendor/yaml"
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
		"%{prj.name}/vendor/glm/glm/**.inl",	
		
		"%{prj.name}/vendor/debug-draw/**.hpp",
		
		"%{prj.name}/vendor/optick/**.h",
		"%{prj.name}/vendor/optick/**.cpp",
		
		"%{prj.name}/vendor/assimp/**.h",
		"%{prj.name}/vendor/assimp/**.hpp",
		"%{prj.name}/vendor/assimp/**.inl",
		
		"%{prj.name}/vendor/ReactPhysics3D/**.h",
		"%{prj.name}/vendor/ReactPhysics3D/**.cpp",
		
		"%{prj.name}/vendor/Recast/**.h",		
		"%{prj.name}/vendor/Recast/**.cpp"
	}	
	
	removefiles {
		"%{prj.name}/vendor/optick/src/optick_gpu.d3d12.**",
		"%{prj.name}/vendor/optick/src/optick_gpu.vulkan.**",
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
		"%{IncludeDir.optick}",
		"%{IncludeDir.debug_draw}",		
		"%{IncludeDir.yaml}",
		"%{IncludeDir.ReactPhysics3D}",
		"%{IncludeDir.RecastDetour}",
		"%{IncludeDir.RecastDetourCrowd}",
		"%{IncludeDir.RecastDetourTileCache}",
		"%{IncludeDir.RecastRecast}"
	}	
		
	libdirs 
	{ 	
		"BaldLionEngine/vendor/assimp/lib",
		--"BaldLionEngine/vendor/ReactPhysics3D/lib",
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"yaml-cpp",
		--"reactphysics3d.lib",
		"assimp-vc142-mtd.lib",
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
		
	filter "files:BaldLionEngine/vendor/optick/**.cpp"
		flags {"NoPCH"}
		
	filter "files:BaldLionEngine/vendor/ReactPhysics3D/**.cpp"
		flags {"NoPCH"}
		
	filter "files:BaldLionEngine/vendor/Recast/**.cpp"
		flags {"NoPCH"}

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

	postbuildcommands {
			-- Copy the necessary files to the target dir
		'{MKDIR} "%{cfg.targetdir}/assets"',
		'{COPYDIR} "%{wks.location}BaldLionEditor/assets/" "%{cfg.targetdir}/assets"',
		'{COPYFILE} "%{wks.location}BaldLionEngine/vendor/assimp/lib/assimp-vc142-mtd.dll" "%{cfg.targetdir}"',			
		'{COPYFILE} "%{wks.location}BaldLionEditor/imgui.ini" "%{cfg.targetdir}"'		
	}	
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.h",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.cpp"		
	}

	includedirs
	{
		"BaldLionEngine/vendor/spdlog/include",
		"BaldLionEngine/src",
		"BaldLionEngine/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.optick}",	
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.debug_draw}",		
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.yaml}",
		"%{IncludeDir.ReactPhysics3D}",
		"%{IncludeDir.RecastDetour}",
		"%{IncludeDir.RecastDetourCrowd}",
		"%{IncludeDir.RecastDetourTileCache}",
		"%{IncludeDir.RecastRecast}"
	}
	
	-- libdirs 
	-- { 
		-- "BaldLionEngine/vendor/assimp/lib",
		-- "BaldLionEngine/vendor/ReactPhysics3D/lib",
	-- }

	links
	{
		"BaldLionEngine",
		-- "assimp-vc142-mtd.lib",
		-- "reactphysics3d.lib"
	}

	
	filter "files:BaldLionEngine/vendor/ImGuizmo/**.cpp"
		flags {"NoPCH"}
				
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
		