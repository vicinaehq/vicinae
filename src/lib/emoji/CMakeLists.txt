cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME emoji)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

OPTION(BUILD_TESTS "Build the test suite" OFF)

#find_package(ICU REQUIRED COMPONENTS uc)

add_library(vicinae-emoji STATIC src/emoji.cpp src/generated/db.cpp)
add_library(vicinae::emoji ALIAS vicinae-emoji)
set(PROJECT_NAME vicinae-emoji)
target_include_directories(${PROJECT_NAME} PUBLIC include PRIVATE src )

# we install icu4c using homebrew. macOS has libicucore.dylib, maybe we should use that.
if (APPLE)
	target_include_directories(${PROJECT_NAME} PRIVATE /opt/homebrew/opt/icu4c@78/include)
	target_link_directories(${PROJECT_NAME} PUBLIC /opt/homebrew/opt/icu4c@78/lib)
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
target_link_libraries(${PROJECT_NAME} PUBLIC icuuc)

if (BUILD_TESTS)
	set(TEST_TARGET ${PROJECT_NAME}-tests)
	find_package(Catch2 3 REQUIRED)
	add_executable(${TEST_TARGET} tests/main.cpp tests/tones.cpp tests/special-cases.cpp)
	target_link_libraries(${TEST_TARGET} PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})
	target_include_directories(${TEST_TARGET} PRIVATE .)
endif()
