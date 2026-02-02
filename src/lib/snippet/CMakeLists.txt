cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME snippet)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

OPTION(BUILD_TESTS "Build the test suite" OFF)

find_package(glaze)

# static lib
add_library(${PROJECT_NAME}  STATIC src/server.cpp src/keyboard.cpp)
add_library(${LIB_NAMESPACE}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include PRIVATE src)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
target_link_libraries(${PROJECT_NAME} PUBLIC vicinae-ipc glaze::glaze xkbcommon udev)

if (BUILD_TESTS)
	set(TEST_TARGET ${PROJECT_NAME}-tests)
	find_package(Catch2 3 REQUIRED)
	add_executable(${TEST_TARGET} tests/main.cpp)
	target_link_libraries(${TEST_TARGET} PRIVATE ${PROJECT_NAME})
	target_include_directories(${TEST_TARGET} PRIVATE .)
endif()
