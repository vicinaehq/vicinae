set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(TARGET vicinae-browser-link)
set(CHROME_EXTENSION_ID "kcmipingpfbohfjckomimmahknoddnke") # https://chromewebstore.google.com/detail/vicinae-integration/kcmipingpfbohfjckomimmahknoddnke
set(NATIVE_MESSAGING_HOST "com.vicinae.vicinae")
set(NATIVE_HOST_BIN ${VICINAE_LIBEXEC_PATH}/${TARGET})

option(INSTALL_BROWSER_NATIVE_HOST "Install native host manifests for firefox and chromium" ON)

find_package(glaze)

add_executable(${TARGET} src/browser.cpp)
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include PRIVATE)
target_compile_features(${TARGET} PUBLIC cxx_std_23)
target_compile_definitions(${TARGET} PUBLIC NATIVE_MESSAGING_HOST="${NATIVE_MESSAGING_HOST}")
target_link_libraries(${TARGET} PUBLIC vicinae-ipc glaze::glaze)

if (INSTALL_BROWSER_NATIVE_HOST)
	configure_file(native-host/${NATIVE_MESSAGING_HOST}.chromium.json.in ${NATIVE_MESSAGING_HOST}.chromium.json)
	set(NATIVE_HOST_PATH "/etc/chromium/native-messaging-hosts")
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${NATIVE_MESSAGING_HOST}.chromium.json DESTINATION ${NATIVE_HOST_PATH} RENAME ${NATIVE_MESSAGING_HOST}.json)
endif()

if (INSTALL_BROWSER_NATIVE_HOST)
	configure_file(native-host/${NATIVE_MESSAGING_HOST}.firefox.json.in ${NATIVE_MESSAGING_HOST}.firefox.json)
	set(NATIVE_HOST_PATH "/usr/lib/mozilla/native-messaging-hosts")
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${NATIVE_MESSAGING_HOST}.firefox.json DESTINATION ${NATIVE_HOST_PATH} RENAME ${NATIVE_MESSAGING_HOST}.json)
endif()

install(FILES
	native-host/${NATIVE_MESSAGING_HOST}.chromium.json.in
	native-host/${NATIVE_MESSAGING_HOST}.firefox.json.in
	DESTINATION ${CMAKE_INSTALL_DATADIR}/vicinae/native-messaging-hosts
)

install(TARGETS ${TARGET}
	RUNTIME DESTINATION ${VICINAE_LIBEXEC_DIR}
)
