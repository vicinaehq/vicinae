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

set(WLR_CLIP_BIN "vicinae-wlr-clip")
set(LOCAL_WLR_CLIP_BIN "${CMAKE_BINARY_DIR}/wlr-clip/${WLR_CLIP_BIN}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_options(-g3)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
elseif(CMAKE_BUILD_TYPE STREQUAL "ReleaseHost")
	add_compile_options(-march=native)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	message(STATUS "Debug build, setting wlr-clip bin to ${LOCAL_WLR_CLIP_BIN}")
	add_compile_definitions(WLR_CLIP_BIN="${LOCAL_WLR_CLIP_BIN}")
else()
	add_compile_definitions(WLR_CLIP_BIN="${WLR_CLIP_BIN}")
endif()

add_subdirectory(vicinae)

if (UNIX AND NOT APPLE)
	add_subdirectory(wlr-clip)
endif()

set(INSTALL_SHARE ${CMAKE_INSTALL_PREFIX}/share)
set(VICINAE_INSTALL_SHARE ${INSTALL_SHARE}/vicinae)

make_directory(${CMAKE_INSTALL_PREFIX}/share/applications)
make_directory(${VICINAE_INSTALL_SHARE})

install(FILES ./extra/vicinae.desktop DESTINATION ${INSTALL_SHARE}/applications)
install(DIRECTORY ./extra/themes DESTINATION ${VICINAE_INSTALL_SHARE})
