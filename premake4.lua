solution "entity"
	language 		"C++"
	targetdir		"lib"
	flags 			"Symbols"
	configurations	"default"
	platforms		"native"
	buildoptions	{ "-Wall", "-Wno-sign-compare",  "-std=c++11", "-O3", "-fPIC" }
	includedirs		{ "include" }
	libdirs 		{ "lib" }
	excludes		{ "**.bak", "**~" }
		
	project "libentity"
		kind 			"SharedLib"
		targetname		"entity"
		linkoptions		"-Wl,-soname,libentity.so.0"
		includedirs		{ "include/entity" }
		files 			{ "include/entity/**.h", "src/entity/**.cpp" }
		postbuildcommands	{ "./strip lib/libentity.so" }

	project "test"
		kind				"SharedLib"
		defines				{ "__stdcall=" }
		links				{ "libentity", "xUnit++" }
		files				{ "include/test/**.h", "src/test/**.cpp" }
		postbuildcommands	{ "./runner" }
