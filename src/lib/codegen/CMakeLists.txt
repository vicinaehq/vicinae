cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME codegen)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(codegen 
	src/main.cpp
)
