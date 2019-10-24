workspace "ManyLights"
	architecture "x64"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

	filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

      filter "action:vs*"
  		defines "/arch:AVX2"
  		defines "/Ox"
  		defines "/Oi"
  		defines "/Ot"
  		defines "/GL"
  		defines "/LTCG:incremental"

    filter "configurations:Dist"
      defines { "NDEBUG" }
      optimize "On"

      filter "action:vs*"
  		defines "/arch:AVX2"
  		defines "/Ox"
  		defines "/Oi"
  		defines "/Ot"
  		defines "/GL"
  		defines "/LTCG:incremental"




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