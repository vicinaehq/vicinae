set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(LIBS)

set(TARGET spellcastd)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Network)
find_package(SQLite3 REQUIRED)


option(WAYLAND_LAYER_SHELL "Enable support for the Wayland layer shell
protocol, required for proper window positioning on most wlroots-based
compositors." OFF)

list(APPEND LIBS tomlplusplus Qt6::Widgets Qt6::Sql Qt6::Network jsoncpp calc)

if (WAYLAND_LAYER_SHELL)
	find_package(Qt6 REQUIRED COMPONENTS WaylandClient)
	find_package(LayerShellQt REQUIRED)
	add_compile_definitions(-DWAYLAND_LAYER_SHELL)
	list(APPEND LIBS LayerShellQt::Interface)
endif()

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/lib
)

qt_add_executable(${TARGET}
	src/main.cpp
	src/app.cpp
	src/config.cpp
	src/commands/index-command.cpp
	src/commands/index-command.cpp
	src/exchange-rate-service.hpp
	src/omnicast.hpp
	src/index-command.hpp
	src/lib/tinyexpr.cpp
	./resources.qrc

	#src/render.cpp
	#src/extension_manager.cpp

	src/ui/status_bar.hpp
	src/ui/status_bar.cpp
	src/ui/input_completer.hpp
	src/ui/input_completer.cpp
	src/ui/inline_qline_edit.hpp
	src/ui/inline_qline_edit.cpp
	src/ui/top_bar.hpp
	src/ui/top_bar.cpp
	src/ui/color_circle.hpp
	src/ui/color_circle.cpp
	src/ui/command_widget.hpp
	src/ui/command_widget.cpp
	src/ui/managed_list.hpp
	src/ui/managed_list.cpp
	
)

target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${TARGET} PRIVATE ${LIBS})

install(TARGETS ${TARGET})
