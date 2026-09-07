cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME xdgpp)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build the xdgpp test suite" OFF)

add_library(vicinae-xdgpp STATIC
	xdgpp/locale/locale.cpp
	xdgpp/desktop-entry/reader.cpp
	xdgpp/desktop-entry/group.cpp
	xdgpp/desktop-entry/value.cpp
	xdgpp/desktop-entry/entry.cpp
	xdgpp/desktop-entry/file.cpp
	xdgpp/desktop-entry/action.cpp
	xdgpp/desktop-entry/exec.cpp
	xdgpp/env/env.cpp
	xdgpp/mime/iterator.cpp
	xdgpp/mime/mime-apps-list.cpp
	xdgpp/mime/mime-apps-list-writer.cpp
	xdgpp/uri/file-uri.cpp
	xdgpp/bookmark/bookmark-file.cpp
)
add_library(vicinae::xdgpp ALIAS vicinae-xdgpp)
set(PROJECT_NAME vicinae-xdgpp)

target_link_libraries(${PROJECT_NAME} PRIVATE pugixml)
target_include_directories(${PROJECT_NAME} PRIVATE $<TARGET_PROPERTY:pugixml,SOURCE_DIR>)

add_compile_definitions(XDGPP_GROUP="xdgpp")

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (BUILD_TESTS)
	set(TEST_TARGET xdgpp-tests)
	set(FIXTURE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/fixtures)
	find_package(Catch2 3 REQUIRED)
	add_compile_options(-g3)
	add_executable(${TEST_TARGET} tests/main.cpp tests/entry.cpp tests/env.cpp tests/locale.cpp tests/mime.cpp tests/file.cpp tests/special.cpp tests/xdg-terminal-exec.cpp tests/bookmark.cpp tests/file-uri.cpp)
	target_compile_definitions(${TEST_TARGET} PRIVATE
		XDGPP_FIXTURE_DIR="${FIXTURE_DIR}"
	)
	target_link_libraries(${TEST_TARGET} PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})
	target_include_directories(${TEST_TARGET} PRIVATE ./xdgpp)
endif()
