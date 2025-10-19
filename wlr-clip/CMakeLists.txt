include("Wayland")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(TARGET ${WLR_CLIP_BIN})
set(PROTO_SRC_DIR ${CMAKE_SOURCE_DIR}/proto)
#set(PROTO_FILES "${PROTO_SRC_DIR}/manager.proto ${PROTO_SRC_DIR}/wlr-clipboard.proto")
file(GLOB PROTO_FILES "${PROTO_SRC_DIR}/*.proto")


include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/lib
	${Protobuf_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}  # For generated headers
)

set(SRCS
	src/main.cpp
	src/app.cpp
	src/data-control-client.cpp
	src/display.cpp
	src/registry.cpp
	src/seat.cpp
	src/data-device.cpp
	src/data-offer.cpp
)
wayland_generate_protocol("${CMAKE_CURRENT_SOURCE_DIR}/protocols/wlr-data-control-unstable-v1.xml")

message(STATUS "common proto dir ${COMMON_PROTO_DIR}")

add_executable(${TARGET} ${SRCS})
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
#target_compile_options(${TARGET} PUBLIC)
target_link_libraries(${TARGET} PRIVATE wayland-client protobuf::libprotobuf) 

make_directory(${CMAKE_CURRENT_BINARY_DIR}/proto/extensions)

protobuf_generate(
	TARGET ${TARGET}
	PROTOS ${PROTO_FILES}
	IMPORT_DIRS ${PROTO_SRC_DIR} ${COMMON_PROTO_DIR}
	PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto
)

install(TARGETS ${TARGET})
