cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME vicinae)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES C CXX)

if (QT_DIR)
	set(CMAKE_PREFIX_PATH ${QT_DIR} ${CMAKE_PREFIX_PATH})
	message(STATUS "Using QT installation at ${QT_DIR}")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Viciane build configuration options

option(IGNORE_CCACHE "Always ignore ccache even if it is installed" OFF)
option(LTO "Enable Link Time Optimization (LTO). This will result in better performance, but greatly increased compile time. (Gentoo chads can't live without this)" OFF)
option(NOSTRIP "Never strip debug symbols from the binary, even in release mode. Note that symbols are never stripped for debug releases." OFF)
option(
	INSTALL_NODE_MODULES 
	"Install required node_modules dependencies as part of the build process. 
	You can turn this off if you have another way to install the node_modules,
	but they are still required build the project." 
	ON
)

option(WAYLAND_LAYER_SHELL "Enable support for the layer shell Wayland protocol" ON)
option(TYPESCRIPT_EXTENSIONS "Enable support for extensions built with React/Typescript (no browser involved)" ON)

option(PREFER_STATIC_LIBS "When available, link to a library statically instead of dynamically" OFF)
option(USE_SYSTEM_PROTOBUF "Use system protobuf instead of building it from source" ON)
option(USE_SYSTEM_ABSEIL "Use system abseil (libabsl) instead of building it from source" ON)
option(USE_SYSTEM_CMARK_GFM "Use system cmark-gfm (github's fork of cmark) instead of building it from source" ON)
option(USE_SYSTEM_MINIZIP "Use system minizip instead of building it from source" ON)
option(USE_SYSTEM_LAYER_SHELL "Use system qt layer shell instead of building it from source" ON)
option(USE_SYSTEM_QT_KEYCHAIN "Use system qt-keychain instead of building it from source. Note: still depends on system libsecret." ON)
option(USE_SYSTEM_RAPIDFUZZ "Use system version of the rapidfuzz-cpp header only library" ON)
option(LIBQALCULATE_BACKEND "Compile in support for the Qalculate! calculator backend" ON)
option(USE_PRECOMPILED_HEADERS "Use precompiled headers to speed up compilation" ON)

option(BUILD_TESTS "Build all vicinae tests" OFF)

if (BUILD_TESTS)
	add_subdirectory(tests)
endif()

if (APPLE)
	set(WAYLAND_LAYER_SHELL OFF)
endif()

if (UNIX AND NOT APPLE)
	add_subdirectory("./lib/xdgpp")
endif()

if (USE_SYSTEM_PROTOBUF)
	set(USE_SYSTEM_ABSEIL ON)
endif()

# End of Vicinae build configuration options

if (PREFER_STATIC_LIBS)
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".so" ".dylib")
	set(BUILD_SHARED_LIBS OFF)
	set(USE_SYSTEM_PROTOBUF OFF)
	set(USE_SYSTEM_ABSEIL OFF)
	set(USE_SYSTEM_CMARK_GFM OFF)
	set(USE_SYSTEM_MINIZIP OFF)
	set(USE_SYSTEM_LAYER_SHELL OFF)
	set(USE_SYSTEM_QT_KEYCHAIN OFF)
	set(USE_SYSTEM_RAPIDFUZZ OFF)
	set(LIBQALCULATE_BACKEND OFF)
endif()

include(Git)
include(Utils)


if (NOT DEFINED VICINAE_GIT_TAG)
	get_git_tag(VICINAE_GIT_TAG)
endif()

if (NOT DEFINED VICINAE_GIT_COMMIT_HASH)
	get_git_commit(VICINAE_GIT_COMMIT_HASH)
endif()

if (DEFINED VICINAE_PROVENANCE)
	message(STATUS "Provenance was explicitly set to ${VICINAE_PROVENANCE}")
else()
	set(VICINAE_PROVENANCE "local")
endif()

message(STATUS "Vicinae version: ${VICINAE_GIT_TAG}")
message(STATUS "Vicinae commit hash: ${VICINAE_GIT_COMMIT_HASH}")

get_cxx_compiler_name(CXX_COMPILER_NAME)

add_compile_definitions(VICINAE_GIT_TAG="${VICINAE_GIT_TAG}")
add_compile_definitions(VICINAE_GIT_COMMIT_HASH="${VICINAE_GIT_COMMIT_HASH}")
add_compile_definitions(VICINAE_PROVENANCE="${VICINAE_PROVENANCE}")

set(CMARK_LIBRARY "cmark-gfm")

if (NOT USE_SYSTEM_RAPIDFUZZ)
	include(RapidFuzz)
	checkout_rapidfuzz()
	# there is probably a better way, but the headers are not part of the build output
	# so cmake won't find them
	include_directories(${CMAKE_CURRENT_BINARY_DIR}/_deps/rapidfuzz-src/)
endif()

if (NOT USE_SYSTEM_QT_KEYCHAIN)
	include(QtKeychain)
	checkout_qt_keychain()
endif()

if (NOT USE_SYSTEM_LAYER_SHELL)
	include(QtLayerShell)
	checkout_layer_shell_qt()
endif()

if (NOT USE_SYSTEM_MINIZIP)
	include(Minizip)
	checkout_minizip()
endif()

if (NOT USE_SYSTEM_CMARK_GFM)
	include(CMark)
	checkout_cmark()
	set(CMARK_LIBRARY "libcmark-gfm_static")
endif()

if (NOT USE_SYSTEM_ABSEIL)
	include(Abseil)
	checkout_abseil()
endif()

if (NOT USE_SYSTEM_PROTOBUF)
	include(Protobuf)
	checkout_protobuf()
	set(COMMON_PROTO_DIR ${CMAKE_CURRENT_BINARY_DIR}/_deps/protobuf-src/src)
endif()

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)
find_package(Protobuf)
unset(CMAKE_FIND_PACKAGE_PREFER_CONFIG)

if (TYPESCRIPT_EXTENSIONS)
	add_subdirectory(typescript)
	add_compile_definitions(HAS_TYPESCRIPT_EXTENSIONS=1)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Configured build type ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release ReleaseHost)

if (NOT NOSTRIP AND NOT ${CMAKE_BUILD_TYPE} MATCHES "Debug")
	#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
	message(STATUS "Built binaries will be stripped")
endif()

set(BUILD_INFO "${CXX_COMPILER_NAME} ${CMAKE_CXX_COMPILER_VERSION} - ${CMAKE_BUILD_TYPE}")

if (LIBQALCULATE_BACKEND)
	message(STATUS "libqalculate! calculator backend is enabled")
endif()

find_program(CCACHE ccache)
find_program(HAS_MOLD mold)

if (HAS_MOLD)
	message(STATUS "Using mold linker as it is present")
	add_link_options("-fuse-ld=mold")
endif()

if (LTO)
	set(BUILD_INFO "${BUILD_INFO} - LTO")
	message(STATUS "Link Time Optimization (LTO) is enabled")
	# gcc needs 'auto' to enable parallel compilation: https://stackoverflow.com/questions/72218980/gcc-v12-1-warning-about-serial-compilation
	add_compile_options(-flto=auto)
	add_link_options(-flto=auto)
endif()

if (CCACHE AND NOT IGNORE_CCACHE) 
	message(STATUS "ccache is installed, changing CMAKE_CXX_COMPILER_LAUNCHER")
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

add_compile_definitions(BUILD_INFO="${BUILD_INFO}")

add_subdirectory(vicinae)

if (UNIX AND NOT APPLE)
	add_subdirectory(wlr-clip)
endif()

set(INSTALL_SHARE ${CMAKE_INSTALL_PREFIX}/share)
set(VICINAE_INSTALL_SHARE ${INSTALL_SHARE}/vicinae)
set(VICINAE_THEME_DIR ${VICINAE_INSTALL_SHARE}/themes)

make_directory(${VICINAE_INSTALL_SHARE})
file(REMOVE_RECURSE ${VICINAE_THEME_DIR})
install(DIRECTORY ./extra/themes DESTINATION ${VICINAE_INSTALL_SHARE})

if (UNIX AND NOT APPLE)
	# install desktop file
	set(APP_DIR ${INSTALL_SHARE}/applications)
	make_directory(${APP_DIR})
	install(FILES ./extra/vicinae.desktop DESTINATION ${APP_DIR})
endif()



