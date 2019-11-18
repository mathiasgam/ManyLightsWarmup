workspace "ManyLights"
	architecture "x64"
	cppdialect "c++17"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

	filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

      filter "action:vs*"
      	buildoptions { "/Ox", "/Oi", "/Ot", "/GL" }
      	linkoptions { "/LTCG", "/INCREMENTAL" }
      	vectorextensions "AVX2"
  		--defines "/arch:AVX2"

    filter "configurations:Dist"
      defines { "NDEBUG" }
      optimize "On"

      filter "action:vs*"
  		buildoptions { "/Ox", "/Oi", "/Ot", "/GL" }
  		linkoptions { "/LTCG", "/INCREMENTAL" }
  		vectorextensions "AVX2"

  	filter "configurations:Debug"
  		defines "DEBUG"
  		linkoptions { "/LTCG", "/INCREMENTAL" }




outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"



project "RayTracer"
	kind "ConsoleApp"
	location "RayTracer"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")


	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	links
	{
		"Vector"
	}

	includedirs {
		"RayTracer/src",
		"Vector/src"
	}
	defines {
		"API_PLATFORM_WINDOW"
	}
	
project "Vector"
	kind "SharedLib"
	location "Vector"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	defines {
		"API_PLATFORM_WINDOW",
		"API_BUILD_DLL"
	}

	postbuildcommands {
		("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/RayTracer")
	}