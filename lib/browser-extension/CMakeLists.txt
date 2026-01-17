set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(TARGET browser)
set(CHROME_EXTENSION_ID "com.vicinae.vicinae")
set(NATIVE_MESSAGING_HOST "com.vicinae.vicinae")
set(NATIVE_HOST_BIN ${CMAKE_INSTALL_PREFIX}/bin/vicinae)

option(INSTALL_FIREFOX_HOST "Install manifest for firefox native host" ON)
option(INSTALL_CHROME_HOST "Install manifest for chromium native host" ON)

find_package(glaze)

add_library(${TARGET} STATIC src/browser.cpp)
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include PRIVATE)
target_compile_features(${TARGET} PUBLIC cxx_std_23)
target_compile_definitions(${TARGET} PUBLIC NATIVE_MESSAGING_HOST="${NATIVE_MESSAGING_HOST}")
target_link_libraries(${TARGET} PUBLIC vicinae-ipc)

if (INSTALL_CHROME_HOST)
	configure_file(native-host/${NATIVE_MESSAGING_HOST}.chromium.json.in ${NATIVE_MESSAGING_HOST}.chromium.json)
	set(NATIVE_HOST_PATH "/etc/chromium/native-messaging-hosts")
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${NATIVE_MESSAGING_HOST}.chromium.json DESTINATION ${NATIVE_HOST_PATH} RENAME ${NATIVE_MESSAGING_HOST}.json)
endif()

if (INSTALL_FIREFOX_HOST)
	configure_file(native-host/${NATIVE_MESSAGING_HOST}.firefox.json.in ${NATIVE_MESSAGING_HOST}.firefox.json)
	set(NATIVE_HOST_PATH "/usr/lib/mozilla/native-messaging-hosts")
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${NATIVE_MESSAGING_HOST}.firefox.json DESTINATION ${NATIVE_HOST_PATH} RENAME ${NATIVE_MESSAGING_HOST}.json)
endif()

install(TARGETS ${TARGET})
