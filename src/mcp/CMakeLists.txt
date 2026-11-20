set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(TARGET vicinae-mcp)

include("Figura")

set(FIGURA_DIR "${CMAKE_SOURCE_DIR}/figura")

figura_compile(
	CLIENT
	PROTO "${FIGURA_DIR}/ipc.fig"
	LANG glaze
	NAMESPACE ipc
	OUTPUT mcp-ipc-client.hpp
)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_executable(${TARGET} src/main.cpp ${SRCS})
vicinae_enable_utf8(${TARGET})
target_include_directories(${TARGET} PRIVATE ${GENOUT})
target_compile_features(${TARGET} PUBLIC cxx_std_23)
target_link_libraries(${TARGET} PUBLIC glaze::glaze vicinae::common Qt6::Core Qt6::Network)

if(WIN32)
	# next to vicinae.exe so the deployed Qt DLLs resolve
	install(TARGETS ${TARGET} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
else()
	install(TARGETS ${TARGET} RUNTIME DESTINATION ${VICINAE_LIBEXEC_DIR})
endif()
