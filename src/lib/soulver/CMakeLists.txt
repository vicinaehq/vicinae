cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME vicinae-soulver)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT APPLE)
	message(FATAL_ERROR "vicinae-soulver can only be built on macOS")
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake)

include(FetchContent)

if (POLICY CMP0135)
	cmake_policy(SET CMP0135 NEW)
endif()

set(SOULVER_CORE_VERSION 3.5.0)
set(SOULVER_CORE_SHA256 b4a590dc75c6855c4a51dcb14a230333579aa5ad898e6825e697b8f02ebb55e7)

FetchContent_Declare(
	soulver_core
	URL https://github.com/soulverteam/SoulverCore/releases/download/${SOULVER_CORE_VERSION}/SoulverCore.xcframework.zip
	URL_HASH SHA256=${SOULVER_CORE_SHA256}
)
FetchContent_MakeAvailable(soulver_core)

set(SOULVER_CORE_SLICE_DIR ${soulver_core_SOURCE_DIR}/macos-arm64_x86_64)
if (NOT IS_DIRECTORY ${SOULVER_CORE_SLICE_DIR})
	set(SOULVER_CORE_SLICE_DIR ${soulver_core_SOURCE_DIR}/SoulverCore.xcframework/macos-arm64_x86_64)
endif()
if (NOT IS_DIRECTORY ${SOULVER_CORE_SLICE_DIR}/SoulverCore.framework)
	message(FATAL_ERROR "SoulverCore.framework not found under ${soulver_core_SOURCE_DIR}")
endif()

enable_language(Swift)

add_library(${PROJECT_NAME} STATIC
	src/soulver-wrapper.swift
	src/result-formatter.swift
	src/currency-provider.swift
)
add_library(vicinae::soulver ALIAS ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES Swift_MODULE_NAME "VicinaeSoulver")

if (CMAKE_OSX_ARCHITECTURES)
	set(SWIFT_TARGET_ARCH ${CMAKE_OSX_ARCHITECTURES})
else()
	set(SWIFT_TARGET_ARCH ${CMAKE_HOST_SYSTEM_PROCESSOR})
endif()

target_compile_options(${PROJECT_NAME} PRIVATE
	"$<$<COMPILE_LANGUAGE:Swift>:SHELL:-target ${SWIFT_TARGET_ARCH}-apple-macosx${CMAKE_OSX_DEPLOYMENT_TARGET}>"
	"$<$<COMPILE_LANGUAGE:Swift>:SHELL:-F ${SOULVER_CORE_SLICE_DIR}>")

target_link_options(${PROJECT_NAME} INTERFACE
	"SHELL:-F ${SOULVER_CORE_SLICE_DIR}"
	"LINKER:SHELL:-rpath ${SOULVER_CORE_SLICE_DIR}")
target_link_libraries(${PROJECT_NAME} PUBLIC "-framework SoulverCore" "-framework Foundation")

include(SwiftRuntime)
link_swift_runtime(${PROJECT_NAME})
