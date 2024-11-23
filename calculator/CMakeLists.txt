cmake_minimum_required(VERSION 3.30)
project("calculator")
set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(TARGET calculator)

add_compile_options(-march=native -g3)


set(SRCS 
	src/main.cpp
	src/tokenizer.cpp
	src/ast.cpp
	src/parser.cpp
	src/unit.cpp
)

include_directories(
	src
)

add_executable(calculator ${SRCS})
target_compile_options(calculator PUBLIC -Wall)
install(TARGETS ${TARGET})
