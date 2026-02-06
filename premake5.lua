workspace "GameEngine"
	architecture "x64"
	startproject "GeometryTool2D"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Engine/vendor/glad/premake5.lua"
include "Engine/vendor/glfw_premake5.lua"

project "Engine"
	location "Engine"
	kind "StaticLib"

	language "C++"
	cppdialect "C++20"

	targetdir ("build/" .. outputdir .. "/%{prj.name}")
	objdir ("build-int/" .. outputdir .. "/%{prj.name}")

	buildoptions "/utf-8"
	staticruntime "On"
	pchheader "pch.h"
	pchsource "Engine/src/pch.cpp"

	files
	{
		"%{prj.name}/src/**.h", 
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/vendor/stb-image",
		"%{prj.name}/vendor/glfw/include",
		"%{prj.name}/vendor/glad/include",
		"%{prj.name}/vendor/glm"
	}

	links
	{
		"opengl32.lib",
		"GLFW",
		"Glad"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"FT_PLATFORM_WINDOWS",
			"FT_OPENGL_RENDERER",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{MKDIR} ../build/" .. outputdir .. "/GeometryTool2D"),
			("{COPYFILE} %{cfg.buildtarget.relpath} ../build/" .. outputdir .. "/GeometryTool2D/%{cfg.buildtarget.name}")
 		}

	filter "configurations:Debug"
		defines "FT_DEBUG"
		runtime "Debug"
		symbols "On"
		
	filter "configurations:Release"
		defines "FT_RELEASE"
		runtime "Release"
		optimize "On"
		
	filter "configurations:Dist"
		defines "FT_DIST"
		runtime "Release"
		optimize "On"

project "GeometryTool2D"
	location "GeometryTool2D"
	kind "ConsoleApp"

	language "C++"
	cppdialect "C++20"

	targetdir ("build/" .. outputdir .. "/%{prj.name}")
	objdir ("build-int/" .. outputdir .. "/%{prj.name}")

	staticruntime "On"
	buildoptions "/utf-8"

	files
	{
		"%{prj.name}/src/**.h", 
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Engine/vendor/spdlog/include",
		"Engine/vendor/stb-image",
		"Engine/vendor/glfw/include",
		"Engine/vendor/glad/include",
		"Engine/vendor/glm",
		"Engine/src"
	}

	links
	{
		"Engine"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"FT_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "FT_DEBUG"
		runtime "Debug"
		symbols "On"
		
	filter "configurations:Release"
		defines "FT_RELEASE"
		runtime "Release"
		optimize "On"
		
	filter "configurations:Dist"
		defines "FT_DIST"
		runtime "Release"
		optimize "On"
		
project "GeometryTool3D"
	location "GeometryTool3D"
	kind "ConsoleApp"

	language "C++"
	cppdialect "C++20"

	targetdir ("build/" .. outputdir .. "/%{prj.name}")
	objdir ("build-int/" .. outputdir .. "/%{prj.name}")

	staticruntime "On"
	buildoptions "/utf-8"

	files
	{
		"%{prj.name}/src/**.h", 
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Engine/vendor/spdlog/include",
		"Engine/vendor/stb-image",
		"Engine/vendor/glfw/include",
		"Engine/vendor/glad/include",
		"Engine/vendor/glm",
		"Engine/src"
	}

	links
	{
		"Engine"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"FT_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "FT_DEBUG"
		runtime "Debug"
		symbols "On"
		
	filter "configurations:Release"
		defines "FT_RELEASE"
		runtime "Release"
		optimize "On"
		
	filter "configurations:Dist"
		defines "FT_DIST"
		runtime "Release"
		optimize "On"
		