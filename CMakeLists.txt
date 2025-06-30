include(ProcessorCount)

cmake_minimum_required(VERSION 3.16)
set(OMNICAST_VERSION v1.0.0)

set(PROJECT_NAME omnicast)
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES C CXX)

add_compile_definitions(OMNICAST_VERSION="${OMNICAST_VERSION}")

#find_program(CCACHE ccache)

ProcessorCount(N)

if(NOT N EQUAL 0)
  message(STATUS "Number of processors: ${N}")
  set(CMAKE_BUILD_PARALLEL_LEVEL ${N})
else()
  message(STATUS "Unable to determine processor count, defaulting to 1")
  set(CMAKE_BUILD_PARALLEL_LEVEL 1)
endif()

if (CCACHE) 
	#set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")
endif()

if ("$ENV{DIST_TYPE}" STREQUAL "generic")
	message(STATUS "DIST_TYPE=generic, won't apply local optimizations")
else()
	add_compile_options(-g3 -march=native)
endif()

add_subdirectory(omnictl)
add_subdirectory(omnicast)
add_subdirectory(proto)
add_subdirectory(libtrie)

if (UNIX AND NOT APPLE)
	add_subdirectory(wlr-clip)
endif()
