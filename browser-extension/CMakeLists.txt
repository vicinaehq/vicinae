set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CHROME_EXTENSION_ID "com.vicinaehq.vicinae")
set(NATIVE_MESSAGING_HOST "com.vicinaehq.vicinae")
set(CHROMIUM_NATIVE_HOST_PATH "/etc/chromium/native-messaging-hosts")
find_package(glaze)

set(TARGET vicinae-chromium-link)

set(CHROME_EXTENSION_LINK_BIN ${CMAKE_INSTALL_PREFIX}/bin/${TARGET})
configure_file(native-host/${NATIVE_MESSAGING_HOST}.json.in ${NATIVE_MESSAGING_HOST}.json)

add_executable(${TARGET} src/main.cpp)

target_link_libraries(${TARGET} PUBLIC vicinae-ipc)
target_compile_features(${TARGET} PUBLIC cxx_std_23)
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${NATIVE_MESSAGING_HOST}.json DESTINATION ${CHROMIUM_NATIVE_HOST_PATH})
install(TARGETS ${TARGET})
