cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME vicinae) # the cli is the "vicinae" binary, a single entrypoint for everything
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include("Figura")

set(FIGURA_DIR "${CMAKE_SOURCE_DIR}/figura")

figura_compile(
	CLIENT
	PROTO "${FIGURA_DIR}/ipc.fig"
	LANG glaze
	NAMESPACE ipc
	OUTPUT ipc-client.hpp
)

set(CLI_SOURCES src/main.cpp src/cli.cpp src/theme.cpp src/fs.cpp src/local-socket.cpp ${SRCS})
if (NOT WIN32)
	list(APPEND CLI_SOURCES src/server.cpp)
endif()

add_executable(${PROJECT_NAME} ${CLI_SOURCES})
vicinae_enable_utf8(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE src ${GENOUT})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
include(EmbedFile)
embed_file(${PROJECT_NAME} "${EXTRA_DIR}/config.jsonc" DEFAULT_CONFIG)
embed_file(${PROJECT_NAME} "${EXTRA_DIR}/theme-template.toml" THEME_TEMPLATE)
target_link_libraries(${PROJECT_NAME} glaze::glaze vicinae::scriptcommand vicinae::common)
install(TARGETS ${PROJECT_NAME})

if (WIN32)
	# Windowed forwarder registered as the shell handler for app URL schemes.
	# We cannot use the CLI directly as it is registered to use the console subsystem, which
	# would spawn a console every time it is invoked.
	add_executable(vicinae-url-handler WIN32 src/url-handler-main.cpp src/local-socket.cpp ${SRCS})
	vicinae_enable_utf8(vicinae-url-handler)
	target_include_directories(vicinae-url-handler PRIVATE src ${GENOUT})
	target_compile_features(vicinae-url-handler PUBLIC cxx_std_23)
	target_link_libraries(vicinae-url-handler glaze::glaze vicinae::common)
	# keep a standard main() entrypoint in the windowed subsystem
	target_link_options(vicinae-url-handler PRIVATE /ENTRY:mainCRTStartup)
	install(TARGETS vicinae-url-handler)
endif()
