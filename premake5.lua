solution "entity"
	language 		"C++"
	targetdir		"lib"
	flags 			"Symbols"
	configurations	"default"
	platforms		"native"
	toolset			"clang"
	buildoptions	{ "-Wall", "-Wno-sign-compare", "-std=c++11", "-O3", "-fPIC", "-D_FORTIFY_SOURCE=2" }
	includedirs		{ "include" }
	libdirs 		{ "lib" }
	excludes		{ "**.bak", "**~" }

	project "libtest"
		kind				"SharedLib"
		targetname			"test"
		defines				{ "__stdcall=" }
		links				{ "xUnit++" }
		files				{ "src/test/**.cpp" }
		postbuildcommands	{ "./runner" }

