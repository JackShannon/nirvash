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

project "GLFW"
	targetname "glfw3-tls"
	targetdir "bin"
	kind "StaticLib"
	language "C"
	files {
		"lib/glfw/src/*.c"
	}
	-- these should all use X11, dunno if BSD/Solaris work, though.
	if os.get() == "bsd"
		or os.get() == "linux"
		or os.get() == "solaris" then
		links { "Xrandr", "Xf86vidmode", "X11", "GL" }
		excludes {
			"lib/glfw/src/cocoa_*",
			"lib/glfw/src/win32_*"
		}
		os.copyfile(
			"lib/glfw/src/config.h.x11", 
			"lib/glfw/src/config.h"
		)
	elseif os.get() == "macosx" then
		links { "OpenGL.framework" }
		excludes {
			"lib/glfw/src/x11_*",
			"lib/glfw/src/win32_*"
		}
		os.copyfile(
			"lib/glfw/src/config.h.cocoa", 
			"lib/glfw/src/config.h"
		)
	elseif os.get() == "windows" then
		links { "opengl32" }
		excludes {
			"lib/glfw/src/cocoa_*",
			"lib/glfw/src/x11_*"
		}
		os.copyfile(
			"lib/glfw/src/config.h.win32", 
			"lib/glfw/src/config.h"
		)
	else
		print "Invalid OS detected. Not sure what to think of this."
	end

project "Nepgear" 
	targetname "nepgear"
	targetdir "bin"
	kind "ConsoleApp"
	language "C++"
	files { "src/**.cpp" }
	libdirs { "lib/glfw" }
	links {
		"pthread", "TinyThread++",
		"GLFW", "Xrandr", "X11", "GL"
	}
	includedirs {
		"lib/glfw/include",
		"lib/glm",
		"lib/tinythread",
		"src"
	}

	configuration { "linux", "gmake" }
		buildoptions { "-std=c++0x", "-Wall", "-pedantic" }