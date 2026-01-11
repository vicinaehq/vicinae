set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CHROME_EXTENSION_ID "com.vicinaehq.vicinae")
set(NATIVE_MESSAGING_HOST "com.vicinaehq.vicinae")
set(CHROMIUM_NATIVE_HOST_PATH "/etc/chromium/native-messaging-hosts")
find_package(glaze)

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)
find_package(Protobuf)
unset(CMAKE_FIND_PACKAGE_PREFER_CONFIG)

set(TARGET vicinae-chromium-link)

set(CHROME_EXTENSION_LINK_BIN ${CMAKE_INSTALL_PREFIX}/bin/${TARGET})
configure_file(native-host/${NATIVE_MESSAGING_HOST}.json.in ${NATIVE_MESSAGING_HOST}.json)

add_executable(${TARGET} src/main.cpp)

target_compile_features(${TARGET} PUBLIC cxx_std_23)
target_link_libraries(${TARGET} PRIVATE protobuf::libprotobuf)
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

set(PROTO_FILES ${CMAKE_SOURCE_DIR}/proto/daemon.proto)
set(PROTO_IMPORT_DIRS ${PROTO_SRC_DIR} ${COMMON_PROTO_DIR} ${CMAKE_SOURCE_DIR}/proto)

message(STATUS "proto import dir ${PROTO_IMPORT_DIRS} ${PROTO_FILES} ")

if (NOT USE_SYSTEM_PROTOBUF)
	list(APPEND PROTO_IMPORT_DIRS ${protobuf_SOURCE_DIR}/src)
endif()

protobuf_generate(
	TARGET ${TARGET}
	PROTOS ${PROTO_FILES}
	IMPORT_DIRS ${PROTO_IMPORT_DIRS}
	PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${NATIVE_MESSAGING_HOST}.json DESTINATION ${CHROMIUM_NATIVE_HOST_PATH})
install(TARGETS ${TARGET})
