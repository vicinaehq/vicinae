cmake_minimum_required(VERSION 3.16)
set(VICINAE_VERSION v1.0.0)
set(PROJECT_NAME omnicast)
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(ExtensionManager)

add_compile_definitions(VICINAE_VERSION="${VICINAE_VERSION}")

find_program(CCACHE ccache)

if (CCACHE) 
	#set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")
endif()

if ("$ENV{DIST_TYPE}" STREQUAL "generic")
	message(STATUS "DIST_TYPE=generic, won't apply local optimizations")
else()
	add_compile_options(-march=native -g3)
endif()

add_subdirectory(omnictl)
add_subdirectory(vicinae)
add_subdirectory(proto)
add_subdirectory(libtrie)

if (UNIX AND NOT APPLE)
	add_subdirectory(wlr-clip)
endif()

install(DIRECTORY ./extra/freedesktop-icon-themes/Reversal/ DESTINATION /usr/share/icons/Reversal)
install(DIRECTORY ./extra/freedesktop-icon-themes/Reversal-dark/ DESTINATION /usr/share/icons/Reversal-dark)
