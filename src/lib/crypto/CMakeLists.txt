# native crypto primitives we can use when we need to encrypt something that is outside sqlcipher.
# One example is the clipboard history data, stored as files on the filesystem.

cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME vicinae-crypto)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build the test suite" OFF)

if (APPLE)
	set(CRYPTO_BACKEND src/gcm-commoncrypto.mm)
else()
	set(CRYPTO_BACKEND src/gcm-openssl.cpp)
endif()

add_library(${PROJECT_NAME} STATIC src/aes-gcm.cpp ${CRYPTO_BACKEND})
add_library(vicinae::crypto ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC include PRIVATE src)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

if (APPLE)
	set_source_files_properties(src/gcm-commoncrypto.mm PROPERTIES COMPILE_OPTIONS "-fobjc-arc")
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
