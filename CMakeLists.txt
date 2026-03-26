cmake_minimum_required(VERSION 3.22)

project(
	libentity
	VERSION 1.2.1
	DESCRIPTION "an object serialisation/deserialisation header-only library"
	LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER_LINKER clang++)

add_compile_options(-Wall -Wextra -Wpedantic -fno-omit-frame-pointer)
enable_testing()


file(GLOB_RECURSE test_sources src/test/*.cpp)

add_executable(etest ${test_sources})
target_include_directories(etest PUBLIC include)
add_test(NAME entity-tests COMMAND etest)
