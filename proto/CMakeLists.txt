set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LIBS)

set(TARGET proto)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/lib
)

set(SRCS
	proto.cpp
	marshal.cpp
)

add_executable(${TARGET} ${SRCS})

target_compile_options(${TARGET} PUBLIC -Wall -g3)

install(TARGETS ${TARGET})
