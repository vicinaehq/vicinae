cmake_minimum_required(VERSION 3.16)
set(OMNICAST_VERSION v1.0.0)

set(PROJECT_NAME omnicast)
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES CXX)

add_compile_definitions(OMNICAST_VERSION="${OMNICAST_VERSION}")

add_compile_options(-march=native -g3)
#add_compile_options(-march=x86-64)

add_subdirectory(omnictl)
add_subdirectory(omnicast)
add_subdirectory(proto)
