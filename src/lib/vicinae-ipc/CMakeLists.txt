cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME vicinae-ipc)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

OPTION(BUILD_TESTS "Build the test suite" OFF)

find_package(glaze)

if (!glaze_FOUND)
	include(FetchContent)

	FetchContent_Declare(
	  glaze
	  GIT_REPOSITORY https://github.com/stephenberry/glaze.git
	  GIT_TAG b64f4a68d3a08e6515a9322a9740cf652ade0bb7
	  GIT_SHALLOW FALSE
	  EXCLUDE_FROM_ALL
	)
	FetchContent_MakeAvailable(glaze)
endif()

add_library(${PROJECT_NAME} STATIC src/ipc.cpp)
add_library(vicinae::ipc ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include PRIVATE src)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
target_link_libraries(${PROJECT_NAME} PUBLIC glaze::glaze)

if (BUILD_TESTS)
	set(TEST_TARGET ${PROJECT_NAME}-tests)
	find_package(Catch2 3 REQUIRED)
	add_executable(${TEST_TARGET} tests/main.cpp)
	target_link_libraries(${TEST_TARGET} PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})
	target_include_directories(${TEST_TARGET} PRIVATE .)
endif()
