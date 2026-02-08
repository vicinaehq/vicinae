cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME vicinae)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES C CXX)

set(LIB_NAMESPACE "vicinae")

if (QT_DIR)
	set(CMAKE_PREFIX_PATH ${QT_DIR} ${CMAKE_PREFIX_PATH})
	message(STATUS "Using QT installation at ${QT_DIR}")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
include(GNUInstallDirs)
include(Git)
include(Utils)

# Viciane build configuration options

option(BUILD_TESTS "Build test suites for various vicinae components" OFF)
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
option(USE_SYSTEM_LAYER_SHELL "Use system qt layer shell instead of building it from source" ON)
option(USE_SYSTEM_GLAZE "Use system glaze instead of fetching it. This is a header only library so only required at build time" OFF)
option(USE_SYSTEM_QT_KEYCHAIN "Use system qt-keychain instead of building it from source. Note: still depends on system libsecret." ON)
option(LIBQALCULATE_BACKEND "Compile in support for the Qalculate! calculator backend" ON)
option(ENABLE_SANITIZERS "Enable ASan + UBSan for debug builds" OFF)
option(ENABLE_PREVIEW_FEATURES "Enable preview features that are not yet ready for global release" OFF)

# disabled for now, as it seems to be incompatible with ccache, and as such slows us down more than anything
option(USE_PRECOMPILED_HEADERS "Use precompiled headers to speed up compilation" OFF)

set(LIB_DIR "${CMAKE_SOURCE_DIR}/src/lib")
set(EXTRA_DIR "${CMAKE_SOURCE_DIR}/extra")
set(THEME_DIR "${EXTRA_DIR}/themes")
set(VICINAE_LIBEXEC_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBEXECDIR}/vicinae)
set(VICINAE_LIBEXEC_DIR ${CMAKE_INSTALL_LIBEXECDIR}/vicinae)

include(Glaze)

if (NOT USE_SYSTEM_GLAZE)
	checkout_glaze()
endif()

if (APPLE)
	set(WAYLAND_LAYER_SHELL OFF)
	set(WLR_DATA_CONTROL OFF)
endif()

include_directories(
    ${CMAKE_BINARY_DIR}/generated  # For version.h
)

add_subdirectory(src/browser-extension)
add_subdirectory(src/snippet)
add_subdirectory(${LIB_DIR}/script-command)
add_subdirectory(${LIB_DIR}/emoji)
add_subdirectory(${LIB_DIR}/xdgpp)
add_subdirectory(${LIB_DIR}/vicinae-ipc)
add_subdirectory(${LIB_DIR}/common)

set(PROTO_DIR ${CMAKE_SOURCE_DIR}/src/proto)

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
	set(USE_SYSTEM_LAYER_SHELL OFF)
	set(USE_SYSTEM_QT_KEYCHAIN OFF)
	set(LIBQALCULATE_BACKEND OFF)
endif()


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

# Generate version header from template
configure_file(
	"${CMAKE_SOURCE_DIR}/cmake/version.h.in"
	"${CMAKE_BINARY_DIR}/generated/version.h"
	@ONLY
)

set(CMARK_LIBRARY "cmark-gfm")
set(CMARK_EXT_LIBRARY "cmark-gfm-extensions")

if (NOT USE_SYSTEM_QT_KEYCHAIN)
	include(QtKeychain)
	checkout_qt_keychain()
endif()

if (NOT USE_SYSTEM_LAYER_SHELL)
	include(QtLayerShell)
	checkout_layer_shell_qt()
endif()

if (NOT USE_SYSTEM_CMARK_GFM)
	include(CMark)
	checkout_cmark()
  set(CMARK_LIBRARY "libcmark-gfm_static")
  set(CMARK_EXT_LIBRARY "libcmark-gfm-extensions_static")
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

find_package(glaze)

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)
find_package(Protobuf)
unset(CMAKE_FIND_PACKAGE_PREFER_CONFIG)

if (TYPESCRIPT_EXTENSIONS)
	add_subdirectory(src/typescript)
	add_compile_definitions(HAS_TYPESCRIPT_EXTENSIONS=1)
endif()

if (ENABLE_PREVIEW_FEATURES)
	add_compile_definitions(ENABLE_PREVIEW_FEATURES=1)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Configured build type ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELEASE} -g3 -O0")

if (ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE MATCHES "Debug")
	message(STATUS "Sanitizers enabled (ASan + UBSan)")
	# glaze has issues with ubsan+variants
	#add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
	add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
	add_link_options(-fsanitize=address)
	add_compile_definitions(DEBUG=1)
endif()

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release ReleaseHost)

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
	add_compile_definitions(DEBUG=1)
else()
	add_compile_definitions(DEBUG=0)
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

add_compile_definitions(BUILD_INFO="${BUILD_INFO}")
add_subdirectory(src/server)
add_subdirectory(src/cli)

if (UNIX AND NOT APPLE)
	add_subdirectory(src/data-control-server)
endif()

set(INSTALL_SHARE ${CMAKE_INSTALL_PREFIX}/share)
set(VICINAE_INSTALL_SHARE ${INSTALL_SHARE}/vicinae)
set(VICINAE_THEME_DIR ${VICINAE_INSTALL_SHARE}/themes)

make_directory(${VICINAE_INSTALL_SHARE})
file(REMOVE_RECURSE ${VICINAE_THEME_DIR})
install(DIRECTORY ./extra/themes DESTINATION ${VICINAE_INSTALL_SHARE})

if (UNIX AND NOT APPLE)
	set(APP_DIR ${INSTALL_SHARE}/applications)
	make_directory(${APP_DIR})
	install(FILES ./extra/vicinae.desktop DESTINATION ${APP_DIR})
	install(FILES ./extra/vicinae-url-handler.desktop DESTINATION ${APP_DIR})

	set(ICON_DIR ${INSTALL_SHARE}/icons/hicolor/512x512/apps)
	make_directory(${ICON_DIR})
	install(FILES ./extra/vicinae.png DESTINATION ${ICON_DIR})

	set(SYSTEMD_USER_DIR ${CMAKE_INSTALL_PREFIX}/lib/systemd/user)
	make_directory(${SYSTEMD_USER_DIR})
	install(FILES ./extra/vicinae.service DESTINATION ${SYSTEMD_USER_DIR})
endif()
