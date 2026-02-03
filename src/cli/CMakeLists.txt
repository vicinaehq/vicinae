cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME vicinae) # the cli is the "vicinae" binary, a single entrypoint for everything
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} src/main.cpp src/cli.cpp src/server.cpp src/theme.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
target_compile_definitions(${PROJECT_NAME} PRIVATE
  DEFAULT_CONFIG_PATH="${EXTRA_DIR}/config.jsonc"
  THEME_TEMPLATE_PATH="${EXTRA_DIR}/theme-template.toml"
)
target_link_libraries(${PROJECT_NAME} vicinae::ipc vicinae::scriptcommand vicinae::common)
install(TARGETS ${PROJECT_NAME})
