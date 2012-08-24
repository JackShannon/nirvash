solution "underwater programmer boxing"
configurations { "Debug", "Release" }

configuration "Debug"
	targetsuffix "-d"

project "TinyThread++"
	targetname "tinythread++"
	targetdir "bin"
	kind "StaticLib"
	language "C++"
	files { "lib/tinythread/*.cpp" }
	links { "pthread" }

project "Nepgear" 
	targetname "nepgear"
	targetdir "bin"
	kind "ConsoleApp"
	language "C++"
	files { "src/**.cpp" }
	libdirs { "lib/glfw" }
	links { "pthread", "glfw3", "X11", "GL", "Xrandr", "TinyThread++" }
	includedirs {
		"lib/glfw/include",
		"lib/glm",
		"lib/tinythread",
		"src"
	}

	configuration { "linux", "gmake" }
		buildoptions { "-std=c++0x", "-Wall", "-pedantic" }