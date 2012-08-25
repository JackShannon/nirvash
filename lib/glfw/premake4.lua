project "GLFW"
targetname "glfw3-tls"
targetdir "../../bin"
kind "StaticLib"
language "C"
files {
	"src/*.c"
}
-- these should all use X11, dunno if BSD/Solaris work, though.
if os.get() == "bsd"
	or os.get() == "linux"
	or os.get() == "solaris" then
	links { "Xrandr", "Xf86vidmode", "X11", "GL" }
	excludes {
		"src/cocoa_*",
		"src/win32_*"
	}
	os.copyfile(
		"src/config.h.x11", 
		"src/config.h"
	)
elseif os.get() == "macosx" then
	links { "OpenGL.framework" }
	excludes {
		"src/x11_*",
		"src/win32_*"
	}
	os.copyfile(
		"src/config.h.cocoa", 
		"src/config.h"
	)
elseif os.get() == "windows" then
	links { "opengl32" }
	excludes {
		"src/cocoa_*",
		"src/x11_*"
	}
	os.copyfile(
		"src/config.h.win32", 
		"src/config.h"
	)
else
	print "Invalid OS detected. Not sure what to think of this."
end

