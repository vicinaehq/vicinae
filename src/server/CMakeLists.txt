set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(LIBS)
set(TARGET vicinae-server)

find_package(Qt6 REQUIRED COMPONENTS Core Sql Network Svg DBus Concurrent Quick Qml GuiPrivate QuickDialogs2 QuickControls2)
find_package(OpenSSL REQUIRED)
find_package(LibXml2 REQUIRED)
if (USE_SYSTEM_KF6)
	find_package(KF6SyntaxHighlighting REQUIRED)
endif()

list(APPEND LIBS  
	Qt6::Sql Qt6::Network Qt6::Svg Qt6::DBus Qt6::Concurrent Qt6::Quick Qt6::Qml
	Qt6::GuiPrivate # for deeper integration with wayland protocols, we need wl_surface
	Qt6::QuickDialogs2 Qt6::QuickControls2
	${CMARK_LIBRARY} ${CMARK_EXT_LIBRARY} 
	protobuf::libprotobuf
	minizip 
	OpenSSL::Crypto 
	wayland-client 
	qt6keychain
	LibXml2::LibXml2
	KF6::SyntaxHighlighting
	glaze::glaze
	vicinae::xdgpp
	vicinae::scriptcommand
	vicinae::emoji
	vicinae::ipc
	vicinae::snippet-types
	vicinae::common
)

set(ASSET_PATH ${CMAKE_CURRENT_SOURCE_DIR}/assets)
set(EXTRA_PATH ${CMAKE_SOURCE_DIR}/extra)
configure_file(resources.qrc.in resources.qrc)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
	add_compile_definitions(LOCAL_THEME_DIR="${THEME_DIR}")
endif()

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
)

file(GLOB PROTO_FILES "${PROTO_DIR}/*.proto")

set(SRCS
	src/main.cpp

	src/theme.hpp
	src/theme.cpp

	src/script/script-command-file.cpp
	src/script/script-output-tokenizer.cpp
	src/script/script-metadata-store.cpp
	src/script/script-scanner.cpp
	src/script/script-actions.cpp
	src/services/script-command/script-command-service.hpp
	src/services/script-command/script-command-service.cpp

	src/placeholder.cpp


	include/font-service.hpp
	src/font-service.cpp

	#src/daemon/ipc-client.cpp
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

	src/services/background-effect/ext-background-effect-v1-manager.cpp
	src/services/background-effect/kde-background-effect-manager.cpp

	src/color-formatter.hpp
	src/color-formatter.cpp

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

	src/services/toast/toast-service.hpp

	# begin app-database
	src/services/app-service/xdg/xdg-app-database.hpp
	src/services/app-service/xdg/xdg-app-database.cpp
	src/services/app-service/xdg/xdg-app.cpp
	src/services/app-service/abstract-app-db.hpp

	# end app-database

	include/ui/action-pannel/action.hpp
	src/ui/action-pannel/action.cpp

	src/lib/crypto.cpp

	
	include/extension/extension-command.hpp

	src/extension/extension-action-panel-builder.hpp
	src/extension/extension-action-panel-builder.cpp

	include/extension/extension.hpp
	src/extension/extension.cpp

	include/extension/extension-command.hpp
	src/extension/extension-command.cpp



	src/ui/omni-painter/omni-painter.hpp
	src/ui/omni-painter/omni-painter.cpp

	src/ui/image/url.cpp
	src/ui/image/image.hpp
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
	src/ui/image/image-decoder.hpp

	include/command-database.hpp
	src/command-database.cpp

		src/ui/image/url.hpp


	include/create-quicklink-command.hpp

	src/builtin_icon.cpp


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

	src/image-fetcher.hpp


	src/command.hpp
	

	src/vicinae.hpp
	src/vicinae.cpp


	${EXTRA_PATH}/extension-boilerplate/boilerplate.qrc

	./icons/icons.qrc

	./database/vicinae/migrations.qrc
	./database/clipboard/migrations.qrc
	./database/file-indexer//migrations.qrc


	src/lib/keyboard/keybind-manager.hpp
	src/lib/keyboard/keybind-manager.cpp
	src/lib/keyboard/keyboard.cpp

	src/theme/theme-file.hpp
	src/theme/theme-file.cpp
	src/theme/theme-parser.hpp
	src/theme/theme-parser.cpp
	src/theme/theme-db.hpp
	src/theme/theme-db.cpp

	src/extensions/clipboard/clipboard-history-command.cpp
	src/extensions/clipboard/history/clipboard-history-controller.hpp
	src/extensions/clipboard/history/clipboard-history-controller.cpp
	src/extensions/browser/browser-extension.cpp

	src/extensions/clipboard/clipboard-extension.hpp
	src/extensions/clipboard/clipboard-extension.cpp

	src/extensions/vicinae/refresh-apps-command.hpp
	src/extensions/vicinae/refresh-apps-command.cpp
	src/extensions/power-management/power-management-extension.cpp

	src/qml/browse-apps-model.hpp
	src/qml/browse-apps-model.cpp
	src/qml/browse-apps-view-host.hpp
	src/qml/browse-apps-view-host.cpp
	src/qml/browser-tabs-model.hpp
	src/qml/browser-tabs-model.cpp
	src/qml/browser-tabs-view-host.hpp
	src/qml/browser-tabs-view-host.cpp
	src/qml/builtin-icons-model.hpp
	src/qml/builtin-icons-model.cpp
	src/qml/builtin-icons-view-host.hpp
	src/qml/builtin-icons-view-host.cpp
	src/qml/manage-fallback-model.hpp
	src/qml/manage-fallback-model.cpp
	src/qml/manage-fallback-view-host.hpp
	src/qml/manage-fallback-view-host.cpp
	src/qml/system-run-model.hpp
	src/qml/system-run-model.cpp
	src/qml/system-run-view-host.hpp
	src/qml/system-run-view-host.cpp
	src/qml/create-extension-success-view-host.hpp
	src/qml/create-extension-success-view-host.cpp
	src/qml/script-executor-view-host.hpp
	src/qml/script-executor-view-host.cpp
	src/qml/provider-search-model.hpp
	src/qml/provider-search-model.cpp
	src/qml/provider-search-view-host.hpp
	src/qml/provider-search-view-host.cpp

	src/cli/cli.cpp
	src/cli/server.cpp


	src/utils/utils.cpp

	src/services/file-chooser/abstract-file-chooser.hpp
	src/services/file-chooser/xdp-file-chooser/xdp-file-chooser.hpp
	src/services/file-chooser/xdp-file-chooser/xdp-file-chooser.cpp
	src/services/root-item-manager/root-item-manager.hpp
	src/services/root-item-manager/root-item-manager.cpp
	src/services/root-item-manager/visit-tracker.cpp
	
	src/services/app-service/app-service.hpp
	src/services/app-service/app-service.cpp

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
	src/services/files-service/file-indexer/file-indexer-query-engine.hpp

	src/services/extension-registry/extension-registry.hpp
	src/services/extension-registry/extension-registry.cpp
	src/services/extension-registry/extension-manifest.hpp
	src/services/extension-registry/extension-manifest.cpp

	src/services/oauth/oauth-token-store.hpp
	src/services/oauth/oauth-token-store.cpp

	src/qml/oauth-overlay-host.hpp
	src/qml/oauth-overlay-host.cpp



	src/utils/migration-manager/migration-manager.hpp
	src/utils/migration-manager/migration-manager.cpp

	src/navigation-controller.hpp
	src/navigation-controller.cpp

	src/ui/views/base-view.cpp
	#src/ui/views/form-view.cpp

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

	src/extensions/vicinae/vicinae-extension.cpp

	src/qml/search-files-model.hpp
	src/qml/search-files-model.cpp
	src/qml/search-files-view-host.hpp
	src/qml/search-files-view-host.cpp

	src/services/raycast/raycast-store.cpp
	src/services/extension-store/vicinae-store.cpp

	src/overlay-controller/overlay-controller.hpp


	src/lib/zip/unzip.cpp
	src/lib/data-uri/data-uri.cpp
	src/lib/pid-file/pid-file.cpp

	src/ui/alert/alert.hpp
	src/ui/alert/alert.cpp

	src/ui/dialog/dialog.hpp
	src/ui/dialog/dialog.cpp

	src/lib/os-release.cpp

	src/services/window-manager/window-manager.cpp

	src/services/extension-boilerplate-generator/extension-boilerplate-generator.cpp

	src/services/window-manager/abstract-window-manager.hpp
	src/services/browser-extension-service.hpp

	src/lib/program-db/program-db.hpp
	src/lib/program-db/program-db.cpp

	src/services/power-manager/systemd/systemd-power-manager.cpp

	src/config/config.hpp
	src/config/config.cpp

	src/qml/local-storage-model.hpp
	src/qml/local-storage-model.cpp
	src/qml/local-storage-view-host.hpp
	src/qml/local-storage-view-host.cpp

	src/qml/dmenu-model.hpp
	src/qml/dmenu-model.cpp
	src/qml/dmenu-view-host.hpp
	src/qml/dmenu-view-host.cpp

	src/services/snippet/snippet-service.hpp
	src/services/snippet/snippet-server.hpp
	src/services/snippet/snippet-db.cpp
	src/services/ai/ai-provider.hpp
	src/services/ai/ai-service.hpp

	# QML frontend (POC)
	src/qml/keybind-bridge.hpp
	src/qml/launcher-window.hpp
	src/qml/launcher-window.cpp
	src/qml/root-search-model.hpp
	src/qml/root-search-model.cpp
	src/qml/command-list-model.hpp
	src/qml/command-list-model.cpp
	src/qml/action-panel-model.hpp
	src/qml/action-panel-model.cpp
	src/qml/action-panel-controller.hpp
	src/qml/action-panel-controller.cpp
	src/qml/alert-model.hpp
	src/qml/alert-model.cpp
	src/qml/bridge-view.hpp
	src/qml/calc-history-model.hpp
	src/qml/calc-history-model.cpp
	src/qml/theme-list-model.hpp
	src/qml/theme-list-model.cpp
	src/qml/command-grid-model.hpp
	src/qml/command-grid-model.cpp
	src/qml/emoji-grid-model.hpp
	src/qml/emoji-grid-model.cpp
	src/qml/view-utils.cpp
	src/qml/extension-view-host.hpp
	src/qml/extension-view-host.cpp
	src/qml/extension-error-view-host.hpp
	src/qml/extension-error-view-host.cpp
	src/qml/empty-view-host.hpp
	src/qml/empty-view-host.cpp
	src/qml/missing-preference-view-host.hpp
	src/qml/missing-preference-view-host.cpp
	src/qml/extension-form-model.hpp
	src/qml/extension-form-model.cpp
	src/qml/clipboard-history-model.hpp
	src/qml/clipboard-history-model.cpp
	src/qml/clipboard-history-view-host.hpp
	src/qml/clipboard-history-view-host.cpp
	src/qml/snippet-form-view-host.hpp
	src/qml/snippet-form-view-host.cpp
	src/qml/alias-form-view-host.hpp
	src/qml/alias-form-view-host.cpp
	src/qml/app-selector-model.hpp
	src/qml/app-selector-model.cpp
	src/qml/shortcut-form-view-host.hpp
	src/qml/shortcut-form-view-host.cpp
	src/qml/extension-list-model.hpp
	src/qml/extension-list-model.cpp
	src/qml/extension-grid-model.hpp
	src/qml/extension-grid-model.cpp
	src/qml/source-blend-rect.hpp
	src/qml/image-url.hpp
	src/qml/image-url.cpp
	src/qml/image-source.hpp
	src/qml/image-source.cpp
	src/qml/animated-image.hpp
	src/qml/animated-image.cpp
	src/qml/async-image-provider.hpp
	src/qml/async-image-provider.cpp
	src/qml/config-bridge.hpp
	src/qml/theme-bridge.hpp
	src/qml/create-extension-view-host.hpp
	src/qml/create-extension-view-host.cpp
	src/qml/fuzzy-list-model.hpp
	src/qml/installed-extensions-model.hpp
	src/qml/installed-extensions-model.cpp
	src/qml/installed-extensions-view-host.hpp
	src/qml/installed-extensions-view-host.cpp
	src/qml/oauth-token-store-model.hpp
	src/qml/oauth-token-store-model.cpp
	src/qml/oauth-token-store-view-host.hpp
	src/qml/oauth-token-store-view-host.cpp
	src/qml/switch-windows-model.hpp
	src/qml/switch-windows-model.cpp
	src/qml/switch-windows-view-host.hpp
	src/qml/switch-windows-view-host.cpp
	src/qml/manage-shortcuts-model.hpp
	src/qml/manage-shortcuts-model.cpp
	src/qml/manage-shortcuts-view-host.hpp
	src/qml/manage-shortcuts-view-host.cpp
	src/qml/manage-snippets-model.hpp
	src/qml/manage-snippets-model.cpp
	src/qml/manage-snippets-view-host.hpp
	src/qml/manage-snippets-view-host.cpp
	src/qml/raycast-store-model.hpp
	src/qml/raycast-store-model.cpp
	src/qml/raycast-store-view-host.hpp
	src/qml/raycast-store-view-host.cpp
	src/qml/vicinae-store-model.hpp
	src/qml/vicinae-store-model.cpp
	src/qml/vicinae-store-view-host.hpp
	src/qml/vicinae-store-view-host.cpp
	src/qml/raycast-store-detail-host.hpp
	src/qml/raycast-store-detail-host.cpp
	src/qml/vicinae-store-detail-host.hpp
	src/qml/vicinae-store-detail-host.cpp
	src/qml/markdown/markdown-model.hpp
	src/qml/markdown/markdown-model.cpp
	src/qml/markdown/text-selection-controller.hpp
	src/qml/markdown/text-selection-controller.cpp
	src/qml/store-intro-view-host.hpp
	src/qml/store-intro-view-host.cpp
	src/qml/font-browser-model.hpp
	src/qml/font-browser-view-host.hpp
	src/qml/font-browser-view-host.cpp
	src/qml/edit-keywords-view-host.hpp
	src/qml/edit-keywords-view-host.cpp
	src/qml/hud-bridge.hpp
	src/qml/hud-bridge.cpp

	src/qml/settings-window.hpp
	src/qml/settings-window.cpp
	src/qml/general-settings-model.hpp
	src/qml/general-settings-model.cpp
	src/qml/keybind-settings-model.hpp
	src/qml/keybind-settings-model.cpp
	src/qml/extension-settings-model.hpp
	src/qml/extension-settings-model.cpp
	src/qml/preference-form-model.hpp
	src/qml/preference-form-model.cpp

	src/extensions/internal/markdown-showcase-command.hpp

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
		src/services/window-manager/niri/niri.cpp
		src/services/window-manager/gnome/gnome-window-manager.cpp
		src/services/window-manager/gnome/gnome-window.cpp
		src/services/window-manager/gnome/gnome-workspace.cpp
		src/services/window-manager/gnome/gnome-listener.cpp
		src/services/window-manager/wayland/wayland.cpp
		src/services/window-manager/kde/kde-window-manager.hpp
		src/services/window-manager/kde/kde-window-manager.cpp
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

    wayland_generate_protocol("wlr-foreign-toplevel-management-unstable-v1")
	wayland_generate_protocol("virtual-keyboard-unstable-v1")
	wayland_generate_protocol("ext-data-control-v1")
	wayland_generate_protocol("kde-blur")
	wayland_generate_protocol("ext-background-effect-v1")

	list(APPEND LIBS xkbcommon)

	# Common Wayland support files
	list(APPEND SRCS
		src/lib/wayland/globals.cpp
		src/lib/wayland/display.cpp
		src/lib/wayland/registry.cpp
		src/lib/wayland/seat.cpp
		src/lib/wayland/virtual-keyboard.hpp
		src/lib/wayland/virtual-keyboard.cpp

		src/services/clipboard/data-control/data-control-clipboard-server.hpp
		src/services/clipboard/data-control/data-control-clipboard-server.cpp
	)

	wayland_generate_protocol("wlr-data-control-unstable-v1")
endif()
	

qt_add_executable(${TARGET} ${SRCS})

set(VICINAE_QML_FILES
	src/qml/qml/LauncherWindow.qml
	src/qml/qml/SearchBar.qml
	src/qml/qml/GenericListView.qml
	src/qml/qml/RootSearchList.qml
	src/qml/qml/ListItemDelegate.qml
	src/qml/qml/ListAccessory.qml
	src/qml/qml/ListAccessoryRow.qml
	src/qml/qml/SelectableDelegate.qml
	src/qml/qml/SectionHeader.qml
	src/qml/qml/Footer.qml
	src/qml/qml/FooterButton.qml
	src/qml/qml/FooterNavStatus.qml
	src/qml/qml/FooterToast.qml
	src/qml/qml/ShortcutBadge.qml
	src/qml/qml/CommandListView.qml
	src/qml/qml/CalculatorResultDelegate.qml
	src/qml/qml/ThemeListView.qml
	src/qml/qml/ThemeItemDelegate.qml
	src/qml/qml/GenericGridView.qml
	src/qml/qml/EmptyView.qml
	src/qml/qml/EmojiGridView.qml
	src/qml/qml/ViciImage.qml
	src/qml/qml/ViciScrollBar.qml
	src/qml/qml/ViciToolTip.qml
	src/qml/qml/ExtensionView.qml
	src/qml/qml/MarkdownDetailView.qml
	src/qml/qml/ExtensionGridView.qml
	src/qml/qml/ActionPanelPopover.qml
	src/qml/qml/ActionListPanel.qml
	src/qml/qml/ActionItemDelegate.qml
	src/qml/qml/SearchableDropdown.qml
	src/qml/qml/ClipboardHistoryView.qml
	src/qml/qml/ClipboardFilterAccessory.qml
	src/qml/qml/DetailPanel.qml
	src/qml/qml/MetadataBar.qml
	src/qml/qml/AlertDialog.qml
	src/qml/qml/FormView.qml
	src/qml/qml/FormField.qml
	src/qml/qml/FormTextInput.qml
	src/qml/qml/FormTextArea.qml
	src/qml/qml/FormCheckbox.qml
	src/qml/qml/FormSeparator.qml
	src/qml/qml/SnippetFormView.qml
	src/qml/qml/AliasFormView.qml
	src/qml/qml/ShortcutFormView.qml
	src/qml/qml/FormCompletedInput.qml
	src/qml/qml/CreateExtensionFormView.qml
	src/qml/qml/ArgCompleter.qml
	src/qml/qml/DetailListView.qml
	src/qml/qml/StoreListingView.qml
	src/qml/qml/AlertBanner.qml
	src/qml/qml/StoreDetailView.qml
	src/qml/qml/StoreIntroView.qml
	src/qml/qml/markdown/MarkdownView.qml
	src/qml/qml/markdown/MarkdownText.qml
	src/qml/qml/markdown/MdHeading.qml
	src/qml/qml/markdown/MdParagraph.qml
	src/qml/qml/markdown/MdCodeBlock.qml
	src/qml/qml/markdown/MdList.qml
	src/qml/qml/markdown/MdTable.qml
	src/qml/qml/markdown/MdImage.qml
	src/qml/qml/markdown/MdHorizontalRule.qml
	src/qml/qml/markdown/MdHtmlBlock.qml
	src/qml/qml/markdown/MdBlockquote.qml
	src/qml/qml/markdown/MdCallout.qml
	src/qml/qml/BrowseFontsView.qml
	src/qml/qml/ExtensionFormView.qml
	src/qml/qml/FormFilePicker.qml
	src/qml/qml/FormDateInput.qml
	src/qml/qml/FormLinkAccessory.qml
	src/qml/qml/ExtensionDropdownAccessory.qml
	src/qml/qml/EditKeywordsFormView.qml
	src/qml/qml/HorizontalLoadingBar.qml
	src/qml/qml/SearchFilesView.qml
	src/qml/qml/TextViewer.qml
	src/qml/qml/FilePreview.qml
	src/qml/qml/DMenuView.qml
	src/qml/qml/ScriptOutputText.qml
	src/qml/qml/ScriptExecutorView.qml
	src/qml/qml/SettingsWindow.qml
	src/qml/qml/SettingsNavBar.qml
	src/qml/qml/SettingsLinkButton.qml
	src/qml/qml/AboutSettingsTab.qml
	src/qml/qml/GeneralSettingsTab.qml
	src/qml/qml/ExtensionSettingsTab.qml
	src/qml/qml/KeybindSettingsTab.qml
	src/qml/qml/ShortcutRecorderField.qml
	src/qml/qml/PreferenceFormView.qml
	src/qml/qml/SettingsPreferenceForm.qml
	src/qml/qml/MissingPreferenceView.qml
	src/qml/qml/OAuthOverlayView.qml
	src/qml/qml/HudWindow.qml
)

# Strip directory prefix so resources are qrc:/Vicinae/Foo.qml
foreach(qml_file IN LISTS VICINAE_QML_FILES)
	get_filename_component(alias ${qml_file} NAME)
	set_source_files_properties(${qml_file} PROPERTIES QT_RESOURCE_ALIAS ${alias})
endforeach()

qt_add_qml_module(${TARGET}
	URI Vicinae
	VERSION 1.0
	RESOURCE_PREFIX /
	QML_FILES ${VICINAE_QML_FILES}
	SOURCES
		src/qml/source-blend-rect.hpp
		src/qml/animated-image.hpp
		src/qml/markdown/markdown-model.hpp
		src/qml/markdown/text-selection-controller.hpp
	DEPENDENCIES QtQuick
	NO_PLUGIN
)

if(Qt6_FOUND)
	qt_import_plugins(${TARGET}
        INCLUDE Qt6::QWaylandIntegrationPlugin
    )
endif()

target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/qml ${CMAKE_CURRENT_SOURCE_DIR}/src/qml/markdown)
target_link_libraries(${TARGET} PRIVATE ${LIBS})
target_compile_features(${TARGET} PUBLIC cxx_std_26)

if (USE_PRECOMPILED_HEADERS)
	message(STATUS "Precompiled headers are enabled")
	target_precompile_headers(${TARGET} PUBLIC "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h>")
	set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)
else()
	message(STATUS "Precompiled headers are disabled")
endif()

make_directory(${CMAKE_CURRENT_BINARY_DIR}/proto)

set(PROTO_IMPORT_DIRS ${PROTO_DIR} ${COMMON_PROTO_DIR})

if (NOT USE_SYSTEM_PROTOBUF)
	list(APPEND PROTO_IMPORT_DIRS ${protobuf_SOURCE_DIR}/src)
endif()

protobuf_generate(
	TARGET ${TARGET}
	PROTOS ${PROTO_FILES}
	IMPORT_DIRS ${PROTO_IMPORT_DIRS}
	PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto
)

foreach(PROTO_FILE ${PROTO_FILES})
	get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
	set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/proto/${PROTO_NAME}.pb.cc PROPERTIES SKIP_LINTING ON)
endforeach()

install(TARGETS ${TARGET}
	RUNTIME DESTINATION ${VICINAE_LIBEXEC_DIR}
)
