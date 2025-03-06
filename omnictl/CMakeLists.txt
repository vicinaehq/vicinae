set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LIBS)

set(TARGET omnictl)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/lib
	${CMAKE_CURRENT_SOURCE_DIR}/../proto
)

set(SRCS
	main.cpp
)

add_executable(${TARGET} main.cpp)
target_compile_options(${TARGET} PUBLIC -g3)
target_link_libraries(${TARGET} PRIVATE proto)

install(TARGETS ${TARGET})
