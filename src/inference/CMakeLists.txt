cmake_minimum_required(VERSION 3.16)
project(inference)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(INFERENCE_BIN vicinae-inference)

find_package(glaze)

include("Figura")
include("EmbedBinary")

set(FIGURA_DIR "${CMAKE_SOURCE_DIR}/figura")

figura_compile(
	SERVER
	PROTO "${FIGURA_DIR}/inference.fig"
	LANG glaze
	NAMESPACE inference_gen
	OUTPUT inference-server.hpp
)

add_executable(${INFERENCE_BIN}
	src/main.cpp
	src/inference-service.cpp
	src/vad.cpp
	${SRCS}
)

embed_binary(${INFERENCE_BIN} "${CMAKE_CURRENT_SOURCE_DIR}/assets/ggml-silero-v6.2.0.bin" SILERO_MODEL)

target_include_directories(${INFERENCE_BIN} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${GENOUT}
)

target_link_libraries(${INFERENCE_BIN} PRIVATE
	glaze::glaze
	whisper
	parakeet
)

install(TARGETS ${INFERENCE_BIN}
	RUNTIME DESTINATION ${VICINAE_LIBEXEC_DIR}
)
