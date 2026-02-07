cmake_minimum_required(VERSION 3.16)
project(data-control-server)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DATA_CONTROL_SERVER_BIN vicinae-data-control-server)

find_package(Protobuf REQUIRED)

include("${CMAKE_SOURCE_DIR}/cmake/Wayland.cmake")
wayland_generate_protocol("wlr-data-control-unstable-v1")
wayland_generate_protocol("ext-data-control-v1")

add_executable(${DATA_CONTROL_SERVER_BIN}
  src/main.cpp
  src/selection.cpp
  src/wayland/display.cpp
  src/wayland/registry.cpp
  src/wayland/seat.cpp
  src/wlr/clipman.cpp
  src/wlr/data-control-client.cpp
  src/wlr/data-device.cpp
  src/wlr/data-offer.cpp
  src/ext/clipman.cpp
  src/ext/data-control-client.cpp
  src/ext/data-device.cpp
  src/ext/data-offer.cpp
  ${SRCS}
)

target_include_directories(${DATA_CONTROL_SERVER_BIN} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(${DATA_CONTROL_SERVER_BIN} PRIVATE
  protobuf::libprotobuf
  wayland-client
)

protobuf_generate(
  TARGET ${DATA_CONTROL_SERVER_BIN}
  PROTOS ${CMAKE_SOURCE_DIR}/src/proto/wlr-clipboard.proto
  IMPORT_DIRS ${CMAKE_SOURCE_DIR}/src/proto
  PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto
)

target_include_directories(${DATA_CONTROL_SERVER_BIN} PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}/proto
)

install(TARGETS ${DATA_CONTROL_SERVER_BIN}
  RUNTIME DESTINATION ${VICINAE_LIBEXEC_DIR}
)
