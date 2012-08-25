project "TinyThread++"
targetname "tinythread++"
targetdir "../../bin"
kind "StaticLib"
language "C++"
files { "*.cpp" }
links { "pthread" }
