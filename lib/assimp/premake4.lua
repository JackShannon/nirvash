project "Assimp"
targetdir "../../bin"
targetname "assimp"
kind "StaticLib"
language "C++"

includedirs {
	"src/BoostWorkaround"
}

files {
	"src/*.cpp",
	"contrib/**.c",
	"contrib/**.cc",
	"contrib/**.cpp"
}
