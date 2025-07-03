set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(LIBS)

set(TARGET omnicast)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Network Svg DBus)
find_package(cmark REQUIRED)

option(WAYLAND_LAYER_SHELL "Enable support for the Wayland layer shell
protocol, required for proper window positioning on most wlroots-based
compositors." ON)


list(APPEND LIBS Qt6::Widgets Qt6::Sql Qt6::Network Qt6::Svg Qt6::DBus cmark proto qalculate)


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

	src/command-actions.cpp

	src/root-search/extensions/extension-root-provider.hpp
	src/root-search/extensions/extension-root-provider.cpp
	src/root-search/bookmarks/bookmark-root-provider.hpp
	src/root-search/bookmarks/bookmark-root-provider.cpp
	src/root-search/apps/app-root-provider.hpp
	src/root-search/apps/app-root-provider.cpp
	
	src/extension/manager/extension-manager.hpp
	src/extension/manager/extension-manager.cpp

	# Bookmark - Start
	src/services/bookmark/bookmark-service.hpp
	src/services/bookmark/bookmark-service.cpp
	src/services/bookmark/bookmark.hpp
	src/services/bookmark/bookmark.cpp

	src/actions/theme/theme-actions.hpp
	src/actions/theme/theme-actions.cpp

	src/actions/root-search/root-search-actions.hpp
	src/actions/root-search/root-search-actions.cpp

	src/actions/app/app-actions.hpp
	src/actions/app/app-actions.cpp


	src/color-formatter.hpp
	src/color-formatter.cpp

	include/ui/popover.hpp
	src/ui/popover.cpp

	src/action-panel/action-panel.hpp

	include/ui/omni-button.hpp
	include/ui/dialog.hpp
	include/ui/alert.hpp

	include/omni-command-db.hpp

	include/ui/split-detail.hpp
	src/ui/split-detail.cpp

	include/ui/detail-widget.hpp
	src/ui/detail-widget.cpp

	src/services/clipboard/clipboard-server.hpp
	src/services/clipboard/wlr-clipboard-server.hpp
	src/services/clipboard/wlr-clipboard-server.cpp
	src/services/clipboard/dummy-clipboard-server.hpp
	src/services/clipboard/dummy-clipboard-server.cpp
	src/services/clipboard/clipboard-server-factory.hpp
	src/services/clipboard/clipboard-server-factory.cpp
	src/services/clipboard/clipboard-service.hpp
	src/services/clipboard/clipboard-service.cpp

	src/services/local-storage/local-storage-service.hpp
	src/services/local-storage/local-storage.cpp

	src/services/config/config-service.hpp

	src/services/toast/toast-service.hpp

	# begin app-database
	include/app/app-database.hpp
	include/app/xdg-app-database.hpp

	src/app/xdg-app-database.cpp

	# end app-database

	# begin action-pannel-widget
	include/ui/action-pannel/action.hpp
	src/ui/action-pannel/action.cpp

	include/ui/action-pannel/action-list-widget.hpp
	src/ui/action-pannel/action-list-widget.cpp

	include/ui/action-pannel/action-list-item.hpp
	src/ui/action-pannel/action-list-item.cpp

	include/ui/argument-completer-widget.hpp

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

	include/clipboard-history-view.hpp

	include/ui/omni-painter.hpp
	src/ui/omni-painter.cpp

	include/wm/window-manager.hpp
	
	include/extension/extension-command.hpp
	include/extension/extension-view.hpp
	include/extension/extension-view-wrapper.hpp
	include/extension/extension-component.hpp

	include/extension/extension-form-component.hpp

	include/extension/extension-list-component.hpp
	src/extension/extension-list-component.cpp

	include/extension/extension-grid-component.hpp
	src/extension/extension-grid-component.cpp

	include/extension/extension.hpp
	src/extension/extension.cpp

	include/extension/extension-command.hpp
	src/extension/extension-command.cpp

	include/extension/form/extension-form-input.hpp
	include/extension/form/extension-text-field.hpp
	include/extension/form/extension-password-field.hpp
	include/extension/form/extension-checkbox-field.hpp
	include/extension/form/extension-dropdown.hpp

	include/extend/extension-detail-view.hpp


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

	include/base-view.hpp

	include/ui/ui-controller.hpp

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
	
	src/ui-controller.cpp

	src/xdg/xdg-desktop.cpp

	include/network-manager.hpp

	#src/commands/create-quicklink/create-quicklink.cpp

	src/omnicast.hpp
	src/omnicast.cpp

	./resources.qrc
	./icons/icons.qrc

	./database/omnicast/migrations.qrc
	./database/clipboard/migrations.qrc

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

	# settings
	src/settings/settings-window.hpp
	src/settings/settings-window.cpp

	src/settings/extension-settings.hpp
	src/settings/command-metadata-settings-detail.cpp
	src/settings/extension-settings-detail.cpp
	src/settings/app-settings-detail.cpp

	src/ui/omni-tree.hpp

	src/extensions/clipboard/clipboard-history-command.cpp

	src/extensions/omnicast/refresh-apps-command.hpp
	src/extensions/omnicast/refresh-apps-command.cpp
	src/extensions/omnicast/open-documentation-command.hpp
	src/extensions/omnicast/open-documentation-command.cpp

	src/ui/switch/switch.hpp
	src/ui/switch/switch.cpp
	src/ui/file-picker/file-picker.hpp
	src/ui/file-picker/file-picker.cpp
	src/ui/file-picker/file-picker-default-item-delegate.hpp
	src/ui/file-picker/file-picker-default-item-delegate.cpp

	src/ui/typography/typography.hpp
	src/ui/typography/typography.cpp

	src/ui/preference-dropdown/preference-dropdown.hpp
	src/ui/preference-dropdown/preference-dropdown.cpp
	src/ui/app-selector/app-selector.hpp
	src/ui/app-selector/app-selector.cpp

	src/utils/utils.cpp

	src/services/file-chooser/abstract-file-chooser.hpp
	src/services/file-chooser/xdp-file-chooser/xdp-file-chooser.hpp
	src/services/file-chooser/xdp-file-chooser/xdp-file-chooser.cpp
	src/services/root-item-manager/root-item-manager.hpp
	src/services/root-item-manager/root-item-manager.cpp
	
	src/services/app-service/app-service.hpp
	src/services/app-service/app-service.cpp

	src/utils/migration-manager/migration-manager.hpp
	src/utils/migration-manager/migration-manager.cpp

	src/utils/layout.hpp
	src/utils/layout.cpp
	
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
