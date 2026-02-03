cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME snippet)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SNIPPET_SERVER_BIN vicinae-snippet-server)

OPTION(BUILD_TESTS "Build the test suite" OFF)

find_package(glaze)

add_library(snippet-types INTERFACE)
add_library(${LIB_NAMESPACE}::snippet-types ALIAS snippet-types)
target_include_directories(snippet-types INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(snippet-types INTERFACE cxx_std_23)
target_link_libraries(snippet-types INTERFACE vicinae::ipc glaze::glaze)

add_library(${PROJECT_NAME} STATIC src/server.cpp src/keyboard.cpp)
add_library(${LIB_NAMESPACE}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include PRIVATE src)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
target_link_libraries(${PROJECT_NAME} PUBLIC snippet-types xkbcommon udev)

add_executable(${SNIPPET_SERVER_BIN} src/main.cpp)
target_link_libraries(${SNIPPET_SERVER_BIN} PRIVATE ${PROJECT_NAME})
target_include_directories(${SNIPPET_SERVER_BIN} PRIVATE .)

install(TARGETS ${SNIPPET_SERVER_BIN}
	RUNTIME DESTINATION ${VICINAE_LIBEXEC_DIR}
)
