solution "entity"
	language 		"C++"
	flags 			"Symbols"
	configurations	"default"
	platforms		"native"
	toolset			"clang"
	buildoptions	{ "-Wall", "-Wno-sign-compare", "-std=c++14" }
	includedirs		{ "include" }
	libdirs 		{ "lib" }
	excludes		{ "**.bak", "**~" }

	project "test"
		kind				"ConsoleApp"
		targetdir			"bin"
		targetname			"test"
		-- buildoptions		{ "-D_GLIBCXX_DEBUG" } --To view strings in gdb
		files				{ "src/test/**.cpp" }
		postbuildcommands	{ "./bin/test" }

