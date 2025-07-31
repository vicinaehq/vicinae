set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(TARGET omni-wlr-clip)
set(PROTO_SRC_DIR ${CMAKE_SOURCE_DIR}/proto/extensions)
set(PROTO_FILES "${PROTO_SRC_DIR}/wlr-clipboard.proto")

find_package(protobuf CONFIG REQUIRED) # may not work on older protobuf versions (ubuntu 25 doesn't from my testing)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/lib
	${Protobuf_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}  # For generated headers

)

set(SRCS
	src/main.cpp
	src/wayland-wlr-data-control-client-protocol.c
	src/data-control-client.cpp
	src/display.cpp
	src/registry.cpp
	src/seat.cpp
	src/utils.cpp
)

add_executable(${TARGET} ${SRCS})
target_compile_options(${TARGET} PUBLIC -g3)
target_link_libraries(${TARGET} PRIVATE wayland-client protobuf::libprotobuf)

make_directory(${CMAKE_CURRENT_BINARY_DIR}/proto)

protobuf_generate(
	TARGET ${TARGET}
	PROTOS ${PROTO_FILES}
	IMPORT_DIRS ${PROTO_SRC_DIR} ${COMMON_PROTO_DIR}
	PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto
)

install(TARGETS ${TARGET})
