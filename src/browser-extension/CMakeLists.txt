set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(TARGET vicinae-browser-link)
set(CHROME_EXTENSION_ID "kcmipingpfbohfjckomimmahknoddnke") # https://chromewebstore.google.com/detail/vicinae-integration/kcmipingpfbohfjckomimmahknoddnke
set(NATIVE_MESSAGING_HOST "com.vicinae.vicinae")
set(NATIVE_HOST_BIN ${VICINAE_LIBEXEC_PATH}/${TARGET})

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

set(DATA_DIR ${CMAKE_INSTALL_DATADIR}/vicinae)
set(NATIVE_HOST_DATA_DIR ${DATA_DIR}/native-host)
set(CHROMIUM_MANIFEST_TEMPLATE native-host/${NATIVE_MESSAGING_HOST}.chromium.json.in)
set(FIREFOX_MANIFEST_TEMPLATE native-host/${NATIVE_MESSAGING_HOST}.firefox.json.in)

configure_file(${CHROMIUM_MANIFEST_TEMPLATE} ${NATIVE_MESSAGING_HOST}.chromium.json)
configure_file(${FIREFOX_MANIFEST_TEMPLATE} ${NATIVE_MESSAGING_HOST}.firefox.json)

# template files (needed by the script install that needs to configure them depending on the chosen install path)
install(FILES ${CHROMIUM_MANIFEST_TEMPLATE} DESTINATION ${NATIVE_HOST_DATA_DIR} RENAME ${NATIVE_MESSAGING_HOST}.chromium.json.in)
install(FILES ${FIREFOX_MANIFEST_TEMPLATE} DESTINATION ${NATIVE_HOST_DATA_DIR} RENAME ${NATIVE_MESSAGING_HOST}.firefox.json.in)

# we install the configured files in share/ because chromium doesn't support installing native host manifests under /usr (!) and mozilla doesn't seem
# to support any other prefix than "/usr/lib/mozilla/native-messaging-hosts/".
# for these reasons, it is up to the packager to decide how to deal with installing this, and we provide both the template and configured version
# so that it can be simply processed or copied at the appropriate location.
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${NATIVE_MESSAGING_HOST}.chromium.json DESTINATION ${NATIVE_HOST_DATA_DIR}/chromium RENAME ${NATIVE_MESSAGING_HOST}.json)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${NATIVE_MESSAGING_HOST}.firefox.json DESTINATION ${NATIVE_HOST_DATA_DIR}/firefox RENAME ${NATIVE_MESSAGING_HOST}.json)

install(TARGETS ${TARGET}
	RUNTIME DESTINATION ${VICINAE_LIBEXEC_DIR}
)
