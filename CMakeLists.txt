cmake_minimum_required(VERSION 3.16)
set(OMNICAST_VERSION v1.0.0)

set(PROJECT_NAME omnicast)
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES C CXX)

add_compile_definitions(OMNICAST_VERSION="${OMNICAST_VERSION}")

find_program(CCACHE ccache)

if (CCACHE) 
	set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")
endif()

if ("$ENV{DIST_TYPE}" STREQUAL "generic")
	message(STATUS "DIST_TYPE=generic, won't apply local optimizations")
else()
	add_compile_options(-march=native -g3)
endif()

add_subdirectory(omnictl)
add_subdirectory(omnicast)
add_subdirectory(proto)
add_subdirectory(wlr-clip)
