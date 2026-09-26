cmake_minimum_required(VERSION 3.24)
project(VicinaeScrolling LANGUAGES CXX)

find_package(Qt6 6.9 REQUIRED COMPONENTS Quick GLOBAL)
set(CMAKE_AUTOMOC ON)

qt_add_qml_module(vicinae-wheelhandler
	STATIC
	PLUGIN_TARGET vicinae-wheelhandler
	URI Vicinae.Scrolling
	VERSION 1.0
	OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Vicinae/Scrolling"
	RESOURCE_PREFIX /qt/qml
	SOURCES wheelhandler.cpp wheelhandler.h
	DEPENDENCIES QtQuick
)
add_library(vicinae::wheelhandler ALIAS vicinae-wheelhandler)
target_compile_features(vicinae-wheelhandler PUBLIC cxx_std_23)
target_link_libraries(vicinae-wheelhandler PUBLIC Qt6::Quick)
