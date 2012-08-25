project "GL3W"
targetname "gl3w"
kind "StaticLib"
language "C"
targetdir "../../bin"

includedirs { "include" }

-- FIXME
links { "X11", "Xrandr", "GL" }

files {
	"src/gl3w.c"
}
