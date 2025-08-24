include("Wayland")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(TARGET ${WLR_CLIP_BIN})
set(PROTO_SRC_DIR ${CMAKE_SOURCE_DIR}/proto/extensions)
set(PROTO_FILES "${PROTO_SRC_DIR}/wlr-clipboard.proto")

# https://cmake.org/cmake/help/latest/module/FindProtobuf.html#example-finding-protobuf-in-config-mode
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)
find_package(Protobuf)
unset(CMAKE_FIND_PACKAGE_PREFER_CONFIG)


include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/lib
	${Protobuf_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}  # For generated headers
)

set(SRCS
	src/main.cpp
	src/data-control-client.cpp
	src/display.cpp
	src/registry.cpp
	src/seat.cpp
	src/utils.cpp
)

add_executable(${TARGET} ${SRCS})
wayland_generate_protocol(${TARGET} ${CMAKE_CURRENT_SOURCE_DIR}/protocols/wlr-data-control-unstable-v1.xml)
target_compile_options(${TARGET} PUBLIC -g3 -fsanitize=address)
target_link_libraries(${TARGET} PRIVATE wayland-client protobuf::libprotobuf -fsanitize=address)

make_directory(${CMAKE_CURRENT_BINARY_DIR}/proto)

protobuf_generate(
	TARGET ${TARGET}
	PROTOS ${PROTO_FILES}
	IMPORT_DIRS ${PROTO_SRC_DIR} ${COMMON_PROTO_DIR}
	PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto
)

install(TARGETS ${TARGET})
