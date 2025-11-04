cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME xdgpp)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

OPTION(XDGPP_BUILD_TESTS "Build the xdgpp test suite" OFF)
OPTION(XDGPP_INSTALL "Install xdgpp library and headers" ON)

add_library(${PROJECT_NAME}
	xdgpp/locale/locale.cpp
	xdgpp/desktop-entry/reader.cpp
	xdgpp/desktop-entry/group.cpp
	xdgpp/desktop-entry/value.cpp
	xdgpp/desktop-entry/entry.cpp
	xdgpp/desktop-entry/file.cpp
	xdgpp/desktop-entry/action.cpp
	xdgpp/desktop-entry/exec.cpp
	xdgpp/desktop-entry/iterator.cpp
	xdgpp/env/env.cpp
	xdgpp/mime/iterator.cpp
	xdgpp/mime/mime-apps-list.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

if(XDGPP_INSTALL)
    install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
    )

    install(DIRECTORY xdgpp
        DESTINATION include
        FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
    )
endif()

if (XDGPP_BUILD_TESTS)
	set(TEST_TARGET tests)
	find_package(Catch2 3 REQUIRED)
	add_compile_options(-g3)
	add_executable(${TEST_TARGET} tests/main.cpp tests/entry.cpp tests/env.cpp tests/locale.cpp tests/mime.cpp tests/file.cpp tests/special.cpp)
	target_link_libraries(${TEST_TARGET} PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})
	target_include_directories(${TEST_TARGET} PRIVATE ./xdgpp)
endif()
