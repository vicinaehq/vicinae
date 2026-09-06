set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(LIBS)
set(TARGET vicinae-server)

find_package(Qt6 REQUIRED COMPONENTS Core Network Svg Concurrent Quick Qml GuiPrivate QuickDialogs2 QuickControls2 ShaderTools LinguistTools)

if (USE_SYSTEM_KF6)
	find_package(KF6SyntaxHighlighting REQUIRED)
endif()

if (USE_SYSTEM_NUMEN)
	find_package(numen REQUIRED)
else()
	include(FetchContent)

	FetchContent_Declare(numen
	  GIT_REPOSITORY https://github.com/vicinaehq/numen
	  GIT_TAG        v0.6.1
	)

	FetchContent_MakeAvailable(numen)
endif()

list(APPEND LIBS
	Qt6::Network Qt6::Svg Qt6::Concurrent Qt6::Quick Qt6::Qml
	Qt6::GuiPrivate # for deeper integration with wayland protocols, we need wl_surface
	Qt6::QuickDialogs2 Qt6::QuickControls2
	${CMARK_LIBRARY} ${CMARK_EXT_LIBRARY}
	zip
	qt6keychain
	pugixml
	glaze::glaze
	vicinae::scriptcommand
	vicinae::glyph
	vicinae::common
	vicinae::fuzzy
	vicinae::crypto
	sqlcipher
	numen::numen
)

list(APPEND LIBS KF6::SyntaxHighlighting)

if (NOT WIN32)
	list(APPEND LIBS vicinae::xdgpp)
endif()

if (UNIX AND NOT APPLE)
	find_package(Qt6 REQUIRED COMPONENTS DBus WaylandClient)
	list(APPEND LIBS wayland-client Qt6::DBus Qt6::WaylandClient)
endif()

if (APPLE AND BUNDLE_SOULVER_CORE)
	list(APPEND LIBS vicinae::soulver)
endif()


set(ASSET_PATH ${CMAKE_CURRENT_SOURCE_DIR}/assets)
set(EXTRA_PATH ${CMAKE_SOURCE_DIR}/extra)

if (UNIX AND NOT APPLE)
	set(DEFAULT_FONT_RESOURCES
"  	<file alias=\"Outfit-Variable.ttf\">${ASSET_PATH}/fonts/Outfit-Variable.ttf</file>
  	<file alias=\"GeistMono-Variable.ttf\">${ASSET_PATH}/fonts/GeistMono-Variable.ttf</file>
")
endif()

configure_file(resources-core.qrc.in resources-core.qrc)
configure_file(resources-extensions.qrc.in resources-extensions.qrc)

if (CMAKE_BUILD_TYPE MATCHES "Debug" AND NOT WIN32)
	add_compile_definitions(LOCAL_THEME_DIR="${THEME_DIR}")
endif()

if (WAYLAND_LAYER_SHELL)
	message(STATUS "Compiling with wayland layer shell support (for wlroots-like compositors)")
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
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/internal
	${CMAKE_CURRENT_SOURCE_DIR}/src/utils
	${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_BINARY_DIR}  # For generated headers
)

set(SRCS
	src/main.cpp

	${CMAKE_SOURCE_DIR}/vendor/fuzzy-trigram/register.c

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


	src/font-service.hpp
	src/font-service.cpp

	src/internal/template-engine/template-engine.cpp
	src/internal/glaze-qt.cpp
	src/internal/http-client.hpp

	src/favicon/favicon-service.hpp
	src/favicon/favicon-service.cpp
	src/favicon/favicon-request.hpp
	src/favicon/favicon-request.cpp
	src/favicon/google-favicon-request.hpp
	src/favicon/google-favicon-request.cpp
	src/favicon/twenty-favicon-request.hpp
	src/favicon/twenty-favicon-request.cpp
	src/favicon/dummy-favicon-request.hpp
	src/favicon/dummy-favicon-request.cpp
	src/favicon/cached-favicon-request.hpp
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
	src/extension/node-runtime/node-runtime.hpp
	src/extension/node-runtime/node-runtime.cpp

	# Bookmark - Start
	src/services/shortcut/shortcut-service.hpp
	src/services/shortcut/shortcut-service.cpp
	src/services/shortcut/shortcut-db.hpp
	src/services/shortcut/shortcut-db.cpp
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

	src/services/clipboard/clipboard-server.hpp
	src/services/clipboard/clipboard-content.hpp
	src/services/clipboard/clipboard-mime.hpp
	src/services/clipboard/clipboard-mime.cpp
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

	src/services/local-storage/local-storage-service.hpp
	src/services/local-storage/local-storage.cpp

	src/services/local-storage/scoped-local-storage.cpp

	src/settings-controller/settings-controller.hpp
	src/settings-controller/settings-controller.cpp

	src/services/toast/toast-service.hpp

	# begin app-database
	src/services/app-service/abstract-app-db.hpp
	src/services/app-service/dummy-app-database.hpp

	# end app-database

	src/ui/action-panel/action.hpp
	src/ui/action-panel/action.cpp
	src/ui/action-panel/action-panel-state.hpp
	src/ui/action-panel/action-panel-view.hpp
	src/ui/action-panel/action-list-view.hpp
	src/ui/action-panel/action-list-view.cpp
	src/ui/action-panel/shortcut-recorder-panel-view.hpp
	src/ui/action-panel/shortcut-recorder-panel-view.cpp

	
	src/extension/extension-command.hpp

	src/extension/extension-action-panel-builder.hpp
	src/extension/extension-action-panel-builder.cpp
	src/extension/extension-action-list-view.hpp
	src/extension/extension-action-list-view.cpp

	src/extension/extension.hpp
	src/extension/extension.cpp

	src/extension/extension-command.hpp
	src/extension/extension-command.cpp



	src/ui/omni-painter/omni-painter.hpp
	src/ui/omni-painter/omni-painter.cpp

	src/ui/image/url.cpp
	src/ui/image/image-renderer.hpp
	src/ui/image/image-renderer.cpp
	src/ui/image/image-stream.hpp
	src/ui/image/image-stream.cpp

	src/command-database.hpp
	src/command-database.cpp

		src/ui/image/url.hpp



	src/builtin_icon.cpp


	src/extend/accessory-model.cpp
	src/extend/model-deser.cpp
	src/extend/form-model.hpp

	src/image-fetcher.hpp


	src/command.hpp
	

	src/vicinae.hpp
	src/vicinae.cpp


	${EXTRA_PATH}/extension-boilerplate/boilerplate.qrc

	./icons/icons.qrc

	./database/vicinae/migrations.qrc
	./database/clipboard/migrations.qrc


	src/internal/keyboard/keybind-manager.hpp
	src/internal/keyboard/keybind-manager.cpp
	src/internal/keyboard/keyboard.cpp
	src/internal/keyboard/layout-resolver.hpp
	src/internal/keyboard/layout-resolver.cpp

	src/theme/theme-file.hpp
	src/theme/theme-file.cpp
	src/theme/theme-parser.cpp
	src/theme/theme-db.hpp
	src/theme/theme-db.cpp

	src/extensions/clipboard/clipboard-history-command.cpp
	src/extensions/clipboard/history/clipboard-history-controller.hpp
	src/extensions/clipboard/history/clipboard-history-controller.cpp
	src/extensions/browser/browser-extension.cpp

	src/extensions/wm/wm-extension.hpp
	src/extensions/wm/wm-extension.cpp

	src/extensions/clipboard/clipboard-extension.hpp
	src/extensions/clipboard/clipboard-extension.cpp

	src/extensions/snippet/snippet-extension.cpp

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

	src/server.cpp


	src/utils/utils.cpp
	src/utils/capabilities.hpp
	src/utils/capabilities.cpp

	src/services/file-chooser/abstract-file-chooser.hpp
	src/services/file-chooser/file-chooser.hpp
	src/services/file-chooser/file-chooser.cpp
	src/services/file-chooser/file-chooser-service.hpp
	src/services/file-chooser/file-chooser-service.cpp
	src/services/root-item-manager/root-item-manager.hpp
	src/services/root-item-manager/root-item-manager.cpp
	src/services/root-item-manager/search-history.cpp
	src/services/root-item-manager/visit-tracker.cpp
	
	src/services/app-service/app-service.hpp
	src/services/app-service/app-service.cpp

	src/services/glyph-service/glyph-service.hpp
	src/services/glyph-service/glyph-service.cpp



	src/services/calculator-service/calculator-service.hpp
	src/services/calculator-service/calculator-service.cpp

	src/services/calculator-service/numen/numen-currency-provider.hpp
	src/services/calculator-service/numen/numen-currency-provider.cpp
	src/services/calculator-service/numen/numen-calculator-backend.hpp
	src/services/calculator-service/numen/numen-calculator-backend.cpp

	src/services/files-service/abstract-file-indexer.hpp
	src/services/files-service/dummy-file-indexer.hpp
	src/services/files-service/file-service.hpp
	src/services/files-service/file-service.cpp

	src/services/extension-registry/extension-registry.hpp
	src/services/extension-registry/extension-registry.cpp
	src/services/extension-registry/extension-manifest.hpp
	src/services/extension-registry/extension-manifest.cpp

	src/services/oauth/oauth-token-store.hpp
	src/services/oauth/oauth-token-store.cpp

	src/extension/views/oauth-overlay-host.hpp
	src/extension/views/oauth-overlay-host.cpp



	src/utils/migration-manager/migration-manager.hpp
	src/utils/migration-manager/migration-manager.cpp

	src/navigation-controller.hpp
	src/navigation-controller.cpp

	src/ui/views/base-view.cpp

	src/command-controller.hpp
	src/command-controller.cpp

	src/ipc-command-server.hpp
	src/ipc-command-server.cpp
	src/ipc-command-handler.hpp
	src/ipc-command-handler.cpp

	src/extension-interval-scheduler.hpp
	src/extension-interval-scheduler.cpp

	src/log/message-handler.cpp

	src/extension/extension-command-runtime.cpp
	src/extension/services/ui-service.hpp
	src/extension/services/wallpaper-service.hpp

	src/extensions/vicinae/vicinae-extension.cpp

	src/qml/search-files-model.hpp
	src/qml/search-files-model.cpp
	src/qml/search-files-view-host.hpp
	src/qml/search-files-view-host.cpp

	src/services/raycast/raycast-store.cpp
	src/services/extension-store/vicinae-store.cpp

	src/overlay-controller/overlay-controller.hpp


	src/internal/zip/unzip.cpp
	src/internal/data-uri/data-uri.cpp
	src/internal/pid-file/pid-file.cpp

	src/ui/alert/alert.hpp
	src/ui/alert/alert.cpp

	src/ui/dialog/dialog.hpp
	src/ui/dialog/dialog.cpp

	src/internal/os-release.cpp

	src/services/window-manager/window-manager.cpp

	src/services/wallpaper/wallpaper-manager.cpp

	src/services/extension-boilerplate-generator/extension-boilerplate-generator.cpp

	src/services/window-manager/abstract-window-manager.hpp
	src/services/paste/abstract-paste-service.hpp
	src/services/paste/dummy-paste-service.hpp
	src/services/paste/paste-service.cpp
	src/services/selection/abstract-selection-service.hpp
	src/services/selection/dummy-selection-service.hpp
	src/services/browser-extension-service.hpp

	src/internal/program-db/program-db.hpp
	src/internal/program-db/program-db.cpp

	src/internal/db/db-encryption.hpp
	src/internal/db/db-encryption.cpp
	src/internal/db/database-key.hpp
	src/internal/db/database-key.cpp

	src/services/power-manager/dummy-power-manager.hpp

	src/services/tray-host/tray-item.hpp
	src/services/tray-host/tray-item.cpp
	src/services/tray-host/abstract-tray-host.hpp
	src/services/tray-host/dummy-tray-host.hpp
	src/services/tray-host/tray-host.hpp
	src/services/tray-host/tray-host.cpp
	src/qml/search-tray-view-host.hpp
	src/extensions/vicinae/search-tray-command.hpp

	src/services/desktop-notification/dummy-desktop-notification-client.hpp

	src/services/audio-control/pactl/pactl-audio-control.cpp
	src/services/audio-control/dummy-audio-control.hpp

	src/services/media-control/abstract-media-control.hpp
	src/services/media-control/abstract-media-control.cpp
	src/services/media-control/dummy-media-control.hpp

	src/config/config.hpp
	src/config/config.cpp

	src/qml/local-storage-model.hpp
	src/qml/local-storage-model.cpp
	src/qml/local-storage-view-host.hpp
	src/qml/local-storage-view-host.cpp

	src/ui/views/dmenu-model.hpp
	src/ui/views/dmenu-model.cpp
	src/ui/views/dmenu-view-host.hpp
	src/ui/views/dmenu-view-host.cpp

	src/services/news/news-service.hpp
	src/services/news/news-service.cpp

	src/services/telemetry/telemetry-service.hpp
	src/services/telemetry/telemetry-service.cpp

	src/services/update/semver.hpp
	src/services/update/github-release.hpp
	src/services/update/abstract-update-installer.hpp
	src/services/update/null-update-installer.hpp
	src/services/update/update-service.hpp
	src/services/update/update-service.cpp

	src/services/snippet/snippet-service.hpp
	src/services/snippet/abstract-snippet-server.hpp
	src/services/snippet/null-snippet-server.hpp
	src/services/snippet/snippet-db.cpp

	# QML frontend
	src/ui/bridges/keybind-bridge.hpp
	src/ui/windows/launcher-window.hpp
	src/ui/windows/launcher-window.cpp
	src/qml/root-view-host.hpp
	src/qml/root-view-host.cpp
	src/qml/root-search-model.hpp
	src/qml/root-search-model.cpp
	src/qml/root-search-sources.hpp
	src/qml/root-search-sources.cpp
	src/ui/views/drag-utils.hpp
	src/ui/views/drag-utils.cpp
	src/ui/views/section-source.hpp
	src/ui/views/section-list-model.hpp
	src/ui/views/section-list-model.cpp
	src/ui/views/fuzzy-section.hpp
	src/ui/views/list-view-host.hpp
	src/ui/action-panel/action-panel-model.hpp
	src/ui/action-panel/action-panel-model.cpp
	src/ui/quick/completion-model.hpp
	src/ui/quick/completion-model.cpp
	src/ui/action-panel/action-panel-controller.hpp
	src/ui/action-panel/action-panel-controller.cpp
	src/ui/alert/alert-model.hpp
	src/ui/alert/alert-model.cpp
	src/ui/views/bridge-view.hpp
	src/qml/calc-history-model.hpp
	src/qml/calc-history-model.cpp
	src/qml/calc-history-view-host.hpp
	src/qml/calc-history-view-host.cpp
	src/qml/theme-list-model.hpp
	src/qml/theme-list-model.cpp
	src/qml/theme-view-host.hpp
	src/qml/theme-view-host.cpp
	src/ui/views/grid-source.hpp
	src/ui/views/section-grid-model.hpp
	src/ui/views/section-grid-model.cpp
	src/qml/emoji-grid-model.hpp
	src/qml/emoji-grid-model.cpp
	src/qml/emoji-grid-view-host.hpp
	src/ui/views/view-utils.cpp
	src/extension/views/extension-view-host.hpp
	src/extension/views/extension-view-host.cpp
	src/extension/views/extension-error-view-host.hpp
	src/extension/views/extension-error-view-host.cpp
	src/ui/views/empty-view-host.hpp
	src/ui/views/empty-view-host.cpp
	src/extension/views/missing-preference-view-host.hpp
	src/extension/views/missing-preference-view-host.cpp
	src/extension/views/extension-form-model.hpp
	src/extension/views/extension-form-model.cpp
	src/qml/clipboard-history-model.hpp
	src/qml/clipboard-history-model.cpp
	src/qml/clipboard-history-view-host.hpp
	src/qml/clipboard-history-view-host.cpp
	src/qml/snippet-form-view-host.hpp
	src/qml/snippet-form-view-host.cpp
	src/qml/alias-form-view-host.hpp
	src/qml/alias-form-view-host.cpp
	src/ui/views/app-selector-model.hpp
	src/ui/views/app-selector-model.cpp
	src/qml/shortcut-form-view-host.hpp
	src/qml/shortcut-form-view-host.cpp
	src/extension/views/extension-list-model.hpp
	src/extension/views/extension-list-model.cpp
	src/extension/views/extension-grid-model.hpp
	src/extension/views/extension-grid-model.cpp
	src/ui/quick/source-blend-rect.hpp
	src/ui/quick/source-blend-rect.cpp
	src/ui/image/image-url.hpp
	src/ui/image/image-url.cpp
	src/ui/image/image-source.hpp
	src/ui/image/image-source.cpp
	src/ui/quick/vici-image-item.hpp
	src/ui/quick/vici-image-item.cpp
	src/ui/bridges/config-bridge.hpp
	src/ui/bridges/theme-bridge.hpp
	src/ui/bridges/keyboard-bridge.hpp
	src/ui/bridges/global-shortcut-bridge.hpp
	src/ui/bridges/platform-bridge.hpp
	src/ui/bridges/style-bridge.hpp
	src/ui/settings/shortcut-conflict.hpp
	src/ui/settings/shortcut-conflict.cpp
	src/qml/create-extension-view-host.hpp
	src/qml/create-extension-view-host.cpp
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
	src/qml/switch-workspaces-model.hpp
	src/qml/switch-workspaces-view-host.hpp
	src/qml/switch-workspaces-view-host.cpp
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
	src/ui/quick/markdown-model.hpp
	src/ui/quick/markdown-model.cpp
	src/ui/quick/text-selection-controller.hpp
	src/ui/quick/text-selection-controller.cpp
	src/qml/store-intro-view-host.hpp
	src/qml/store-intro-view-host.cpp
	src/qml/font-browser-view-host.hpp
	src/qml/font-grid-model.hpp
	src/qml/font-grid-model.cpp
	src/qml/font-demo-view-host.hpp
	src/qml/font-demo-view-host.cpp
	src/qml/edit-keywords-view-host.hpp
	src/qml/edit-keywords-view-host.cpp
	src/ui/windows/hud-bridge.hpp
	src/ui/windows/hud-bridge.cpp

	src/ui/windows/settings-window.hpp
	src/ui/windows/settings-window.cpp
	src/ui/windows/onboarding-window.hpp
	src/ui/windows/onboarding-window.cpp
	src/ui/settings/general-settings-model.hpp
	src/ui/settings/general-settings-model.cpp
	src/ui/settings/keybind-settings-model.hpp
	src/ui/settings/keybind-settings-model.cpp
	src/ui/settings/settings-sidebar-model.hpp
	src/ui/settings/settings-sidebar-model.cpp
	src/ui/settings/extension-settings-model.hpp
	src/ui/settings/extension-settings-model.cpp
	src/ui/settings/preference-form-model.hpp
	src/ui/settings/preference-form-model.cpp
	src/ui/settings/provider-command-model.hpp
	src/ui/settings/provider-command-model.cpp

	src/extensions/internal/markdown-showcase-command.hpp

)

if ((UNIX AND NOT APPLE) OR (APPLE AND BUNDLE_SOULVER_CORE))
	list(APPEND SRCS
		src/services/calculator-service/soulver-core/soulver-core.cpp
	)
endif()

if (APPLE AND BUNDLE_SOULVER_CORE)
	add_compile_definitions(BUNDLE_SOULVER_CORE)
endif()

if (APPLE)
	list(APPEND SRCS
		src/ui/image/mac-file-icon-loader.hpp
		src/ui/image/mac-file-icon-loader.mm
		src/services/app-service/macos/mac-app.hpp
		src/services/app-service/macos/mac-app.mm
		src/services/app-service/macos/mac-app-database.hpp
		src/services/app-service/macos/mac-app-database.mm
		src/services/app-runtime/macos/mac-app-runtime.hpp
		src/services/app-runtime/macos/mac-app-runtime.mm
		src/services/clipboard/macos/macos-clipboard-server.hpp
		src/services/clipboard/macos/macos-clipboard-server.mm
		src/root-search/macos-settings/macos-settings-root-provider.hpp
		src/root-search/macos-settings/macos-settings-root-provider.mm
		src/services/global-shortcuts/macos-global-shortcut-backend.hpp
		src/services/global-shortcuts/macos-global-shortcut-backend.cpp
		src/internal/keyboard/keyboard-macos.hpp
		src/internal/keyboard/keyboard-macos.cpp
		src/services/files-service/macos/spotlight-file-indexer.hpp
		src/services/files-service/macos/spotlight-file-indexer.mm
		src/services/tray/macos/tray-service-macos.hpp
		src/services/tray/macos/tray-service-macos.mm
		src/services/paste/macos-paste-service.hpp
		src/services/paste/macos-paste-service.mm
		src/services/selection/macos-selection-service.hpp
		src/services/selection/macos-selection-service.mm
		src/services/power-manager/macos/macos-power-manager.hpp
		src/services/power-manager/macos/macos-power-manager.mm
		src/services/audio-control/macos/coreaudio-audio-control.hpp
		src/services/audio-control/macos/coreaudio-audio-control.cpp
		src/services/window-manager/macos/macos-window.hpp
		src/services/window-manager/macos/macos-window.mm
		src/services/window-manager/macos/macos-window-manager.hpp
		src/services/window-manager/macos/macos-window-manager.mm
		src/services/wallpaper/macos/mac-wallpaper-backend.hpp
		src/services/wallpaper/macos/mac-wallpaper-backend.mm
		src/services/snippet/macos-snippet-server.hpp
		src/services/snippet/macos-snippet-server.mm
		src/services/update/macos-update-installer.hpp
		src/services/update/macos-update-installer.mm
		src/services/permissions/macos-permission-service.hpp
		src/services/permissions/macos-permission-service.mm
		src/services/autostart/macos-login-item.hpp
		src/services/autostart/macos-login-item.mm
		src/services/desktop-notification/macos/macos-notification-client.hpp
		src/services/desktop-notification/macos/macos-notification-client.mm
		src/services/file-chooser/macos/mac-file-chooser.hpp
		src/services/file-chooser/macos/mac-file-chooser.mm
		src/services/menu-bar/macos-menu-bar.hpp
		src/services/menu-bar/macos-menu-bar.mm
		src/qml/menu-bar-search-view-host.hpp
		src/extensions/vicinae/search-menu-bar-command.hpp
	)
	set_source_files_properties(
		src/ui/image/mac-file-icon-loader.mm
		src/services/wallpaper/macos/mac-wallpaper-backend.mm
		src/services/app-service/macos/mac-app.mm
		src/services/app-service/macos/mac-app-database.mm
		src/services/app-runtime/macos/mac-app-runtime.mm
		src/services/clipboard/macos/macos-clipboard-server.mm
		src/root-search/macos-settings/macos-settings-root-provider.mm
		src/services/files-service/macos/spotlight-file-indexer.mm
		src/services/tray/macos/tray-service-macos.mm
		src/services/paste/macos-paste-service.mm
		src/services/selection/macos-selection-service.mm
		src/services/power-manager/macos/macos-power-manager.mm
		src/services/window-manager/macos/macos-window.mm
		src/services/window-manager/macos/macos-window-manager.mm
		src/services/snippet/macos-snippet-server.mm
		src/services/update/macos-update-installer.mm
		src/services/permissions/macos-permission-service.mm
		src/services/autostart/macos-login-item.mm
		src/services/desktop-notification/macos/macos-notification-client.mm
		src/services/file-chooser/macos/mac-file-chooser.mm
		src/services/menu-bar/macos-menu-bar.mm
		PROPERTIES COMPILE_OPTIONS "-fobjc-arc")
	list(APPEND LIBS "-framework ServiceManagement" "-framework UserNotifications")

	if (AUTO_ENABLE_AUTOSTART)
		add_compile_definitions(AUTO_ENABLE_AUTOSTART)
	endif()
elseif (WIN32)
	list(APPEND SRCS
		src/ui/image/win-file-icon-loader.hpp
		src/ui/image/win-file-icon-loader.cpp
		src/services/app-service/windows/win-app.hpp
		src/services/app-service/windows/win-app-database.hpp
		src/services/app-service/windows/win-app-database.cpp
		src/services/global-shortcuts/windows-global-shortcut-backend.hpp
		src/services/global-shortcuts/windows-global-shortcut-backend.cpp
		src/internal/keyboard/windows-layout-resolver.hpp
		src/internal/keyboard/windows-layout-resolver.cpp
		src/services/clipboard/windows/windows-clipboard-server.hpp
		src/services/clipboard/windows/windows-clipboard-server.cpp
		src/services/clipboard/windows/windows-image-mime.hpp
		src/services/clipboard/windows/windows-image-mime.cpp
		src/services/tray/windows/tray-service-windows.hpp
		src/services/tray/windows/tray-service-windows.cpp
		src/services/paste/windows-paste-service.hpp
		src/services/paste/windows-paste-service.cpp
		src/services/selection/windows-selection-service.hpp
		src/services/selection/windows-selection-service.cpp
		src/services/snippet/windows-snippet-server.hpp
		src/services/snippet/windows-snippet-server.cpp
		src/services/files-service/windows/win-file-candidates.hpp
		src/services/files-service/windows/win-file-candidates.cpp
		src/services/files-service/windows/wsearch/win-file-indexer.hpp
		src/services/files-service/windows/wsearch/win-file-indexer.cpp
		src/services/files-service/windows/everything/everything-client.hpp
		src/services/files-service/windows/everything/everything-window-client.hpp
		src/services/files-service/windows/everything/everything-window-client.cpp
		src/services/files-service/windows/everything/everything-pipe-client.hpp
		src/services/files-service/windows/everything/everything-pipe-client.cpp
		src/services/files-service/windows/everything/everything-file-indexer.hpp
		src/services/files-service/windows/everything/everything-file-indexer.cpp
		src/services/files-service/windows/win-file-indexer-selector.hpp
		src/services/files-service/windows/win-file-indexer-selector.cpp
		src/services/window-manager/windows/windows-window.hpp
		src/services/window-manager/windows/virtual-desktops.hpp
		src/services/window-manager/windows/virtual-desktops.cpp
		src/services/window-manager/windows/windows-window-manager.hpp
		src/services/window-manager/windows/windows-window-manager.cpp
		src/services/app-runtime/windows/win-app-runtime.hpp
		src/services/app-runtime/windows/win-app-runtime.cpp
		src/root-search/windows-settings/windows-settings-root-provider.hpp
		src/root-search/windows-settings/windows-settings-root-provider.cpp
		src/root-search/control-panel/control-panel-root-provider.hpp
		src/root-search/control-panel/control-panel-root-provider.cpp
		src/services/url-scheme/win-url-scheme-registrar.hpp
		src/services/url-scheme/win-url-scheme-registrar.cpp
		src/services/desktop-notification/windows/windows-notification-client.hpp
		src/services/desktop-notification/windows/windows-notification-client.cpp
		src/services/wallpaper/windows/windows-wallpaper-backend.hpp
		src/services/wallpaper/windows/windows-wallpaper-backend.cpp
		src/services/audio-control/windows/wasapi-audio-control.hpp
		src/services/audio-control/windows/wasapi-audio-control.cpp
		src/services/media-control/windows/windows-media-control.hpp
		src/services/media-control/windows/windows-media-control.cpp
		src/services/power-manager/windows/windows-power-manager.hpp
		src/services/power-manager/windows/windows-power-manager.cpp
		src/utils/win-chassis.hpp
		src/utils/win-chassis.cpp
	)
	list(APPEND LIBS shell32 ole32 shlwapi user32 gdi32 advapi32 version windowsapp dwmapi comctl32 uuid uiautomationcore powrprof everything-sdk3)
	add_compile_definitions(VICINAE_BUNDLE_ID="${VICINAE_BUNDLE_ID}")
else()
	list(APPEND SRCS
		src/services/app-runtime/linux/linux-app-runtime.hpp
		src/services/app-runtime/linux/linux-app-runtime.cpp
	)
endif()

if (ENABLE_ONBOARDING)
	add_compile_definitions(ENABLE_ONBOARDING)
endif()

list(APPEND SRCS
	src/services/app-runtime/abstract-app-runtime.hpp
	src/services/app-runtime/app-runtime.hpp
	src/services/app-runtime/app-runtime.cpp
)

list(APPEND SRCS
	src/services/tray/tray-service.hpp
	src/services/tray/tray-service.cpp
)

list(APPEND SRCS
	src/services/global-shortcuts/abstract-global-shortcut-backend.hpp
	src/services/global-shortcuts/dummy-global-shortcut-backend.hpp
	src/services/global-shortcuts/global-shortcut-backend-factory.hpp
	src/services/global-shortcuts/global-shortcut-backend-factory.cpp
	src/services/global-shortcuts/global-shortcut-service.hpp
	src/services/global-shortcuts/global-shortcut-service.cpp
)

if (NOT WIN32)
	list(APPEND LIBS qalculate)
	list(APPEND SRCS
		src/services/calculator-service/qalculate/qalculate-backend.hpp
		src/services/calculator-service/qalculate/qalculate-backend.cpp
	)
endif()

list(APPEND SRCS ${CMAKE_CURRENT_BINARY_DIR}/resources-core.qrc)
if (TYPESCRIPT_EXTENSIONS)
    list(APPEND SRCS ${CMAKE_CURRENT_BINARY_DIR}/resources-extensions.qrc)
endif()

if (AUTO_INSTALL_BROWSER_MANIFESTS)
	add_compile_definitions(
		AUTO_INSTALL_BROWSER_MANIFESTS
		NATIVE_MESSAGING_HOST="${NATIVE_MESSAGING_HOST}"
		CHROME_EXTENSION_ID="${CHROME_EXTENSION_ID}"
		FIREFOX_EXTENSION_ID="${FIREFOX_EXTENSION_ID}"
	)
	list(APPEND SRCS
		src/services/browser-extension/native-host-installer.hpp
		src/services/browser-extension/native-host-installer.cpp
	)
endif()

if (UNIX AND NOT APPLE)
	list(APPEND SRCS
		src/services/clipboard/gnome/gnome-clipboard-server.hpp
		src/services/clipboard/gnome/gnome-clipboard-server.cpp
		src/services/app-service/xdg/xdg-app-database.hpp
		src/services/app-service/xdg/xdg-app-database.cpp
		src/services/app-service/xdg/xdg-app.cpp
		src/root-search/kde-settings/kde-settings-root-provider.hpp
		src/root-search/kde-settings/kde-settings-root-provider.cpp
		src/services/file-chooser/xdp-file-chooser/xdp-file-chooser.hpp
		src/services/file-chooser/xdp-file-chooser/xdp-file-chooser.cpp
		src/services/power-manager/systemd/systemd-power-manager.cpp
		src/services/tray-host/sni/sni-types.hpp
		src/services/tray-host/sni/sni-types.cpp
		src/services/tray-host/sni/sni-tray-host.hpp
		src/services/tray-host/sni/sni-tray-host.cpp
		src/services/tray-host/sni/sni-watcher.hpp
		src/services/tray-host/sni/sni-watcher.cpp
		src/services/tray/linux/tray-service-linux.hpp
		src/services/tray/linux/tray-service-linux.cpp
		src/services/desktop-notification/freedesktop/freedesktop-notification-client.cpp

		src/services/media-control/mpris/mpris-media-control.hpp
		src/services/media-control/mpris/mpris-media-control.cpp
		src/services/media-control/mpris/mpris-metadata.hpp
		src/services/media-control/mpris/mpris-metadata.cpp

		src/services/paste/linux-paste-service.cpp
		src/services/selection/linux-selection-service.cpp

		src/services/input-server/linux-input-server.hpp
		src/services/input-server/linux-input-server.cpp
		src/services/snippet/linux-snippet-server.hpp
		src/services/snippet/linux-snippet-server.cpp

		src/services/global-shortcuts/xkb-keysym.hpp
		src/services/global-shortcuts/xkb-keysym.cpp

		src/services/global-shortcuts/x11-global-shortcut-backend.hpp
		src/services/global-shortcuts/x11-global-shortcut-backend.cpp

		src/internal/icon-theme-db/icon-theme-db.cpp

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
		src/services/window-manager/abstract-wayland-window-manager.cpp
		src/services/window-manager/wayland/wayland.cpp
		src/services/window-manager/kde/kde-window-manager.hpp
		src/services/window-manager/kde/kde-window-manager.cpp
		src/services/window-manager/x11/x11-window-manager.cpp
		src/services/window-manager/x11/x11-event-listener.cpp
		src/services/window-manager/x11/x11-window.cpp

		src/services/wallpaper/gnome/gnome-wallpaper-backend.cpp
		src/services/wallpaper/swww/swww-wallpaper-backend.cpp
		src/services/wallpaper/hyprpaper/hyprpaper-wallpaper-backend.cpp
		src/services/wallpaper/kde/kde-wallpaper-backend.cpp
		src/services/wallpaper/cinnamon/cinnamon-wallpaper-backend.cpp
		src/services/wallpaper/mate/mate-wallpaper-backend.cpp

	)

	# Add XCB (X11 C Bindings) library for X11 window manager support
	find_package(X11 REQUIRED)
	if (X11_FOUND)
		list(APPEND LIBS X11::xcb X11::xcb_keysyms X11::xcb_xkb)
		message(STATUS "XCB library found for X11 window manager support")
	endif()

    include("Wayland")

	# only needed to provide the xdg_* interface symbols referenced by the
	# generated wlr-layer-shell protocol code
	wayland_system_protocol(XDG_SHELL_XML "stable/xdg-shell/xdg-shell.xml")
	wayland_generate_protocol(${XDG_SHELL_XML})

	wayland_system_protocol(EXT_BACKGROUND_EFFECT_XML "staging/ext-background-effect/ext-background-effect-v1.xml")
	wayland_system_protocol(EXT_DATA_CONTROL_XML "staging/ext-data-control/ext-data-control-v1.xml")
	wayland_system_protocol(KEYBOARD_SHORTCUTS_INHIBIT_XML "unstable/keyboard-shortcuts-inhibit/keyboard-shortcuts-inhibit-unstable-v1.xml")
	wayland_system_protocol(XDG_ACTIVATION_XML "staging/xdg-activation/xdg-activation-v1.xml")

	list(APPEND LIBS xkbcommon xkbcommon-x11)

	# Common Wayland support files
	list(APPEND SRCS
		src/internal/wayland/output-tracker.cpp
		src/internal/wayland/layer-shell.cpp
		src/internal/wayland/xdg-activation.cpp
		src/services/window-material/ext-background-effect-v1-manager.cpp
		src/services/shortcut-inhibit/wayland-shortcut-inhibit-manager.cpp
		src/services/clipboard/data-control/data-control-clipboard-server.hpp
		src/services/clipboard/data-control/data-control-clipboard-server.cpp

		src/services/global-shortcuts/vicinae-hotkey-global-shortcut-backend.hpp
		src/services/global-shortcuts/vicinae-hotkey-global-shortcut-backend.cpp

		src/internal/keyboard/xkb-layout-resolver.hpp
		src/internal/keyboard/xkb-layout-resolver.cpp
		src/internal/keyboard/wayland-layout-resolver.hpp
		src/internal/keyboard/wayland-layout-resolver.cpp
		src/internal/keyboard/x11-layout-resolver.hpp
		src/internal/keyboard/x11-layout-resolver.cpp

		src/services/files-service/file-indexer/file-indexer.hpp
		src/services/files-service/file-indexer/file-indexer.cpp
	)

endif()

include("Figura")

figura_compile(
	SERVER
	PROTO "${FIGURA_DIR}/tsapi.fig"
	LANG glaze-qt
	NAMESPACE tsapi
	OUTPUT tsapi.hpp
)

figura_compile(
	CLIENT
	PROTO "${FIGURA_DIR}/manager.fig"
	LANG glaze-qt
	NAMESPACE manager
	OUTPUT manager.hpp
)

figura_compile(
	SERVER
	PROTO "${FIGURA_DIR}/ipc.fig"
	LANG glaze-qt
	NAMESPACE ipc_gen
	OUTPUT ipc-server.hpp
)

figura_compile(
	CLIENT
	PROTO "${FIGURA_DIR}/snippet.fig"
	LANG glaze-qt
	NAMESPACE snippet_gen
	OUTPUT snippet-client.hpp
)

if (UNIX AND NOT APPLE)
	figura_compile(
		CLIENT
		PROTO "${FIGURA_DIR}/file-indexer.fig"
		LANG glaze-qt
		NAMESPACE file_indexer_gen
		OUTPUT file-indexer-client.hpp
	)
endif()


qt_add_executable(${TARGET} ${SRCS})

if (UNIX AND NOT APPLE)
	qt6_generate_wayland_protocol_client_sources(${TARGET}
		PRIVATE_CODE
		FILES
			${EXT_BACKGROUND_EFFECT_XML}
			${EXT_DATA_CONTROL_XML}
			${KEYBOARD_SHORTCUTS_INHIBIT_XML}
			${XDG_ACTIVATION_XML}
			${CMAKE_SOURCE_DIR}/src/wayland-protocols/vicinae-hotkey-v1.xml
			${CMAKE_SOURCE_DIR}/src/wayland-protocols/wlr-foreign-toplevel-management-unstable-v1.xml
			${CMAKE_SOURCE_DIR}/src/wayland-protocols/wlr-layer-shell-unstable-v1.xml
	)
endif()

vicinae_enable_utf8(${TARGET})

if(APPLE)
	set(VICINAE_BUNDLE_EXECUTABLE "Vicinae")
	set(VICINAE_BUNDLE_SHORT_VERSION "${VICINAE_GIT_TAG}")
	string(REGEX REPLACE "^v" "" VICINAE_BUNDLE_SHORT_VERSION "${VICINAE_BUNDLE_SHORT_VERSION}")
	if(NOT VICINAE_BUNDLE_SHORT_VERSION MATCHES "^[0-9]+(\\.[0-9]+)*$")
		set(VICINAE_BUNDLE_SHORT_VERSION "0.0.0")
	endif()
	get_git_commit_count(VICINAE_BUNDLE_VERSION)
	configure_file(
		"${CMAKE_SOURCE_DIR}/extra/Info.plist.in"
		"${CMAKE_BINARY_DIR}/Info.plist"
		@ONLY
	)
endif()

set(VICINAE_QML_FILES
	src/ui/qml/launcher/LauncherWindow.qml
	src/ui/qml/controls/PopupMaterial.qml
	src/ui/qml/controls/ViciPopover.qml
	src/ui/qml/controls/PopoverBackground.qml
	src/ui/qml/launcher/SearchBar.qml
	src/ui/qml/views/GenericListView.qml
	src/ui/qml/controls/StatusBarInset.qml
	src/ui/qml/list/HoverResetOnModelChange.qml
	src/ui/qml/list/HoverResetOnShow.qml
	src/ui/qml/views/RootSearchList.qml
	src/ui/qml/list/ListItemDelegate.qml
	src/ui/qml/list/ListAccessory.qml
	src/ui/qml/list/ListAccessoryRow.qml
	src/ui/qml/list/SelectableDelegate.qml
	src/ui/qml/list/DraggableMouseArea.qml
	src/ui/qml/list/SectionHeader.qml
	src/ui/qml/launcher/Footer.qml
	src/ui/qml/launcher/FooterButton.qml
	src/ui/qml/launcher/FooterNavStatus.qml
	src/ui/qml/launcher/FooterToast.qml
	src/ui/qml/controls/ShortcutBadge.qml
	src/ui/qml/controls/TextBadge.qml
	src/ui/qml/views/CommandListView.qml
	src/ui/qml/views/CalcHistoryListView.qml
	src/ui/qml/list/CalculatorResultDelegate.qml
	src/ui/qml/views/ThemeListView.qml
	src/ui/qml/list/ThemeItemDelegate.qml
	src/ui/qml/views/GenericGridView.qml
	src/ui/qml/views/EmptyView.qml
	src/ui/qml/views/EmojiGridView.qml
	src/ui/qml/controls/ViciButton.qml
	src/ui/qml/controls/ViciScrollBar.qml
	src/ui/qml/controls/ViciDivider.qml
	src/ui/qml/controls/ViciToolTip.qml
	src/ui/qml/views/ExtensionView.qml
	src/ui/qml/views/MarkdownDetailView.qml
	src/ui/qml/views/ExtensionGridView.qml
	src/ui/qml/actions/ActionPanelPopover.qml
	src/ui/qml/actions/ActionListPanel.qml
	src/ui/qml/actions/ActionItemDelegate.qml
	src/ui/qml/controls/SearchableDropdown.qml
	src/ui/qml/views/ClipboardHistoryView.qml
	src/ui/qml/accessories/ClipboardFilterAccessory.qml
	src/ui/qml/accessories/EmojiCategoryFilterAccessory.qml
	src/ui/qml/detail/DetailPanel.qml
	src/ui/qml/detail/MetadataBar.qml
	src/ui/qml/launcher/AlertDialog.qml
	src/ui/qml/form/FormView.qml
	src/ui/qml/form/FormField.qml
	src/ui/qml/form/FormInputBackground.qml
	src/ui/qml/form/FormTextInput.qml
	src/ui/qml/form/FormTextArea.qml
	src/ui/qml/form/FormCheckbox.qml
	src/ui/qml/form/FormSeparator.qml
	src/ui/qml/views/SnippetFormView.qml
	src/ui/qml/views/AliasFormView.qml
	src/ui/qml/views/ShortcutFormView.qml
	src/ui/qml/form/FormCompletedInput.qml
	src/ui/qml/form/FormCompletedTextArea.qml
	src/ui/qml/form/PlaceholderCompleter.qml
	src/ui/qml/controls/CompletionPopup.qml
	src/ui/qml/views/CreateExtensionFormView.qml
	src/ui/qml/launcher/ArgCompleter.qml
	src/ui/qml/views/DetailListView.qml
	src/ui/qml/views/StoreListingView.qml
	src/ui/qml/launcher/AlertBanner.qml
	src/ui/qml/views/StoreDetailView.qml
	src/ui/qml/views/StoreIntroView.qml
	src/ui/qml/detail/ImageViewer.qml
	src/ui/qml/markdown/MarkdownView.qml
	src/ui/qml/markdown/MarkdownText.qml
	src/ui/qml/markdown/MdHeading.qml
	src/ui/qml/markdown/MdParagraph.qml
	src/ui/qml/markdown/MdCodeBlock.qml
	src/ui/qml/markdown/MdList.qml
	src/ui/qml/markdown/MdTable.qml
	src/ui/qml/markdown/MdImage.qml
	src/ui/qml/markdown/MdHorizontalRule.qml
	src/ui/qml/markdown/MdHtmlBlock.qml
	src/ui/qml/markdown/MdBlockquote.qml
	src/ui/qml/markdown/MdCallout.qml
	src/ui/qml/views/BrowseFontsView.qml
	src/ui/qml/views/FontDemoView.qml
	src/ui/qml/accessories/CategoryFilterAccessory.qml
	src/ui/qml/views/ExtensionFormView.qml
	src/ui/qml/form/FormFilePicker.qml
	src/ui/qml/form/FormAppSelector.qml
	src/ui/qml/form/FormDateInput.qml
	src/ui/qml/form/FormLinkAccessory.qml
	src/ui/qml/accessories/ExtensionDropdownAccessory.qml
	src/ui/qml/views/EditKeywordsFormView.qml
	src/ui/qml/launcher/HorizontalLoadingBar.qml
	src/ui/qml/views/SearchFilesView.qml
	src/ui/qml/detail/TextViewer.qml
	src/ui/qml/detail/FilePreview.qml
	src/ui/qml/views/DMenuView.qml
	src/ui/qml/views/ScriptOutputText.qml
	src/ui/qml/views/ScriptExecutorView.qml
	src/ui/qml/settings/SettingsWindow.qml
	src/ui/qml/settings/SettingsScrollEdge.qml
	src/ui/qml/settings/SettingsNavButtons.qml
	src/ui/qml/settings/SettingsSidebar.qml
	src/ui/qml/settings/SettingsRow.qml
	src/ui/qml/settings/SettingsGroup.qml
	src/ui/qml/settings/SettingsSectionLabel.qml
	src/ui/qml/settings/InlineEditableText.qml
	src/ui/qml/settings/SettingsToggle.qml
	src/ui/qml/settings/GeneralSettingsPage.qml
	src/ui/qml/settings/AppearanceSettingsPage.qml
	src/ui/qml/settings/AdvancedSettingsPage.qml
	src/ui/qml/settings/ShortcutsSettingsPage.qml
	src/ui/qml/settings/ExtensionSettingsPage.qml
	src/ui/qml/settings/AboutSettingsPage.qml
	src/ui/qml/form/ShortcutRecorderField.qml
	src/ui/qml/form/ShortcutRecorderCapture.qml
	src/ui/qml/actions/ShortcutRecorderPanel.qml
	src/ui/qml/form/ShortcutField.qml
	src/ui/qml/views/PreferenceFormView.qml
	src/ui/qml/settings/SettingsPreferenceForm.qml
	src/ui/qml/views/MissingPreferenceView.qml
	src/ui/qml/views/OAuthOverlayView.qml
	src/ui/qml/hud/HudWindow.qml
	src/ui/qml/onboarding/OnboardingWindow.qml
)

if(WAYLAND_LAYER_SHELL)
	list(APPEND VICINAE_QML_FILES
		src/ui/qml/launcher/LauncherWindowLayerShell.qml
		src/ui/qml/hud/HudWindowLayerShell.qml
	)
endif()

if(APPLE)
	list(APPEND VICINAE_QML_FILES
		src/ui/qml/launcher/LauncherWindowMacOS.qml
		src/ui/qml/onboarding/OnboardingWindowMacOS.qml
		src/ui/qml/hud/HudWindowMacOS.qml
		src/ui/qml/controls/CompletionPanelMacOS.qml
		src/ui/qml/controls/PopupMaterialMacOS.qml
		src/ui/qml/settings/SettingsWindowMacOS.qml
	)
endif()

if(WIN32)
	list(APPEND VICINAE_QML_FILES
		src/ui/qml/launcher/LauncherWindowWindows.qml
		src/ui/qml/hud/HudWindowWindows.qml
	)
endif()

# Strip directory prefix so resources are qrc:/Vicinae/Foo.qml
foreach(qml_file IN LISTS VICINAE_QML_FILES)
	get_filename_component(alias ${qml_file} NAME)
	set_source_files_properties(${qml_file} PROPERTIES QT_RESOURCE_ALIAS ${alias})
endforeach()

set(VICINAE_QML_SOURCES
	src/ui/quick/source-blend-rect.hpp
	src/ui/quick/vici-image-item.hpp
	src/ui/quick/hover-activation.hpp
	src/ui/quick/hover-activation.cpp
	src/ui/quick/match-highlighter.hpp
	src/ui/quick/match-highlighter.cpp
	src/ui/quick/completion-model.hpp
	src/ui/quick/markdown-model.hpp
	src/ui/quick/text-selection-controller.hpp
	src/ui/quick/window-material-attached.hpp
	src/ui/quick/window-material-attached.cpp
	src/ui/quick/popup-placement-attached.hpp
	src/ui/quick/popup-placement-attached.cpp
	src/ui/quick/shortcut-inhibitor-attached.hpp
	src/ui/quick/shortcut-inhibitor-attached.cpp
	${CMAKE_SOURCE_DIR}/vendor/kirigami-wheelhandler/wheelhandler.h
	${CMAKE_SOURCE_DIR}/vendor/kirigami-wheelhandler/wheelhandler.cpp
)

if(APPLE)
	list(APPEND VICINAE_QML_SOURCES
		src/ui/quick/macos-chrome-attached.hpp
		src/ui/quick/macos-chrome-attached.mm
	)
endif()

if(WIN32)
	list(APPEND VICINAE_QML_SOURCES
		src/ui/quick/windows-chrome-attached.hpp
		src/ui/quick/windows-chrome-attached.cpp
		src/ui/windows/launcher-window-platform.cpp
	)
endif()

qt_add_qml_module(${TARGET}
	URI Vicinae
	VERSION 1.0
	RESOURCE_PREFIX /
	QML_FILES ${VICINAE_QML_FILES}
	SOURCES ${VICINAE_QML_SOURCES}
	DEPENDENCIES QtQuick
	NO_PLUGIN
)

# BASE keeps the resource path at :/shaders/<name>.qsb instead of mirroring the source tree.
qt6_add_shaders(${TARGET} "sourceblend_shaders"
	BATCHABLE
	PREFIX "/shaders"
	BASE "src/ui/quick/shaders"
	GLSL "100es,120,150,330,440"
	FILES
		src/ui/quick/shaders/sourceblend.vert
		src/ui/quick/shaders/sourceblend.frag
)

if(Qt6_FOUND AND NOT WIN32)
	qt_import_plugins(${TARGET}
        INCLUDE Qt6::QWaylandIntegrationPlugin
    )
endif()

# src/ui/quick holds the C++ types registered into the Vicinae QML module. It must be on the
# include path because the generated type registration file includes those headers by basename.
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/quick ${CMAKE_SOURCE_DIR}/vendor/kirigami-wheelhandler ${CMAKE_SOURCE_DIR}/vendor/sqlcipher)
target_link_libraries(${TARGET} PRIVATE ${LIBS})
if(APPLE)
	target_link_libraries(${TARGET} PRIVATE "-framework AppKit" "-framework QuartzCore" "-framework UniformTypeIdentifiers" "-framework Carbon" "-framework CoreServices" "-framework ApplicationServices" "-framework CoreAudio" "-framework CoreFoundation" "-framework Security" "-F/System/Library/PrivateFrameworks" "-framework login")
endif()

if(APPLE AND DEFINED ENV{VICINAE_CODESIGN_IDENTITY})
	add_custom_command(TARGET ${TARGET} POST_BUILD
		COMMAND codesign --force --identifier ${VICINAE_BUNDLE_ID}
			--sign "$ENV{VICINAE_CODESIGN_IDENTITY}" $<TARGET_FILE:${TARGET}>
		VERBATIM
		COMMENT "codesign ${TARGET} ($ENV{VICINAE_CODESIGN_IDENTITY})")
endif()

target_compile_features(${TARGET} PUBLIC cxx_std_23)

# Workaround: libc++ pre-C++26 has unconstrained std::variant relational operators,
# which trigger a hard error inside glaze's tuplet-based reflection over structs
# whose fields contain variants. Compile this file in C++26 mode on Apple so
# libc++'s SFINAE on variant::operator< makes the synthesis short-circuit.
if(APPLE)
	set_source_files_properties(src/extend/model-deser.cpp
		PROPERTIES COMPILE_OPTIONS "-std=gnu++26")
endif()

if (USE_PRECOMPILED_HEADERS)
	message(STATUS "Precompiled headers are enabled")
	target_precompile_headers(${TARGET} PUBLIC "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h>")
	set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)
else()
	message(STATUS "Precompiled headers are disabled")
endif()

# Include sources excluded from the current platform's target.
file(GLOB_RECURSE VICINAE_I18N_SOURCES CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.mm)

qt_add_translations(${TARGET}
	SOURCE_TARGETS ${TARGET}
	TS_FILE_BASE vicinae
	TS_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/translations/vicinae_de.ts
		${CMAKE_CURRENT_SOURCE_DIR}/translations/vicinae_fr.ts
		${CMAKE_CURRENT_SOURCE_DIR}/translations/vicinae_it.ts
		${CMAKE_CURRENT_SOURCE_DIR}/translations/vicinae_ru.ts
		${CMAKE_CURRENT_SOURCE_DIR}/translations/vicinae_uk.ts
		${CMAKE_CURRENT_SOURCE_DIR}/translations/vicinae_zh_CN.ts
	PLURALS_TS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/translations/vicinae_en.ts
	SOURCES ${VICINAE_I18N_SOURCES}
	LUPDATE_OPTIONS -locations relative -no-obsolete
	MERGE_QT_TRANSLATIONS
	QT_TRANSLATION_CATALOGS qtbase qtdeclarative
)

if(WIN32)
	include(WindowsDeploy)
	vicinae_windows_deploy(${TARGET})
elseif(NOT APPLE)
	install(TARGETS ${TARGET}
		RUNTIME DESTINATION ${VICINAE_LIBEXEC_DIR}
	)
endif()

if (BUILD_TESTS AND UNIX AND NOT APPLE)
	set(TEST_TARGET ${TARGET}-tests)
	find_package(Catch2 3 REQUIRED)
	add_executable(${TEST_TARGET}
		src/internal/keyboard/keyboard.cpp
		src/internal/keyboard/keybind-manager.cpp
	)
	target_sources(${TEST_TARGET} PRIVATE
		src/services/calculator-service/qalculate/qalculate-backend.cpp
		tests/calculator/qalculate-backend.cpp
	)
	target_link_libraries(${TEST_TARGET} PRIVATE vicinae::common qalculate Qt6::Concurrent)
	target_link_libraries(${TEST_TARGET} PRIVATE Catch2::Catch2WithMain Qt6::Core Qt6::Gui)
	target_compile_features(${TEST_TARGET} PUBLIC cxx_std_23)
endif()
