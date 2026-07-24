cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME vicinae)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "14.4" CACHE STRING "Minimum macOS version to target")
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES C CXX)

if (WIN32 AND NOT MSVC)
	message(FATAL_ERROR "Windows builds are only supported with MSVC (got '${CMAKE_CXX_COMPILER_ID}'). Use the windows-* presets or set -DCMAKE_CXX_COMPILER=cl.")
endif()

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

# Vicinae build configuration options

option(BUILD_TESTS "Build test suites for various vicinae components" OFF)
option(IGNORE_CCACHE "Always ignore ccache/sccache even if installed" OFF)
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

# Default for the USE_SYSTEM_* options below.
# On linux we generally prefer linking to system libraries by default.

set(USE_SYSTEM_DEFAULT ON)
if (APPLE OR WIN32)
	set(USE_SYSTEM_DEFAULT OFF)
endif()

option(USE_SYSTEM_CMARK_GFM "Use system cmark-gfm (github's fork of cmark) instead of the vendored copy under vendor/cmark-gfm" ${USE_SYSTEM_DEFAULT})
option(USE_SYSTEM_LAYER_SHELL "Use system qt layer shell instead of building it from source" ON)
option(USE_SYSTEM_KF6 "Use system KDE syntax highlighting library for markdown code block highlight support" ${USE_SYSTEM_DEFAULT})
option(USE_SYSTEM_GLAZE "Use system glaze instead of fetching it. This is a header only library so only required at build time" OFF)
option(USE_SYSTEM_QT_KEYCHAIN "Use system qt-keychain instead of building it from source. Note: still depends on system libsecret." ${USE_SYSTEM_DEFAULT})
option(ENABLE_SANITIZERS "Enable ASan + UBSan for debug builds" OFF)
option(ENABLE_CLANG_TIDY "Run clang-tidy during compilation" OFF)
option(INSTALL_MODULES_LOAD_CONFIG "Install config files to auto load required kernel modules" ON)
option(AUTO_INSTALL_BROWSER_MANIFESTS "Install per-user browser native messaging host manifests at server startup" ON)
option(AUTO_ENABLE_AUTOSTART "Register the app to start at login on first launch (currently macOS only)" ON)
option(ENABLE_ONBOARDING "Show the onboarding window on first launch (macOS only)" ON)

set(BUNDLE_SOULVER_CORE_DEFAULT OFF)
if (APPLE)
	set(BUNDLE_SOULVER_CORE_DEFAULT ON)
endif()

option(
	BUNDLE_SOULVER_CORE
	"Download the SoulverCore framework and bundle it with the app, making it the default calculator backend (macOS only)"
	${BUNDLE_SOULVER_CORE_DEFAULT}
)

set(VICINAE_BUNDLE_ID "com.vicinaehq.Vicinae" CACHE STRING "macOS app bundle identifier")

set(NATIVE_MESSAGING_HOST "com.vicinae.vicinae")
set(CHROME_EXTENSION_ID "kcmipingpfbohfjckomimmahknoddnke") # https://chromewebstore.google.com/detail/vicinae-integration/kcmipingpfbohfjckomimmahknoddnke
set(FIREFOX_EXTENSION_ID "firefox@vicinae.com")

if (ENABLE_CLANG_TIDY)
	set(CMAKE_CXX_CLANG_TIDY clang-tidy)
endif()

# disabled for now, as it seems to be incompatible with ccache, and as such slows us down more than anything
option(USE_PRECOMPILED_HEADERS "Use precompiled headers to speed up compilation" OFF)

set(LIB_DIR "${CMAKE_SOURCE_DIR}/src/lib")
set(EXTRA_DIR "${CMAKE_SOURCE_DIR}/extra")
set(THEME_DIR "${EXTRA_DIR}/themes")
set(VICINAE_LIBEXEC_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBEXECDIR}/vicinae)
set(VICINAE_LIBEXEC_DIR ${CMAKE_INSTALL_LIBEXECDIR}/vicinae)

if (APPLE)
	if (NOT QT_DIR AND NOT CMAKE_PREFIX_PATH)
		file(GLOB _qt_candidates "$ENV{HOME}/Qt/*/macos/lib/cmake/Qt6")
		if (_qt_candidates)
			list(SORT _qt_candidates COMPARE NATURAL ORDER DESCENDING)
			list(GET _qt_candidates 0 _qt_cmake_dir)
			get_filename_component(_qt_prefix "${_qt_cmake_dir}/../../.." ABSOLUTE)
			list(PREPEND CMAKE_PREFIX_PATH "${_qt_prefix}")
			message(STATUS "Using Qt from ${_qt_prefix}")
		endif()
	endif()

	find_program(BREW_EXECUTABLE brew)
	if (BREW_EXECUTABLE)
		execute_process(
			COMMAND ${BREW_EXECUTABLE} --prefix
			OUTPUT_VARIABLE HOMEBREW_PREFIX
			OUTPUT_STRIP_TRAILING_WHITESPACE
			RESULT_VARIABLE _brew_status
		)
		if (NOT _brew_status EQUAL 0)
			set(HOMEBREW_PREFIX "")
		endif()
	endif()

	if (HOMEBREW_PREFIX)
		message(STATUS "Detected Homebrew prefix: ${HOMEBREW_PREFIX}")
		if (NOT QT_DIR AND NOT CMAKE_PREFIX_PATH AND EXISTS "${HOMEBREW_PREFIX}/opt/qt/lib/cmake/Qt6")
			list(PREPEND CMAKE_PREFIX_PATH "${HOMEBREW_PREFIX}/opt/qt")
		endif()
		include_directories(${HOMEBREW_PREFIX}/include)
		link_directories(${HOMEBREW_PREFIX}/lib)
	endif()
endif()

if (APPLE OR WIN32)
	set(WAYLAND_LAYER_SHELL OFF)
	set(WLR_DATA_CONTROL OFF)
endif()

# the Qt deploy API registers its support files on the first Qt6::Core scope seen
find_package(Qt6 REQUIRED COMPONENTS Core Qml)

if (NOT USE_SYSTEM_GLAZE)
	include(Glaze)
	import_glaze()
endif()

if (NOT USE_SYSTEM_KF6 AND NOT WIN32)
	include(KF6)
	import_kf6()
endif()

set(VENDOR_DIR ${CMAKE_SOURCE_DIR}/vendor)

include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/vendor
)

find_package(glaze)

add_subdirectory(src/browser-extension)
add_subdirectory(${LIB_DIR}/common)
add_subdirectory(${LIB_DIR}/script-command)
add_subdirectory(${LIB_DIR}/glyph)
if (NOT WIN32)
	add_subdirectory(${LIB_DIR}/xdgpp)
endif()
add_subdirectory(${LIB_DIR}/fuzzy)
add_subdirectory(${LIB_DIR}/crypto)

if (APPLE AND BUNDLE_SOULVER_CORE)
	add_subdirectory(${LIB_DIR}/soulver)
endif()

# statically linked vendored libs.
# Most of the other vendored libs are header only so no need for a CMakeLists.txt
add_subdirectory(${VENDOR_DIR}/sqlcipher)
add_subdirectory(${VENDOR_DIR}/pugixml)
add_subdirectory(${VENDOR_DIR}/zip)

if (UNIX AND NOT APPLE)
	add_subdirectory(${LIB_DIR}/linux-utils)
	add_subdirectory(src/snippet)
	add_subdirectory(src/file-indexer)
endif()
add_subdirectory(${LIB_DIR}/figura)

set(FIGURA_DIR ${CMAKE_SOURCE_DIR}/figura)


# End of Vicinae build configuration options

if (PREFER_STATIC_LIBS)
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".so" ".dylib")
	set(BUILD_SHARED_LIBS OFF)
	set(USE_SYSTEM_CMARK_GFM OFF)
	set(USE_SYSTEM_LAYER_SHELL OFF)
	set(USE_SYSTEM_QT_KEYCHAIN OFF)
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

set(VICINAE_NODE_RUNTIME_VERSION "v22.23.1" CACHE STRING "Pinned node runtime used to run extensions")

if (APPLE)
	# we only support apple silicon macs
	set(_node_runtime_platform "darwin-arm64")
	set(_node_runtime_sha256 "ef28d8fab2c0e4314522d4bb1b7173270aa3937e93b92cb7de79c112ac1fa953")
elseif (WIN32)
	if (CMAKE_SYSTEM_PROCESSOR MATCHES "ARM64|aarch64")
		set(_node_runtime_platform "win-arm64")
		set(_node_runtime_sha256 "b470fdfe3502c05151656e06d495e3f47544f2ee8b1d9c8705090f2dd5996bd0")
	else()
		set(_node_runtime_platform "win-x64")
		set(_node_runtime_sha256 "7df0bc9375723f4a86b3aa1b7cc73342423d9677a8df4538aca31a049e309c29")
	endif()
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
	set(_node_runtime_platform "linux-arm64")
	set(_node_runtime_sha256 "543fa39e57d4c07855939459a323f4deb9a79dd1bb45e6e99458b0f2de10db8d")
else()
	set(_node_runtime_platform "linux-x64")
	set(_node_runtime_sha256 "7a8cb04b4a1df4eaf432125324b81b29a088e73570a23259a8de1c65d07fc129")
endif()

if (WIN32)
	set(_node_runtime_archive_ext "zip")
else()
	set(_node_runtime_archive_ext "tar.gz")
endif()

set(VICINAE_NODE_RUNTIME_SHA256 "${_node_runtime_sha256}" CACHE STRING "sha256 of the node runtime archive")
set(VICINAE_NODE_RUNTIME_URL
	"https://nodejs.org/dist/${VICINAE_NODE_RUNTIME_VERSION}/node-${VICINAE_NODE_RUNTIME_VERSION}-${_node_runtime_platform}.${_node_runtime_archive_ext}"
	CACHE STRING "URL of the node runtime archive")
set(VICINAE_NODE_RUNTIME_TAR_MEMBER
	"node-${VICINAE_NODE_RUNTIME_VERSION}-${_node_runtime_platform}/bin/node")

# self-distributed builds (dmg, appimage, windows) always download the pinned runtime
# so the node version never depends on what the host has installed; package manager
# builds use the system node unless this is explicitly turned on
if (APPLE OR WIN32 OR VICINAE_PROVENANCE STREQUAL "appimage")
	set(_node_runtime_download_default ON)
else()
	set(_node_runtime_download_default OFF)
endif()
option(VICINAE_NODE_RUNTIME_DOWNLOAD "Run extensions with the pinned downloaded node runtime"
	${_node_runtime_download_default})

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
	setup_vendored_cmark()
  set(CMARK_LIBRARY "libcmark-gfm_static")
  set(CMARK_EXT_LIBRARY "libcmark-gfm-extensions_static")
endif()

if (TYPESCRIPT_EXTENSIONS)
	add_subdirectory(src/typescript)
	add_compile_definitions(HAS_TYPESCRIPT_EXTENSIONS=1)
endif()


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Configured build type ${CMAKE_BUILD_TYPE}")

if (MSVC)
	# /Zc:preprocessor and /bigobj are required by glaze's compile-time reflection
	add_compile_options(/permissive- /Zc:preprocessor /Zc:__cplusplus /bigobj /utf-8 /W3 /wd4996)
	# keep <windows.h> from leaking min/max and other macros into the codebase
	add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)
else()
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0")

	add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-missing-field-initializers)
endif()

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

find_program(CCACHE ccache)
find_program(SCCACHE sccache)

if (LTO)
	set(BUILD_INFO "${BUILD_INFO} - LTO")
	message(STATUS "Link Time Optimization (LTO) is enabled")
	if (MSVC)
		include(CheckIPOSupported)
		check_ipo_supported(RESULT _ipo_ok OUTPUT _ipo_msg)
		if (_ipo_ok)
			set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
		else()
			message(WARNING "IPO/LTCG unsupported: ${_ipo_msg}")
		endif()
	else()
		add_compile_options(-flto=auto)
		add_link_options(-flto=auto)
	endif()
endif()

if (NOT IGNORE_CCACHE)
	if (CCACHE)
		set(COMPILER_CACHE "${CCACHE}")
	elseif (SCCACHE)
		set(COMPILER_CACHE "${SCCACHE}")
	endif()

	if (COMPILER_CACHE)
		message(STATUS "Using compiler cache: ${COMPILER_CACHE}")
		set(CMAKE_C_COMPILER_LAUNCHER "${COMPILER_CACHE}")
		set(CMAKE_CXX_COMPILER_LAUNCHER "${COMPILER_CACHE}")
	endif()
endif()

add_compile_definitions(BUILD_INFO="${BUILD_INFO}")
add_subdirectory(src/cli)
add_subdirectory(src/server)

if (UNIX AND NOT APPLE)
	add_subdirectory(src/data-control-server)
endif()

set(INSTALL_SHARE ${CMAKE_INSTALL_PREFIX}/share)
set(INSTALL_MODULES ${CMAKE_INSTALL_PREFIX}/lib/modules-load.d)
set(VICINAE_INSTALL_SHARE ${INSTALL_SHARE}/vicinae)
set(VICINAE_THEME_DIR ${VICINAE_INSTALL_SHARE}/themes)

if(WIN32)
	install(DIRECTORY ${THEME_DIR} DESTINATION .)
	set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR})
	include(InstallRequiredSystemLibraries)
elseif(NOT APPLE)
	make_directory(${VICINAE_INSTALL_SHARE})
	file(REMOVE_RECURSE ${VICINAE_THEME_DIR})
	install(DIRECTORY ./extra/themes DESTINATION ${VICINAE_INSTALL_SHARE})
endif()

if (UNIX AND NOT APPLE)
	if (INSTALL_MODULES_LOAD_CONFIG)
		install(DIRECTORY ${EXTRA_DIR}/modules-load.d/ DESTINATION ${INSTALL_MODULES})
	endif()

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

# CPack .deb package configuration
string(REGEX REPLACE "^v" "" CPACK_PACKAGE_VERSION "${VICINAE_GIT_TAG}")
if(CPACK_PACKAGE_VERSION STREQUAL "")
	set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
endif()
set(CPACK_PACKAGE_NAME "vicinae")
set(CPACK_PACKAGE_VENDOR "vicinaehq")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A focused launcher for your desktop")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "vicinaehq <contact@vicinaehq.com>")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/vicinaehq/vicinae")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
include(CPack)
