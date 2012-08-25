solution "underwater programmer boxing"
configurations { "Debug", "Release" }

configuration "Debug"
	targetsuffix "-d"

configuration { "linux", "gmake", "Debug" }
	buildoptions { "-ggdb" }

include "lib/tinythread"
include "lib/glfw"
include "lib/assimp"

project "Nepgear" 
	targetname "nepgear"
	targetdir "bin"
	kind "ConsoleApp"
	language "C++"
	files { "src/**.cpp" }
	libdirs { "bin" }
	links {
		"pthread", "TinyThread++",
		"GLFW", "Xrandr", "X11", "GL", "z",
		"Assimp"
	}
	includedirs {
		"lib/glfw/include",
		"lib/assimp/include",
		"lib/glm",
		"lib/tinythread",
		"src"
	}

	configuration { "linux", "gmake" }
		buildoptions { "-std=c++0x", "-Wall", "-pedantic" }

	configuration { "linux", "gmake", "Debug" }
		buildoptions { "-ggdb" }

-- TODO
newaction {
	trigger     = "install",
	description = "Install Nepgear (NOT IMPLEMENTED)",
	execute = function ()
		-- copy files, etc. here
	end
}