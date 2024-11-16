cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME omnicast)
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES CXX)


add_compile_options(-march=native -g3)
#add_compile_options(-march=x86-64)

add_subdirectory(daemon)

