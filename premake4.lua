solution "entity"
	language 		"C++"
	targetdir		"lib"
	flags 			"Symbols"
	configurations	"default"
	platforms		"native"
	buildoptions	{ "-Wall", "-Wno-sign-compare", "-std=c++11", "-O3", "-fPIC", "-D_FORTIFY_SOURCE=2" }
	includedirs		{ "include" }
	libdirs 		{ "lib" }
	excludes		{ "**.bak", "**~" }

	-- project "libentity"
	-- 	kind 			"None"
	-- 	targetname		"entity"
	-- 	linkoptions		"-Wl,-soname,libentity.so.0"
	-- 	includedirs		{ "include/entity" }
	-- 	files 			{ "include/entity/**.hpp", "src/entity/**.cpp" }
	-- 	postbuildcommands	{ "./strip lib/libentity.so" }

	project "libtest"
		kind				"SharedLib"
		targetname			"test"
		defines				{ "__stdcall=" }
		links				{ "xUnit++" } --{ "libentity", "xUnit++" }
		files				{ "src/test/**.cpp" }
		postbuildcommands	{ "./runner" }

	project "benchmark"
		kind			"ConsoleApp"
		targetname		"benchmark"
		targetdir		"bin"
		includedirs		{ "include/benchmark" }
		-- links			{ "libentity" }
		files			{ "src/benchmark/**.cpp" }
