set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(LIBS)

set(TARGET vicinae)


find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Network Svg DBus Keychain)
find_package(OpenSSL REQUIRED)

# https://cmake.org/cmake/help/latest/module/FindProtobuf.html#example-finding-protobuf-in-config-mode
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)
find_package(Protobuf)
unset(CMAKE_FIND_PACKAGE_PREFER_CONFIG)

option(WAYLAND_LAYER_SHELL "Enable support for the Wayland layer shell
protocol, required for proper window positioning on most wlroots-based
compositors." ON)


list(APPEND LIBS Qt6::Widgets Qt6::Sql Qt6::Network Qt6::Svg Qt6::DBus qt6keychain cmark-gfm qalculate protobuf::libprotobuf minizip OpenSSL::Crypto)

set(WLR_CLIP_BIN ${CMAKE_BINARY_DIR}/wlr-clip/wlr-clip${CMAKE_EXECUTABLE_SUFFIX})
set(ASSET_PATH ${CMAKE_CURRENT_SOURCE_DIR}/assets)
configure_file(resources.qrc.in resources.qrc)


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
	${CMAKE_CURRENT_SOURCE_DIR}/..
	${Protobuf_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}  # For generated headers
)

set(PROTO_SRC_DIR ${CMAKE_SOURCE_DIR}/proto/extensions)


file(GLOB PROTO_FILES "${PROTO_SRC_DIR}/*.proto")

message(STATUS "Protobuf version: ${protobuf_VERSION}")
message(STATUS "Protobuf compiler: ${protobuf_PROTOC_EXE}")

set(SRCS
	src/main.cpp

	include/theme.hpp
	src/theme.cpp

	include/font-service.hpp
	src/font-service.cpp

	src/daemon/ipc-client.cpp

	include/favicon/favicon-service.hpp
	src/favicon/favicon-service.cpp
	include/favicon/favicon-request.hpp
	src/favicon/favicon-request.cpp
	include/favicon/google-favicon-request.hpp
	src/favicon/google-favicon-request.cpp
	include/favicon/twenty-favicon-request.hpp
	src/favicon/twenty-favicon-request.cpp
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
	src/root-search/shortcuts/shortcut-root-provider.hpp
	src/root-search/shortcuts/shortcut-root-provider.cpp
	src/root-search/apps/app-root-provider.hpp
	src/root-search/apps/app-root-provider.cpp
	
	src/extension/manager/extension-manager.hpp
	src/extension/manager/extension-manager.cpp

	# Bookmark - Start
	src/services/shortcut/shortcut-service.hpp
	src/services/shortcut/shortcut-service.cpp
	src/services/shortcut/shortcut.hpp
	src/services/shortcut/shortcut.cpp

	src/actions/theme/theme-actions.hpp
	src/actions/theme/theme-actions.cpp

	src/actions/calculator/calculator-actions.hpp
	src/actions/calculator/calculator-actions.cpp

	src/actions/root-search/root-search-actions.hpp
	src/actions/root-search/root-search-actions.cpp

	src/actions/app/app-actions.hpp
	src/actions/app/app-actions.cpp
	
	src/service-registry.cpp

	src/color-formatter.hpp
	src/color-formatter.cpp

	src/ui/popover/popover.hpp
	src/ui/popover/popover.cpp

	src/action-panel/action-panel.hpp

	include/omni-command-db.hpp

	src/ui/split-detail/split-detail.hpp
	src/ui/split-detail/split-detail.cpp

	src/services/clipboard/clipboard-server.hpp
	src/services/clipboard/clipboard-db.hpp
	src/services/clipboard/clipboard-db.cpp
	src/services/clipboard/wlr/wlr-clipboard-server.hpp
	src/services/clipboard/wlr/wlr-clipboard-server.cpp
	src/services/clipboard/dummy/dummy-clipboard-server.hpp
	src/services/clipboard/dummy/dummy-clipboard-server.cpp
	src/services/clipboard/clipboard-server-factory.hpp
	src/services/clipboard/clipboard-server-factory.cpp
	src/services/clipboard/clipboard-service.hpp
	src/services/clipboard/clipboard-service.cpp

	src/services/local-storage/local-storage-service.hpp
	src/services/local-storage/local-storage.cpp

	src/settings-controller/settings-controller.hpp
	src/settings-controller/settings-controller.cpp

	src/services/config/config-service.hpp

	src/services/toast/toast-service.hpp

	# begin app-database
	src/services/app-service/xdg/xdg-app-database.hpp
	src/services/app-service/xdg/xdg-app-database.cpp
	src/services/app-service/abstract-app-db.hpp

	# end app-database

	# begin action-pannel-widget
	include/ui/action-pannel/action.hpp
	src/ui/action-pannel/action.cpp

	include/ui/action-pannel/action-list-widget.hpp
	src/ui/action-pannel/action-list-widget.cpp

	include/ui/action-pannel/action-list-item.hpp
	src/ui/action-pannel/action-list-item.cpp

	# end action-pannel-widget

	src/ui/loading-bar/horizontal-loading-bar.hpp
	src/ui/loading-bar/horizontal-loading-bar.cpp

	# start markdown renderer
	src/ui/markdown/markdown-renderer.hpp
	src/ui/markdown/markdown-renderer.cpp
	# end markdown renderer

	include/lib/emoji-detect.hpp
	src/lib/emoji-detect.cpp
	src/lib/crypto.cpp


	include/clipboard-history-view.hpp

	src/ui/omni-painter/omni-painter.hpp
	src/ui/omni-painter/omni-painter.cpp

	
	include/extension/extension-command.hpp
	include/extension/extension-view.hpp
	include/extension/extension-view-wrapper.hpp

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


	src/ui/image/url.cpp
	src/ui/image/image.hpp
	src/ui/image/image.cpp
	src/ui/image/static-image-loader.cpp
	src/ui/image/animated-image-loader.cpp
	src/ui/image/io-image-loader.cpp
	src/ui/image/local-image-loader.cpp
	src/ui/image/http-image-loader.cpp
	src/ui/image/data-uri-image-loader.cpp
	src/ui/image/favicon-image-loader.cpp
	src/ui/image/svg-image-loader.cpp
	src/ui/image/builtin-icon-loader.cpp
	src/ui/image/qicon-image-loader.cpp
	src/ui/image/emoji-image-loader.cpp

	src/ui/icon-button/icon-button.hpp
	src/ui/icon-button/icon-button.cpp

	include/command-database.hpp
	src/command-database.cpp

	# omni list 
	src/ui/omni-list/omni-list.hpp
	src/ui/omni-list/omni-list.cpp

	src/ui/omni-list/omni-list-item-widget-wrapper.hpp
	src/ui/omni-list/omni-list-item-widget-wrapper.cpp

	src/ui/selectable-omni-list-widget/selectable-omni-list-widget.hpp
	src/ui/selectable-omni-list-widget/selectable-omni-list-widget.cpp

	include/ui/list-section-header.hpp
	src/ui/list-section-header.cpp

	src/ui/default-list-item-widget/default-list-item-widget.hpp
	src/ui/default-list-item-widget/default-list-item-widget.cpp

	src/ui/omni-grid/grid-item-content-widget.hpp
	src/ui/omni-grid/grid-item-content-widget.cpp

	src/ui/tooltip/tooltip.hpp
	src/ui/tooltip/tooltip.cpp

	src/ui/omni-grid/grid-item-widget.hpp
	src/ui/omni-grid/grid-item-widget.cpp

	src/ui/omni-list/omni-list-item-widget.hpp

		src/ui/image/url.hpp


	src/calculator-history-command.hpp

	include/create-quicklink-command.hpp

	include/builtin_icon.hpp
	src/builtin_icon.cpp

	src/ui/transform-result/transform-result.hpp
	src/ui/transform-result/transform-result.cpp

	src/ui/color-transform/color-transform-widget.hpp
	src/ui/color-transform/color-transform-widget.cpp

	src/ui/emoji-viewer/emoji-viewer.hpp
	src/ui/emoji-viewer/emoji-viewer.cpp

	src/ui/empty-view/empty-view.hpp
	src/ui/empty-view/empty-view.cpp

	src/ui/keyboard-shortcut-indicator/keyboard-shortcut-indicator.hpp
	src/ui/keyboard-shortcut-indicator/keyboard-shortcut-indicator.cpp

	include/emoji-command.hpp

	src/manage-quicklinks-command.hpp

	include/ui/metadata-pane.hpp

	include/ui/horizontal-metadata.hpp
	src/ui/horizontal-metadata.cpp

	src/ui/vertical-metadata/vertical-metadata.hpp
	src/ui/vertical-metadata/vertical-metadata.cpp

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


	src/image-fetcher.hpp


	src/root-command.hpp
	src/command.hpp
	

	src/xdg/xdg-desktop.cpp

	src/vicinae.hpp
	src/vicinae.cpp

	${CMAKE_CURRENT_BINARY_DIR}/resources.qrc

	./icons/icons.qrc

	./database/vicinae/migrations.qrc
	./database/clipboard/migrations.qrc
	./database/file-indexer//migrations.qrc

	src/ui/inline-input/inline_qline_edit.hpp
	src/ui/inline-input/inline_qline_edit.cpp
	src/ui/color-circle/color_circle.hpp
	src/ui/color-circle/color_circle.cpp
	src/ui/toast/toast.hpp
	src/ui/toast/toast.cpp

	# settings
	src/settings/settings-window.hpp
	src/settings/settings-window.cpp

	src/settings/extension-settings.hpp
	src/settings/command-metadata-settings-detail.cpp
	src/settings/extension-settings-detail.cpp
	src/settings/app-settings-detail.cpp

	src/ui/omni-tree/omni-tree.hpp

	src/extensions/clipboard/clipboard-history-command.cpp

	src/extensions/vicinae/refresh-apps-command.hpp
	src/extensions/vicinae/refresh-apps-command.cpp

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

	src/services/emoji-service/emoji.hpp
	src/services/emoji-service/emoji.cpp
	src/services/emoji-service/emoji-service.hpp
	src/services/emoji-service/emoji-service.cpp

	src/services/calculator-service/qalculate/qalculate-backend.hpp
	src/services/calculator-service/qalculate/qalculate-backend.cpp
	src/services/calculator-service/calculator-service.hpp
	src/services/calculator-service/calculator-service.cpp

	src/services/files-service/abstract-file-indexer.hpp
	src/services/files-service/file-service.hpp
	src/services/files-service/file-service.cpp
	src/services/files-service/file-indexer/file-indexer.hpp
	src/services/files-service/file-indexer/file-indexer.cpp
	src/services/files-service/file-indexer/filesystem-walker.cpp
	src/services/files-service/file-indexer/relevancy-scorer.cpp
	src/services/files-service/file-indexer/incremental-scanner.cpp
	src/services/files-service/file-indexer/indexer-scanner.cpp
	src/services/files-service/file-indexer/home-directory-watcher.cpp
	src/services/files-service/file-indexer/file-indexer-db.cpp

	src/services/extension-registry/extension-registry.hpp
	src/services/extension-registry/extension-registry.cpp


	src/utils/migration-manager/migration-manager.hpp
	src/utils/migration-manager/migration-manager.cpp

	src/utils/layout.hpp
	src/utils/layout.cpp

	src/ui/top-bar/top-bar.hpp
	src/ui/top-bar/top-bar.cpp

	src/navigation-controller.hpp
	src/navigation-controller.cpp

	src/ui/views/base-view.cpp
	src/ui/views/simple-view.cpp
	src/ui/views/list-view.cpp
	src/ui/views/grid-view.cpp
	#src/ui/views/form-view.cpp
	
	src/ui/launcher-window/launcher-window.cpp

	src/ui/status-bar/status-bar.hpp
	src/ui/status-bar/status-bar.cpp

	src/command-controller.hpp
	src/command-controller.cpp

	src/ipc-command-server.hpp
	src/ipc-command-server.cpp
	src/ipc-command-handler.hpp
	src/ipc-command-handler.cpp

	src/log/message-handler.cpp

	src/extension/requests/storage-request-router.cpp
	src/extension/requests/ui-request-router.cpp
	src/extension/requests/app-request-router.cpp
	src/extension/requests/clipboard-request-router.cpp
	src/extension/extension-command-runtime.cpp

	src/extensions/raycast/store/store-listing-view.hpp
	src/extensions/raycast/store/store-detail-view.hpp

	src/services/raycast/raycast-store.cpp

	src/ui/thumbnail/thumbnail.hpp
	src/ui/thumbnail/thumbnail.cpp

	src/overlay-controller/overlay-controller.hpp

	src/ui/overlay/overlay.hpp
	src/ui/overlay/overlay.cpp

	src/lib/zip/unzip.cpp
	src/lib/data-uri/data-uri.cpp

	src/ui/hud/hud.hpp
	src/ui/hud/hud.cpp

	src/ui/button-base/button-base.hpp
	src/ui/button-base/button-base.cpp
	src/ui/button/button.hpp
	src/ui/button/button.cpp

	src/ui/shortcut-button/shortcut-button.hpp
	src/ui/shortcut-button/shortcut-button.cpp

	src/ui/search-bar/search-bar.hpp

	src/ui/vertical-scroll-area/vertical-scroll-area.cpp

	src/ui/alert/alert.hpp
	src/ui/alert/alert.cpp

	src/ui/dialog/dialog.hpp
	src/ui/dialog/dialog.cpp

	src/ui/list-accessory/list-accessory.hpp
	src/ui/list-accessory/list-accessory.cpp

	src/ui/scroll-bar/scroll-bar.hpp
	src/ui/scroll-bar/scroll-bar.cpp

	src/ui/flow-layout/flow-layout.hpp
	src/ui/flow-layout/flow-layout.cpp

	src/ui/tag/tag.cpp

	src/ui/theme-selector/theme-selector.cpp
	src/ui/qtheme-selector/qtheme-selector.cpp
	src/ui/font-selector/font-selector.cpp
	src/ui/favicon-service-selector/favicon-service-selector.cpp

	src/settings/general-settings.cpp
	src/settings/settings-about.cpp

	src/ui/arg-completer/arg-completer.hpp
	src/ui/arg-completer/arg-completer.cpp

	src/root-search.cpp

	src/services/window-manager/window-manager.cpp
) 


if (UNIX AND NOT APPLE)
	list(APPEND SRCS
		src/lib/xkbcommon-utils.cpp

		src/services/window-manager/hyprland/hyprland.cpp

		# Hyprland
		# End Hyprland
	)
endif()
	

qt_add_executable(${TARGET} ${SRCS})

add_dependencies(${TARGET} wlr-clip)
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${TARGET} PRIVATE ${LIBS})

make_directory(${CMAKE_CURRENT_BINARY_DIR}/proto)

protobuf_generate(
	TARGET ${TARGET}
	PROTOS ${PROTO_FILES}
	IMPORT_DIRS ${PROTO_SRC_DIR} ${COMMON_PROTO_DIR}
	PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto
)

install(TARGETS ${TARGET})
