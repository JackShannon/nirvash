project "Lua"
targetdir "../../bin"
targetname "lua"
kind "StaticLib"
language "C"

files {
	"src/*.c",
}

excludes {
	"src/luac.c"
}