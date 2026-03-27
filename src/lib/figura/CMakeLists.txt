cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME figura)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} 
	src/main.cpp
)
target_link_libraries(${PROJECT_NAME} PRIVATE vicinae::common)
