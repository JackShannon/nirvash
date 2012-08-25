project "PhysFS"
targetname "physfs"
kind "StaticLib"
language "C"
targetdir "../../bin"

links("ZLib")

includedirs{ "./" }

files {
	"*.c",
	"archivers/dir.c",
	"archivers/zip.c",
	"platform/unix.c",
	"platform/posix.c"
}

if os.is "macosx" then
	files { "platform/macosx.c" }
	table.insert(global_links, "IOKit.framework")
end

defines {
	"PHYSFS_NO_CDROM_SUPPORT"
}
