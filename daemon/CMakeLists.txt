set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(LIBS)

set(TARGET spellcastd)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Network Pdf)
find_package(SQLite3 REQUIRED)
find_package(cmark REQUIRED)

option(WAYLAND_LAYER_SHELL "Enable support for the Wayland layer shell
protocol, required for proper window positioning on most wlroots-based
compositors." OFF)

list(APPEND LIBS Qt6::Widgets Qt6::Sql Qt6::Network Qt6::Pdf cmark calc)

if (WAYLAND_LAYER_SHELL)
	find_package(Qt6 REQUIRED COMPONENTS WaylandClient)
	find_package(LayerShellQt REQUIRED)
	add_compile_definitions(-DWAYLAND_LAYER_SHELL)
	list(APPEND LIBS LayerShellQt::Interface)
endif()

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/lib
)

qt_add_executable(${TARGET}
	src/main.cpp
	src/app.cpp
	src/app.hpp

	#src/commands/index/index-command.hpp
	#src/commands/index/index-command.cpp
	#src/commands/calculator-history/calculator-history.hpp
	#src/commands/calculator-history/calculator-history.cpp
	#src/commands/create-quicklink/create-quicklink.hpp

	include/extend/extension-list-controller.hpp
	include/extend/extension-view.hpp
	include/extend/extension-command.hpp
	include/extend/extension-detail-view.hpp

	include/ui/virtual-list.hpp
	
	include/ui/list-view.hpp
	include/ui/empty-view.hpp

	include/ui/native-list.hpp
	include/ui/test-list.hpp

	src/manage-quicklinks-command.hpp

	include/ui/metadata-pane.hpp
	src/ui/metadata-pane.cpp

	include/ui/horizontal-metadata.hpp
	src/ui/horizontal-metadata.cpp

	include/ui/vertical-metadata.hpp
	src/ui/vertical-metadata.cpp

	include/ui/text-label.hpp
	src/ui/text-label.cpp

	src/extend/action-model.cpp
	src/extend/color-model.cpp
	src/extend/detail-model.cpp
	src/extend/image-model.cpp
	src/extend/list-model.cpp
	src/extend/metadata-model.cpp
	src/extend/model-parser.cpp
	src/extend/tag-list.cpp
	src/extend/root-detail-model.cpp
	src/extend/empty-view-model.cpp

	include/image-fetcher.hpp

	src/json-patch.cpp

	include/markdown-renderer.hpp
	src/ui/markdown-renderer.cpp

	include/remote-image-viewer.hpp
	src/ui/remote-image-viewer.cpp

	src/root-command.hpp
	src/command.hpp
	src/extension.hpp
	src/view.hpp


	src/xdg/xdg-desktop.cpp

	#src/commands/create-quicklink/create-quicklink.cpp

	#src/command-database.cpp

	#src/exchange-rate-service.hpp
	src/omnicast.hpp

	./resources.qrc

	#src/render.cpp
	#src/extension_manager.cpp

	src/filesystem-database.hpp
	src/files-command.hpp
	src/indexer-service.hpp


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
	src/ui/toast.hpp
	src/ui/toast.cpp
	src/ui/action_popover.hpp
	src/ui/action_popover.cpp
	src/ui/turbobox.hpp
	src/extension_manager.hpp
)

target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${TARGET} PRIVATE ${LIBS})

install(TARGETS ${TARGET})
