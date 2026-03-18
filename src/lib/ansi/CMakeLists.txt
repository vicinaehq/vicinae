cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME ansi)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

OPTION(BUILD_TESTS "Build the test suite" OFF)

add_library(vicinae-ansi INTERFACE)
add_library(vicinae::ansi ALIAS vicinae-ansi)
target_include_directories(vicinae-ansi INTERFACE .)
target_compile_features(vicinae-ansi INTERFACE cxx_std_23)

if (BUILD_TESTS)
	set(TEST_TARGET vicinae-ansi-tests)
	find_package(Catch2 3 REQUIRED)
	add_executable(${TEST_TARGET} tests/tokenizer.cpp tests/html.cpp)
	target_link_libraries(${TEST_TARGET} PRIVATE Catch2::Catch2WithMain vicinae-ansi)
endif()
