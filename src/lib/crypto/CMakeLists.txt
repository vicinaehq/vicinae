# native crypto primitives we can use when we need to encrypt something that is outside sqlcipher.
# One example is the clipboard history data, stored as files on the filesystem.
# This always uses the platform's native crypto backend (openssl on linux) so that we can benefit from system security updates to these critical libraries.

cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME vicinae-crypto)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake)

option(BUILD_TESTS "Build the test suite" OFF)

if (APPLE)
	enable_language(Swift)
	set(CRYPTO_BACKEND src/gcm-cryptokit.swift src/gcm-apple.cpp)
elseif (WIN32)
	set(CRYPTO_BACKEND src/gcm-stub.cpp)
else()
	set(CRYPTO_BACKEND src/gcm-openssl.cpp)
endif()

add_library(${PROJECT_NAME} STATIC src/aes-gcm.cpp src/kdf.cpp ${CRYPTO_BACKEND})
add_library(vicinae::crypto ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC include PRIVATE src)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

if (APPLE)
	set_target_properties(${PROJECT_NAME} PROPERTIES Swift_MODULE_NAME "VicinaeCrypto")
	if (CMAKE_OSX_ARCHITECTURES)
		set(SWIFT_TARGET_ARCH ${CMAKE_OSX_ARCHITECTURES})
	else()
		set(SWIFT_TARGET_ARCH ${CMAKE_HOST_SYSTEM_PROCESSOR})
	endif()
	target_compile_options(${PROJECT_NAME} PRIVATE
		"$<$<COMPILE_LANGUAGE:Swift>:SHELL:-target ${SWIFT_TARGET_ARCH}-apple-macosx${CMAKE_OSX_DEPLOYMENT_TARGET}>")
	target_link_libraries(${PROJECT_NAME} PUBLIC "-framework CryptoKit" "-framework Foundation")

	include(SwiftRuntime)
	link_swift_runtime(${PROJECT_NAME})
elseif (WIN32)
else()
	find_package(OpenSSL REQUIRED)
	target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::Crypto)
endif()

if (BUILD_TESTS)
	set(TEST_TARGET ${PROJECT_NAME}-tests)
	find_package(Catch2 3 REQUIRED)
	add_executable(${TEST_TARGET} tests/main.cpp)
	target_include_directories(${TEST_TARGET} PRIVATE src)
	target_link_libraries(${TEST_TARGET} PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})
endif()
