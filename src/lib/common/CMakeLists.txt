cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME common)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)



add_library(${PROJECT_NAME} STATIC src/common.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include PRIVATE src)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
