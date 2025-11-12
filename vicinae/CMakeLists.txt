set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(LIBS)
set(TARGET vicinae)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql Network Svg DBus Concurrent)
find_package(OpenSSL REQUIRED)

list(APPEND LIBS  Qt6::Widgets Qt6::Sql Qt6::Network Qt6::Svg Qt6::DBus Qt6::Concurrent ${CMARK_LIBRARY} ${CMARK_EXT_LIBRARY} protobuf::libprotobuf minizip OpenSSL::Crypto wayland-client xdgpp qt6keychain)

set(WLR_CLIP_BIN ${CMAKE_BINARY_DIR}/wlr-clip/wlr-clip${CMAKE_EXECUTABLE_SUFFIX})
set(ASSET_PATH ${CMAKE_CURRENT_SOURCE_DIR}/assets)
set(EXTRA_PATH ${CMAKE_SOURCE_DIR}/extra)
configure_file(resources.qrc.in resources.qrc)

if (WAYLAND_LAYER_SHELL)
	message(STATUS "Compiling with wayland layer shell support (for wlroots-like compositors)")
	#find_package(Qt6 REQUIRED COMPONENTS WaylandClient)
	if (USE_SYSTEM_LAYER_SHELL)
		find_package(LayerShellQt REQUIRED)
		list(APPEND LIBS LayerShellQt::Interface)
	else()
		list(APPEND LIBS LayerShellQtInterface)
	endif()
	add_compile_definitions(WAYLAND_LAYER_SHELL)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_definitions(QT_MESSAGELOGCONTEXT)
else()
	add_compile_definitions(QT_NO_DEBUG_OUTPUT)
endif()

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/lib
	${CMAKE_CURRENT_SOURCE_DIR}/src/utils
	${CMAKE_CURRENT_SOURCE_DIR}/..
	${Protobuf_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}  # For generated headers
    ${CMAKE_BINARY_DIR}/generated  # For version.h
)

set(PROTO_SRC_DIR ${CMAKE_SOURCE_DIR}/proto)

file(GLOB PROTO_FILES "${PROTO_SRC_DIR}/*.proto")

set(SRCS
	src/main.cpp

	include/theme.hpp
	src/theme.cpp

	src/cli/cli.hpp
	src/cli/cli.cpp
	src/cli/theme.hpp
	src/cli/theme.cpp
	src/cli/server.hpp
	src/cli/server.cpp

	src/ui/spinner/spinner.hpp

	src/ui/dmenu-view/dmenu-view.hpp
	src/ui/dmenu-view/dmenu-view.cpp

	include/font-service.hpp
	src/font-service.cpp

	src/daemon/ipc-client.cpp
	src/lib/template-engine/template-engine.cpp

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

	src/ui/detail/detail-widget.hpp
	src/ui/detail/detail-widget.cpp
	src/ui/file-detail/file-detail.hpp
	src/ui/file-detail/file-detail.cpp

	src/ui/form/text-area.cpp

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

	src/actions/extension/extension-actions.cpp

	src/actions/app/app-actions.hpp
	src/actions/app/app-actions.cpp
	
	src/service-registry.cpp

	src/color-formatter.hpp
	src/color-formatter.cpp

	src/ui/popover/popover.hpp
	src/ui/popover/popover.cpp

	src/action-panel/action-panel.hpp

	src/ui/split-detail/split-detail.hpp
	src/ui/split-detail/split-detail.cpp

	src/services/clipboard/clipboard-server.hpp
	src/services/clipboard/clipboard-db.hpp
	src/services/clipboard/clipboard-db.cpp
	src/services/clipboard/qt/qt-clipboard-server.hpp
	src/services/clipboard/qt/qt-clipboard-server.cpp
	src/services/clipboard/x11/x11-clipboard-server.hpp
	src/services/clipboard/dummy/dummy-clipboard-server.hpp
	src/services/clipboard/dummy/dummy-clipboard-server.cpp
	src/services/clipboard/clipboard-server-factory.hpp
	src/services/clipboard/clipboard-server-factory.cpp
	src/services/clipboard/clipboard-service.hpp
	src/services/clipboard/clipboard-encrypter.cpp
	src/services/clipboard/clipboard-service.cpp
	src/services/clipboard/gnome/gnome-clipboard-server.hpp
	src/services/clipboard/gnome/gnome-clipboard-server.cpp

	src/services/local-storage/local-storage-service.hpp
	src/services/local-storage/local-storage.cpp

	src/services/local-storage/scoped-local-storage.cpp

	src/settings-controller/settings-controller.hpp
	src/settings-controller/settings-controller.cpp

	src/services/config/config-service.hpp

	src/services/toast/toast-service.hpp

	# begin app-database
	src/services/app-service/xdg/xdg-app-database.hpp
	src/services/app-service/xdg/xdg-app-database.cpp
	src/services/app-service/xdg/xdg-app.cpp
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

	src/lib/crypto.cpp


	src/ui/omni-painter/omni-painter.hpp
	src/ui/omni-painter/omni-painter.cpp

	
	include/extension/extension-command.hpp
	include/extension/extension-view.hpp
	include/extension/extension-view-wrapper.hpp

	include/extension/extension-form-component.hpp

	include/extension/extension-list-component.hpp
	src/extension/extension-list-component.cpp
	src/extension/extension-list.hpp
	src/extension/extension-list.cpp

	include/extension/extension-grid-component.hpp
	src/extension/extension-grid-component.cpp
	src/extension/extension-grid.hpp
	src/extension/extension-grid.cpp

	include/extension/extension.hpp
	src/extension/extension.cpp

	include/extension/extension-command.hpp
	src/extension/extension-command.cpp

	include/extension/form/extension-form-input.hpp
	include/extension/form/extension-text-field.hpp
	include/extension/form/extension-password-field.hpp
	include/extension/form/extension-checkbox-field.hpp
	include/extension/form/extension-dropdown.hpp
	include/extension/form/extension-date-picker.hpp

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

	src/ui/spinner/spinner.hpp
	src/ui/spinner/spinner.cpp

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


	src/manage-quicklinks-command.hpp

	include/ui/metadata-pane.hpp

	include/ui/horizontal-metadata.hpp
	src/ui/horizontal-metadata.cpp

	src/ui/vertical-metadata/vertical-metadata.hpp
	src/ui/vertical-metadata/vertical-metadata.cpp

	src/extend/accessory-model.cpp
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


	src/command.hpp
	

	src/vicinae.hpp
	src/vicinae.cpp


	../extra/extension-boilerplate/boilerplate.qrc

	contribs.qrc

	src/contribs/contribs.cpp

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
	src/ui/color-box/color-box.cpp

	src/lib/keyboard/keybind-manager.hpp
	src/lib/keyboard/keybind-manager.cpp
	src/lib/keyboard/keyboard.cpp

	src/ui/shortcut-recorder/shortcut-recorder.hpp
	src/ui/shortcut-recorder/shortcut-recorder.cpp

	# settings
	src/settings/settings-window.hpp
	src/settings/settings-window.cpp

	src/settings/extension-settings.hpp
	src/settings/command-metadata-settings-detail.cpp
	src/settings/extension-settings-detail.cpp
	src/settings/provider-settings-detail.cpp
	src/settings/keybind-settings.cpp

	src/ui/omni-tree/omni-tree.hpp
	src/ui/omni-tree/omni-tree.cpp

	src/theme/theme-file.hpp
	src/theme/theme-file.cpp
	src/theme/theme-parser.hpp
	src/theme/theme-parser.cpp
	src/theme/theme-db.hpp
	src/theme/theme-db.cpp

	src/extensions/vicinae/search-emojis/search-emojis-view.hpp
	src/extensions/vicinae/search-emojis/search-emojis-view.cpp
	src/extensions/clipboard/clipboard-history-command.cpp
	src/extensions/clipboard/history/clipboard-history-view.hpp
	src/extensions/clipboard/history/clipboard-history-view.cpp
	src/extensions/calculator/history/calculator-history-view.hpp
	src/extensions/calculator/history/calculator-history-view.cpp
	src/extensions/theme/manage/manage-themes-view.hpp
	src/extensions/theme/manage/manage-themes-view.cpp
	
	src/extensions/clipboard/clipboard-extension.hpp
	src/extensions/clipboard/clipboard-extension.cpp

	src/extensions/raycast/store/store-detail-view.cpp
	src/extensions/raycast/store/store-listing-view.cpp

	src/extensions/vicinae/refresh-apps-command.hpp
	src/extensions/vicinae/refresh-apps-command.cpp
	src/extensions/vicinae/oauth-token-store/oauth-token-store-view.hpp
	src/extensions/vicinae/oauth-token-store/oauth-token-store-view.cpp
	src/extensions/power-management/power-management-extension.cpp
	src/extensions/root/root-search-view.cpp

	src/extensions/system/browse-apps/browse-apps-view.cpp

	src/ui/screenshot-list/screenshot-list.hpp
	src/ui/switch/switch.hpp
	src/ui/switch/switch.cpp
	src/ui/file-picker/file-picker.hpp
	src/ui/file-picker/file-picker.cpp
	src/ui/file-picker/file-picker-default-item-delegate.hpp
	src/ui/file-picker/file-picker-default-item-delegate.cpp

	src/ui/text-file-viewer/text-file-viewer.cpp

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
	src/services/file-chooser/native/native-file-chooser.hpp
	src/services/file-chooser/native/native-file-chooser.cpp
	src/services/root-item-manager/root-item-manager.hpp
	src/services/root-item-manager/root-item-manager.cpp
	
	src/services/app-service/app-service.hpp
	src/services/app-service/app-service.cpp

	src/services/emoji-service/emoji.hpp
	src/services/emoji-service/emoji.cpp
	src/services/emoji-service/emoji-service.hpp
	src/services/emoji-service/emoji-service.cpp



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
	src/services/files-service/file-indexer/watcher-scanner.cpp
	src/services/files-service/file-indexer/home-directory-watcher.cpp
	src/services/files-service/file-indexer/file-indexer-db.cpp
	src/services/files-service/file-indexer/db-writer.cpp
	src/services/files-service/file-indexer/writer-worker.hpp
	src/services/files-service/file-indexer/writer-worker.cpp
	src/services/files-service/file-indexer/scan-dispatcher.cpp
	src/services/files-service/file-indexer/abstract-scanner.hpp

	src/services/extension-registry/extension-registry.hpp
	src/services/extension-registry/extension-registry.cpp

	src/services/oauth/oauth-token-store.hpp
	src/services/oauth/oauth-token-store.cpp

	src/ui/oauth-view/oauth-view.hpp
	src/ui/oauth-view/oauth-view.cpp



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
	src/extension/requests/oauth-router.cpp
	src/extension/requests/file-search-request-router.cpp
	src/extension/requests/app-request-router.cpp
	src/extension/requests/clipboard-request-router.cpp
	src/extension/requests/command-request-router.cpp
	src/extension/requests/wm-router.cpp
	src/extension/extension-command-runtime.cpp

	src/extensions/raycast/store/store-listing-view.hpp
	src/extensions/raycast/store/store-detail-view.hpp

	src/extensions/vicinae/vicinae-extension.cpp
	src/extensions/vicinae/store/store-listing-view.hpp
	src/extensions/vicinae/store/store-listing-view.cpp
	src/extensions/vicinae/store/store-detail-view.hpp
	src/extensions/vicinae/store/store-detail-view.cpp
	src/extensions/vicinae/list-installed-extensions-view.hpp
	src/extensions/vicinae/list-installed-extensions-view.cpp

	src/services/raycast/raycast-store.cpp
	src/services/extension-store/vicinae-store.cpp

	src/ui/thumbnail/thumbnail.hpp
	src/ui/thumbnail/thumbnail.cpp

	src/overlay-controller/overlay-controller.hpp

	src/ui/overlay/overlay.hpp
	src/ui/overlay/overlay.cpp

	src/lib/zip/unzip.cpp
	src/lib/data-uri/data-uri.cpp
	src/lib/pid-file/pid-file.cpp

	src/ui/hud/hud.hpp
	src/ui/hud/hud.cpp

	src/ui/button/button.hpp
	src/ui/button/button.cpp

	src/ui/shortcut-button/shortcut-button.hpp
	src/ui/shortcut-button/shortcut-button.cpp

	src/ui/search-bar/search-bar.hpp
	src/ui/search-bar/search-bar.cpp

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
	src/ui/keybinding-selector/keybinding-selector.cpp

	src/settings/general-settings.cpp
	src/settings/settings-about.cpp

	src/lib/os-release.cpp

	src/ui/arg-completer/arg-completer.hpp
	src/ui/arg-completer/arg-completer.cpp

	src/services/window-manager/window-manager.cpp

	src/services/extension-boilerplate-generator/extension-boilerplate-generator.cpp

	src/ui/file-picker-button/file-picker-button.hpp
	src/ui/file-picker-button/file-picker-button.cpp

	src/ui/form/directory-input.cpp

	src/services/window-manager/abstract-window-manager.hpp

	src/lib/program-db/program-db.hpp
	src/lib/program-db/program-db.cpp

	src/ui/button-base/button-base.hpp
	src/ui/button-base/button-base.cpp

	src/services/power-manager/systemd/systemd-power-manager.cpp

) 

if (UNIX)
	list(APPEND SRCS
		src/services/calculator-service/soulver-core/soulver-core.cpp
	)
endif()

if (LIBQALCULATE_BACKEND)
	list(APPEND LIBS qalculate)
	add_compile_definitions(HAS_QALCULATE=1)
	list(APPEND SRCS
		src/services/calculator-service/qalculate/qalculate-backend.hpp
		src/services/calculator-service/qalculate/qalculate-backend.cpp
	)
endif()



if (TYPESCRIPT_EXTENSIONS)
	list(APPEND SRCS ${CMAKE_CURRENT_BINARY_DIR}/resources.qrc)
endif()

if (UNIX AND NOT APPLE)
	list(APPEND SRCS
		#src/lib/xkbcommon-utils.cpp

		src/lib/icon-theme-db/icon-theme-db.cpp

		src/services/window-manager/hyprland/hyprland.cpp
		src/services/window-manager/hyprland/hypr-listener.hpp
		src/services/window-manager/hyprland/hypr-listener.cpp
		src/services/window-manager/hyprland/hyprctl.cpp
		src/services/window-manager/hyprland/hypr-workspace.cpp
		src/services/window-manager/hyprland/hypr-window.cpp
		src/services/window-manager/gnome/gnome-window-manager.cpp
		src/services/window-manager/gnome/gnome-window.cpp
		src/services/window-manager/gnome/gnome-workspace.cpp
		src/services/window-manager/gnome/gnome-listener.cpp
		src/services/window-manager/wayland/wayland.cpp
		src/services/window-manager/x11/x11-window-manager.cpp
		src/services/window-manager/x11/x11-event-listener.cpp
		src/services/window-manager/x11/x11-window.cpp

		src/lib/wayland/virtual-keyboard.hpp
		src/lib/wayland/virtual-keyboard.cpp
	)

	# Add XCB (X11 C Bindings) library for X11 window manager support
	find_package(X11 REQUIRED)
	if (X11_FOUND)
		list(APPEND LIBS X11::xcb)
		message(STATUS "XCB library found for X11 window manager support")
	endif()

    include("Wayland")

    wayland_generate_protocol("${CMAKE_CURRENT_SOURCE_DIR}/protocols/wlr-foreign-toplevel-management-unstable-v1.xml")
	wayland_generate_protocol("${CMAKE_CURRENT_SOURCE_DIR}/protocols/virtual-keyboard-unstable-v1.xml")
		wayland_generate_protocol("${CMAKE_CURRENT_SOURCE_DIR}/protocols/ext-data-control-v1.xml")

	list(APPEND LIBS xkbcommon)

	# Common Wayland support files
	list(APPEND SRCS
		src/lib/wayland/globals.cpp
		src/lib/wayland/display.cpp
		src/lib/wayland/registry.cpp
		src/lib/wayland/seat.cpp
		src/lib/wayland/virtual-keyboard.hpp
		src/lib/wayland/virtual-keyboard.cpp
	)

	# ext-data-control
	list(APPEND SRCS
		src/ext-clip/app.cpp
		src/ext-clip/data-control-client.cpp
		src/ext-clip/data-device.cpp
		src/ext-clip/data-offer.cpp
		src/services/clipboard/ext/ext-clipboard-server.hpp
		src/services/clipboard/ext/ext-clipboard-server.cpp
	)

	# wlr-data-control
	list(APPEND SRCS
		src/wlr-clip/app.cpp
		src/wlr-clip/data-control-client.cpp
		src/wlr-clip/data-device.cpp
		src/wlr-clip/data-offer.cpp
		src/services/clipboard/wlr/wlr-clipboard-server.hpp
		src/services/clipboard/wlr/wlr-clipboard-server.cpp
	)
	wayland_generate_protocol("${CMAKE_CURRENT_SOURCE_DIR}/protocols/wlr-data-control-unstable-v1.xml")
endif()
	

qt_add_executable(${TARGET} ${SRCS})

if(Qt6_FOUND)
	qt_import_plugins(${TARGET}
        INCLUDE Qt6::QWaylandIntegrationPlugin
    )
endif()

target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${TARGET} PRIVATE ${LIBS})

if (USE_PRECOMPILED_HEADERS)
	message(STATUS "Precompiled headers are enabled")
	target_precompile_headers(${TARGET} PUBLIC "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h>")
	set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)
else()
	message(STATUS "Precompiled headers are disabled")
endif()

make_directory(${CMAKE_CURRENT_BINARY_DIR}/proto)

set(PROTO_IMPORT_DIRS ${PROTO_SRC_DIR} ${COMMON_PROTO_DIR})

if (NOT USE_SYSTEM_PROTOBUF)
	list(APPEND PROTO_IMPORT_DIRS ${protobuf_SOURCE_DIR}/src)
endif()

protobuf_generate(
	TARGET ${TARGET}
	PROTOS ${PROTO_FILES}
	IMPORT_DIRS ${PROTO_IMPORT_DIRS}
	PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto
)

install(TARGETS ${TARGET})
