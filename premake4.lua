solution "entity"
	language 		"C++"
	targetdir		"bin"
	flags 			"Symbols"
	configurations	"default"
	platforms		"native"
	buildoptions	{ "-Wall", "-Wno-sign-compare",  "-std=c++11", "-O3" }
	--links			{ "boost_system", "boost_filesystem", "boost_serialization", "freeimageplus" }
	includedirs		{ "include" }
	excludes		{ "**.bak", "**~" }
		
	project "libentity"
		kind 			"SharedLib"
		targetname		"entity"
		targetdir		"lib"
		linkoptions		"-Wl,-soname,libentity.so.0"
		includedirs		{ "include/entity" }
		files 			{ "include/entity/**.h", "src/entity/**.cpp" }
		postbuildcommands	{ "./strip lib/libentity.so" }

	project "test"
		kind 				"ConsoleApp"
		links				{ "libentity" }
		files				{ "include/test/**.h", "src/test/**.cpp" }
		postbuildcommands	{ "./strip bin/test" }