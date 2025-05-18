set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(LIBS)

set(TARGET omnicast)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Network Svg)
find_package(cmark REQUIRED)

option(WAYLAND_LAYER_SHELL "Enable support for the Wayland layer shell
protocol, required for proper window positioning on most wlroots-based
compositors." ON)


list(APPEND LIBS Qt6::Widgets Qt6::Sql Qt6::Network Qt6::Svg cmark proto qalculate)


if (UNIX AND NOT APPLE)
	message(STATUS "Compiling with wayland layer shell support (for wlroots-like compositors)")
	find_package(Qt6 REQUIRED COMPONENTS WaylandClient)
	find_package(LayerShellQt REQUIRED)
	add_compile_definitions(-DWAYLAND_LAYER_SHELL)
	list(APPEND LIBS LayerShellQt::Interface)
endif()

add_compile_definitions(QT_MESSAGELOGCONTEXT)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/lib
	${CMAKE_CURRENT_SOURCE_DIR}/../proto
	${CMAKE_CURRENT_SOURCE_DIR}/..
)

set(SRCS
	src/main.cpp
	src/app.cpp
	src/app.hpp

	include/theme.hpp
	src/theme.cpp

	include/font-service.hpp
	src/font-service.cpp

	include/config-service.hpp

	include/root-item-manager.hpp

	include/ui/image/omnimg.hpp

	include/favicon/favicon-service.hpp
	src/favicon/favicon-service.cpp
	include/favicon/favicon-request.hpp
	src/favicon/favicon-request.cpp
	include/favicon/google-favicon-request.hpp
	src/favicon/google-favicon-request.cpp
	include/favicon/dummy-favicon-request.hpp
	src/favicon/dummy-favicon-request.cpp
	include/favicon/cached-favicon-request.hpp
	src/favicon/cached-favicon-request.cpp

	include/ui/form/base-input.hpp
	src/ui/form/base-input.cpp
	include/ui/form/selector-input.hpp
	src/ui/form/selector-input.cpp
	include/ui/form/completed-input.hpp
	src/ui/form/completed-input.cpp
	include/ui/form/form.hpp
	src/ui/form/form.cpp
	include/ui/form/form-field.hpp
	src/ui/form/form-field.cpp
	include/ui/form/checkbox.hpp
	src/ui/form/checkbox.cpp
	include/ui/form/checkbox-input.hpp
	include/ui/form/app-picker-input.hpp
	src/ui/form/app-picker-input.cpp

	include/ui/popover.hpp
	src/ui/popover.cpp

	include/ui/typography.hpp

	include/ui/omni-button.hpp
	include/ui/dialog.hpp
	include/ui/alert.hpp

	include/omni-command-db.hpp

	include/ui/split-detail.hpp
	src/ui/split-detail.cpp

	include/ui/detail-widget.hpp
	src/ui/detail-widget.cpp

	include/clipboard/clipboard-server.hpp
	include/clipboard/wlr-clipboard-server.hpp
	src/clipboard/wlr-clipboard-server.cpp
	include/clipboard/dummy-clipboard-server.hpp
	src/clipboard/dummy-clipboard-server.cpp
	include/clipboard/clipboard-server-factory.hpp
	src/clipboard/clipboard-server-factory.cpp
	include/clipboard/clipboard-service.hpp
	src/clipboard/clipboard-service.cpp

	# begin app-database
	include/app/app-database.hpp
	include/app/xdg-app-database.hpp

	src/app/xdg-app-database.cpp

	# end app-database

	# begin action-pannel-widget
	include/ui/action-pannel/action.hpp
	src/ui/action-pannel/action.cpp

	include/ui/action-pannel/action-section.hpp
	src/ui/action-pannel/action-section.cpp

	include/ui/action-pannel/action-pannel-view.hpp

	include/ui/action-pannel/action-pannel-widget.hpp
	src/ui/action-pannel/action-pannel-widget.cpp

	include/ui/action-pannel/action-pannel-list-view.hpp
	src/ui/action-pannel/action-pannel-list-view.cpp

	include/ui/action-pannel/action-list-widget.hpp
	src/ui/action-pannel/action-list-widget.cpp

	include/ui/action-pannel/action-list-item.hpp
	src/ui/action-pannel/action-list-item.cpp

	include/ui/action-pannel/open-with-action.hpp
	src/ui/action-pannel/open-with-action.cpp

	# end action-pannel-widget

	include/ui/horizontal-loading-bar.hpp
	src/ui/horizontal-loading-bar.cpp

	# start markdown renderer
	include/ui/markdown/markdown-renderer.hpp
	src/ui/markdown/markdown-renderer.cpp
	# end markdown renderer

	include/lib/emoji-detect.hpp
	src/lib/emoji-detect.cpp

	# start AI
	include/ai/ai-service.hpp
	include//ai/ai-provider.hpp
	include/ai/ollama-ai-provider.hpp
	# end AI

	include/clipboard-history-command.hpp

	include/ui/omni-painter.hpp
	src/ui/omni-painter.cpp

	include/wm/window-manager.hpp
	
	include/extension/extension-command.hpp
	include/extension/extension-view.hpp
	include/extension/extension-component.hpp

	include/extension/extension-form-component.hpp

	include/extension/extension-list-component.hpp
	src/extension/extension-list-component.cpp

	include/extension/extension-grid-component.hpp
	src/extension/extension-grid-component.cpp

	include/extension/extension.hpp
	src/extension/extension.cpp

	include/root-item-manager.hpp

	include/extension/extension-command.hpp
	src/extension/extension-command.cpp

	include/extension/form/extension-form-input.hpp
	include/extension/form/extension-text-field.hpp
	include/extension/form/extension-password-field.hpp
	include/extension/form/extension-checkbox-field.hpp
	include/extension/form/extension-dropdown.hpp

	include/bookmark-service.hpp
	include/root-bookmark-provider.hpp


	include/extend/extension-detail-view.hpp

	include/local-storage-service.hpp
	src/local-storage.cpp

	include/ui/argument-completer-widget.hpp

	include/ui/button.hpp
	src/ui/button.cpp


	include/ui/icon-button.hpp
	src/ui/icon-button.cpp

	include/command-database.hpp
	src/command-database.cpp

	# omni list 
	include/ui/omni-list.hpp
	src/ui/omni-list.cpp

	include/ui/omni-list-item-widget-wrapper.hpp
	src/ui/omni-list-item-widget-wrapper.cpp

	include/ui/selectable-omni-list-widget.hpp
	src/ui/selectable-omni-list-widget.cpp

	include/ui/list-section-header.hpp
	src/ui/list-section-header.cpp

	include/ui/default-list-item-widget.hpp
	src/ui/default-list-item-widget.cpp
	include/ui/list-accessory-widget.hpp
	src/ui/list-accessory-widget.cpp

	include/ui/text-widget.hpp
	src/ui/text-widget.cpp

	include/ui/grid-item-content-widget.hpp
	src/ui/grid-item-content-widget.cpp

	include/ui/tooltip.hpp
	src/ui/tooltip.cpp

	include/ui/grid-item-widget.hpp
	src/ui/grid-item-widget.cpp

	include/ui/omni-list-item-widget.hpp

	include/ollama-service.hpp

	include/omni-icon.hpp

	include/ui/omni-scroll-bar.hpp
	src/ui/omni-scroll-bar.cpp

	src/emoji-list.cpp

	src/calculator-history-command.hpp

	src/ui/ellided-label.cpp
	include/ui/ellided-label.hpp

	src/tinyexpr.cpp
	include/tinyexpr.hpp

	include/create-quicklink-command.hpp

	include/builtin_icon.hpp
	src/builtin_icon.cpp

	include/ui/transform-result.hpp
	src/ui/transform-result.cpp

	include/ui/color-transform-widget.hpp
	src/ui/color-transform-widget.cpp

	include/ui/emoji-viewer.hpp
	src/ui/emoji-viewer.cpp

	include/ui/empty-view.hpp

	include/ui/keyboard-shortcut-indicator.hpp
	src/ui/keyboard-shortcut-indicator.cpp

	include/ui/shortcut-button.hpp
	src/ui/shortcut-button.cpp

	include/emoji-command.hpp

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
	src/extend/grid-model.cpp
	src/extend/list-model.cpp
	src/extend/metadata-model.cpp
	src/extend/model-parser.cpp
	src/extend/tag-list.cpp
	src/extend/root-detail-model.cpp
	src/extend/empty-view-model.cpp
	include/extend/form-model.hpp
	src/extend/form-model.cpp

	include/ui/focus-notifier.hpp


	include/image-fetcher.hpp

	src/json-patch.cpp

	src/root-command.hpp
	src/command.hpp
	src/view.hpp
	

	src/xdg/xdg-desktop.cpp

	#src/commands/create-quicklink/create-quicklink.cpp

	src/omnicast.hpp
	src/omnicast.cpp

	./resources.qrc
	./icons/icons.qrc

	src/filesystem-database.hpp
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
	#src/ui/action_popover.hpp
	#src/ui/action_popover.cpp
	src/extension_manager.hpp
)


if (UNIX AND NOT APPLE)
	list(APPEND SRCS
		src/lib/xkbcommon-utils.cpp

		# Hyprland
		include/wm/hyprland/hyprland.hpp
		include/wm/hyprland/hyprctl.hpp
		# End Hyprland
	)
endif()
	

qt_add_executable(${TARGET} ${SRCS})

target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${TARGET} PRIVATE ${LIBS})

install(TARGETS ${TARGET})
