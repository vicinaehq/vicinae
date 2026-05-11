cmake_minimum_required(VERSION 3.16)
project(data-control-server)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DATA_CONTROL_SERVER_BIN vicinae-data-control-server)

include("${CMAKE_SOURCE_DIR}/cmake/Wayland.cmake")
wayland_generate_protocol("ext-data-control-v1")

add_executable(${DATA_CONTROL_SERVER_BIN}
  src/main.cpp
  src/selection.cpp
  src/stdin-reader.cpp
  src/clipboard-writer.cpp
  src/wayland/display.cpp
  src/wayland/registry.cpp
  src/wayland/seat.cpp
  src/ext/clipman.cpp
  src/ext/data-control-client.cpp
  src/ext/data-device.cpp
  src/ext/data-offer.cpp
  ${SRCS}
)

target_include_directories(${DATA_CONTROL_SERVER_BIN} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src/common
)

target_link_libraries(${DATA_CONTROL_SERVER_BIN} PRIVATE
  glaze::glaze
  wayland-client
)

install(TARGETS ${DATA_CONTROL_SERVER_BIN}
  RUNTIME DESTINATION ${VICINAE_LIBEXEC_DIR}
)
