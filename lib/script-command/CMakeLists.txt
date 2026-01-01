cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME scriptcommand)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(TEST_TARGET tests)
find_package(Catch2 3 REQUIRED)

find_package(glaze)

if (!glaze_FOUND)
	include(FetchContent)

	FetchContent_Declare(
	  glaze
	  GIT_REPOSITORY https://github.com/stephenberry/glaze.git
	  GIT_TAG v6.4.0  # Replace with desired version
	  GIT_SHALLOW TRUE
	  EXCLUDE_FROM_ALL
	)
	FetchContent_MakeAvailable(glaze)
endif()

# static lib
add_library(${PROJECT_NAME} STATIC src/script-command.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_link_libraries(${PROJECT_NAME} PUBLIC glaze::glaze)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_26)

# tests
add_compile_definitions(${TEST_TARGET} FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures")
add_executable(${TEST_TARGET} tests/main.cpp tests/failure.cpp tests/edge-cases.cpp tests/time-parsing.cpp tests/argument-validation.cpp tests/fields.cpp tests/comment-styles.cpp)
target_link_libraries(${TEST_TARGET} PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})
target_include_directories(${TEST_TARGET} PRIVATE .)
