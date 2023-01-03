solution "entity"
	language 		"C++"
	symbols			"On"
	configurations	"default"
	platforms		"native"
	toolset			"clang"
	buildoptions	{ "-Wall", "-Wextra", "-Wpedantic", "-std=c++17" }
	includedirs		{ "include" }
	libdirs 		{ "lib" }
	excludes		{ "**.bak", "**~" }

	project "test"
		kind				"ConsoleApp"
		targetdir			"bin"
		targetname			"test"
		-- buildoptions		{ "-D_GLIBCXX_DEBUG" } --To view strings in gdb
		files				{ "src/test/**.cpp" }
		configuration "linux"
			postbuildcommands	{ "./bin/test" }
