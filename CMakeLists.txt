cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME vicinae)
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(ExtensionManager)
include(Git)

get_git_commit(GIT_COMMIT)
get_git_tag(GIT_TAG)

add_compile_definitions(VICINAE_GIT_TAG="${GIT_TAG}")
add_compile_definitions(VICINAE_GIT_COMMIT_HASH="${GIT_COMMIT}")

option(IGNORE_CCACHE "Always ignore ccache even if it is installed" OFF)
option(LTO "Enable Link Time Optimization (LTO). This will result in better performance, but greatly increased compile time. (Gentoo chads can't live without this)" OFF)
option(NOSTRIP "Never strip debug symbols from the binary, even in release mode. Note that symbols are never stripped for debug releases." OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Configured build type ${CMAKE_BUILD_TYPE}")

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release ReleaseHost)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

find_program(CCACHE ccache)
find_program(HAS_MOLD mold)

if (HAS_MOLD)
	message(STATUS "Using mold linker as it is present")
	add_link_options("-fuse-ld=mold")
endif()

if (LTO)
	message(STATUS "Link Time Optimization (LTO) is enabled")
	# gcc needs 'auto' to enable parallel compilation: https://stackoverflow.com/questions/72218980/gcc-v12-1-warning-about-serial-compilation
	add_compile_options(-flto=auto)
endif()

if (CCACHE AND NOT IGNORE_CCACHE) 
	message(STATUS "Found ccache installed, changing CMAKE_CXX_COMPILER_LAUNCHER")
	set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")
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
