set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LIBS)

set(TARGET omni)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/lib
	${CMAKE_CURRENT_SOURCE_DIR}/../proto
)

set(SRCS
	main.cpp
	command.hpp
	command.cpp
)

add_executable(${TARGET} ${SRCS})
target_compile_options(${TARGET} PUBLIC -g3)
target_link_libraries(${TARGET} PRIVATE proto)

install(TARGETS ${TARGET})
