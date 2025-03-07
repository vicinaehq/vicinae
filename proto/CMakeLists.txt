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
)

add_library(${TARGET} ${SRCS})
target_compile_options(${TARGET} PUBLIC -g3)

add_executable(prototest main.cpp)
target_link_libraries(prototest PRIVATE proto)
