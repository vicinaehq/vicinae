cmake_minimum_required(VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES C CXX)

set(CHROME_EXTENSION_ID "com.vicinaehq.vicinae")
set(CHROMIUM_NATIVE_HOST_PATH "/etc/chromium/native-messaging-hosts")
set(CHROME_EXTENSION_LINK_BIN ${CMAKE_INSTALL_PREFIX}/bin/vicinae-chromium-link)
find_package(glaze)
configure_file(native-host/com.vicinaehq.ipc_host.json.in com.vicinaehq.ipc_host.json)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/com.vicinaehq.ipc_host.json DESTINATION ${CHROMIUM_NATIVE_HOST_PATH})
