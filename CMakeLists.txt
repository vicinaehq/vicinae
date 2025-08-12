cmake_minimum_required(VERSION 3.16)
set(VICINAE_VERSION v1.0.0)
set(PROJECT_NAME omnicast)
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(ExtensionManager)
include(Git)

get_git_commit(GIT_COMMIT)
get_git_tag(GIT_TAG)

add_compile_definitions(VICINAE_GIT_TAG="${GIT_TAG}")
add_compile_definitions(VICINAE_GIT_COMMIT_HASH="${GIT_COMMIT}")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Configured build type ${CMAKE_BUILD_TYPE}")

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release ReleaseHost)

find_program(CCACHE ccache)

if (CCACHE) 
	#set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_options(-g3)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
elseif(CMAKE_BUILD_TYPE STREQUAL "ReleaseHost")
	add_compile_options(-march=native)
endif()


#add_subdirectory(omnictl)
add_subdirectory(vicinae)
add_subdirectory(libtrie)

if (UNIX AND NOT APPLE)
	add_subdirectory(wlr-clip)
endif()

install(DIRECTORY ./extra/freedesktop-icon-themes/Reversal/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/Reversal)
install(DIRECTORY ./extra/freedesktop-icon-themes/Reversal-dark/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/Reversal-dark)
install(FILES ./extra/vicinae.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
