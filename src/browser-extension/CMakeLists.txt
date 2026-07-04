set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(TARGET vicinae-browser-link)
# NATIVE_MESSAGING_HOST and CHROME_EXTENSION_ID are set in the top-level CMakeLists.txt

include("Figura")

set(FIGURA_DIR "${CMAKE_SOURCE_DIR}/figura")

figura_compile(
	CLIENT
	PROTO "${FIGURA_DIR}/ipc.fig"
	LANG glaze
	NAMESPACE ipc
	OUTPUT browser-ipc-client.hpp
)

add_executable(${TARGET} src/browser.cpp ${SRCS})
target_include_directories(${TARGET} PRIVATE ${GENOUT})
target_compile_features(${TARGET} PUBLIC cxx_std_23)
target_compile_definitions(${TARGET} PUBLIC NATIVE_MESSAGING_HOST="${NATIVE_MESSAGING_HOST}")
target_link_libraries(${TARGET} PUBLIC glaze::glaze)

install(TARGETS ${TARGET}
	RUNTIME DESTINATION ${VICINAE_LIBEXEC_DIR}
)
