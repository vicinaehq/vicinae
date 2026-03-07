set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(LIBS)
set(TARGET vicinae-server)

find_package(Qt6 REQUIRED COMPONENTS Core Network Svg Concurrent Quick Qml GuiPrivate QuickDialogs2 QuickControls2 ShaderTools)

if (USE_SYSTEM_KF6)
	find_package(KF6SyntaxHighlighting REQUIRED)
endif()

list(APPEND LIBS
	Qt6::Network Qt6::Svg Qt6::Concurrent Qt6::Quick Qt6::Qml
	Qt6::GuiPrivate # for deeper integration with wayland protocols, we need wl_surface
	Qt6::QuickDialogs2 Qt6::QuickControls2
	${CMARK_LIBRARY} ${CMARK_EXT_LIBRARY}
	zip
	qt6keychain
	pugixml
	KF6::SyntaxHighlighting
	glaze::glaze
	vicinae::xdgpp
	vicinae::scriptcommand
	vicinae::glyph
	vicinae::common
	vicinae::fuzzy
	vicinae::crypto
	sqlcipher
)

if (UNIX AND NOT APPLE)
	find_package(Qt6 REQUIRED COMPONENTS DBus)
	list(APPEND LIBS wayland-client Qt6::DBus)
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

if (CMAKE_BUILD_TYPE MATCHES "Debug")
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

	src/ui/action-pannel/action.hpp
	src/ui/action-pannel/action.cpp
	src/ui/action-pannel/action-panel-state.hpp
	src/ui/action-pannel/action-panel-view.hpp
	src/ui/action-pannel/action-list-view.hpp
	src/ui/action-pannel/action-list-view.cpp

	
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


	src/create-quicklink-command.hpp

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

	src/theme/theme-file.hpp
	src/theme/theme-file.cpp
	src/theme/theme-parser.cpp
	src/theme/theme-db.hpp
	src/theme/theme-db.cpp

	src/extensions/clipboard/clipboard-history-command.cpp
	src/extensions/clipboard/history/clipboard-history-controller.hpp
	src/extensions/clipboard/history/clipboard-history-controller.cpp
	src/extensions/browser/browser-extension.cpp

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
	src/services/root-item-manager/visit-tracker.cpp
	
	src/services/app-service/app-service.hpp
	src/services/app-service/app-service.cpp

	src/services/glyph-service/glyph-service.hpp
	src/services/glyph-service/glyph-service.cpp



	src/services/calculator-service/calculator-service.hpp
	src/services/calculator-service/calculator-service.cpp

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

	src/qml/oauth-overlay-host.hpp
	src/qml/oauth-overlay-host.cpp



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
	src/services/browser-extension-service.hpp

	src/internal/program-db/program-db.hpp
	src/internal/program-db/program-db.cpp

	src/internal/db/db-encryption.hpp
	src/internal/db/db-encryption.cpp
	src/internal/db/database-key.hpp
	src/internal/db/database-key.cpp

	src/services/power-manager/dummy-power-manager.hpp

	src/services/desktop-notification/dummy-desktop-notification-client.hpp

	src/services/audio-control/pactl/pactl-audio-control.cpp
	src/services/audio-control/dummy-audio-control.hpp

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

	src/services/snippet/snippet-server.cpp
	src/services/ai/ai-provider.hpp
	src/services/ai/ai-service.hpp
	src/services/ai/ai-config.hpp

	src/qml/quick-ai-view-host.hpp
	src/qml/quick-ai-view-host.cpp

	src/qml/keybind-bridge.hpp
	src/qml/launcher-window.hpp
	src/qml/launcher-window.cpp
	src/qml/root-view-host.hpp
	src/qml/root-view-host.cpp
	src/qml/root-search-model.hpp
	src/qml/root-search-model.cpp
	src/qml/root-search-sources.hpp
	src/qml/root-search-sources.cpp
	src/qml/section-source.hpp
	src/qml/section-list-model.hpp
	src/qml/section-list-model.cpp
	src/qml/fuzzy-section.hpp
	src/qml/list-view-host.hpp
	src/qml/action-panel-model.hpp
	src/qml/action-panel-model.cpp
	src/qml/completion-model.hpp
	src/qml/completion-model.cpp
	src/qml/action-panel-controller.hpp
	src/qml/action-panel-controller.cpp
	src/qml/alert-model.hpp
	src/qml/alert-model.cpp
	src/qml/bridge-view.hpp
	src/qml/calc-history-model.hpp
	src/qml/calc-history-model.cpp
	src/qml/calc-history-view-host.hpp
	src/qml/calc-history-view-host.cpp
	src/qml/theme-list-model.hpp
	src/qml/theme-list-model.cpp
	src/qml/theme-view-host.hpp
	src/qml/theme-view-host.cpp
	src/qml/grid-source.hpp
	src/qml/section-grid-model.hpp
	src/qml/section-grid-model.cpp
	src/qml/emoji-grid-model.hpp
	src/qml/emoji-grid-model.cpp
	src/qml/emoji-grid-view-host.hpp
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
	src/qml/source-blend-rect.cpp
	src/qml/image-url.hpp
	src/qml/image-url.cpp
	src/qml/image-source.hpp
	src/qml/image-source.cpp
	src/qml/vici-image-item.hpp
	src/qml/vici-image-item.cpp
	src/qml/config-bridge.hpp
	src/qml/theme-bridge.hpp
	src/qml/keyboard-bridge.hpp
	src/qml/global-shortcut-bridge.hpp
	src/qml/platform-bridge.hpp
	src/qml/shortcut-conflict.hpp
	src/qml/shortcut-conflict.cpp
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
	src/qml/font-browser-view-host.hpp
	src/qml/font-grid-model.hpp
	src/qml/font-grid-model.cpp
	src/qml/font-demo-view-host.hpp
	src/qml/font-demo-view-host.cpp
	src/qml/edit-keywords-view-host.hpp
	src/qml/edit-keywords-view-host.cpp
	src/qml/hud-bridge.hpp
	src/qml/hud-bridge.cpp

	src/qml/settings-window.hpp
	src/qml/settings-window.cpp
	src/qml/onboarding-window.hpp
	src/qml/onboarding-window.cpp
	src/qml/general-settings-model.hpp
	src/qml/general-settings-model.cpp
	src/qml/keybind-settings-model.hpp
	src/qml/keybind-settings-model.cpp
	src/qml/settings-sidebar-model.hpp
	src/qml/settings-sidebar-model.cpp
	src/qml/extension-settings-model.hpp
	src/qml/extension-settings-model.cpp
	src/qml/preference-form-model.hpp
	src/qml/preference-form-model.cpp
	src/qml/provider-command-model.hpp
	src/qml/provider-command-model.cpp

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
		src/services/files-service/macos/spotlight-file-indexer.hpp
		src/services/files-service/macos/spotlight-file-indexer.mm
		src/services/tray/macos/tray-service-macos.hpp
		src/services/tray/macos/tray-service-macos.mm
		src/services/paste/macos-paste-service.hpp
		src/services/paste/macos-paste-service.mm
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
		src/services/power-manager/macos/macos-power-manager.mm
		src/services/window-manager/macos/macos-window.mm
		src/services/window-manager/macos/macos-window-manager.mm
		src/services/snippet/macos-snippet-server.mm
		src/services/update/macos-update-installer.mm
		src/services/permissions/macos-permission-service.mm
		src/services/autostart/macos-login-item.mm
		src/services/desktop-notification/macos/macos-notification-client.mm
		PROPERTIES COMPILE_OPTIONS "-fobjc-arc")
	list(APPEND LIBS "-framework ServiceManagement" "-framework UserNotifications")

	if (AUTO_ENABLE_AUTOSTART)
		add_compile_definitions(AUTO_ENABLE_AUTOSTART)
	endif()
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

list(APPEND LIBS qalculate)
list(APPEND SRCS
	src/services/calculator-service/qalculate/qalculate-backend.hpp
	src/services/calculator-service/qalculate/qalculate-backend.cpp
)

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
		src/services/file-chooser/xdp-file-chooser/xdp-file-chooser.hpp
		src/services/file-chooser/xdp-file-chooser/xdp-file-chooser.cpp
		src/services/power-manager/systemd/systemd-power-manager.cpp
		src/services/desktop-notification/freedesktop/freedesktop-notification-client.cpp

		src/services/paste/linux-paste-service.cpp

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
		list(APPEND LIBS X11::xcb X11::xcb_keysyms)
		message(STATUS "XCB library found for X11 window manager support")
	endif()

    include("Wayland")

    wayland_generate_protocol("wlr-foreign-toplevel-management-unstable-v1")
	wayland_generate_protocol("ext-data-control-v1")
	wayland_generate_protocol("kde-blur")
	wayland_generate_protocol("ext-background-effect-v1")
	wayland_generate_protocol("keyboard-shortcuts-inhibit-unstable-v1")
	wayland_generate_protocol("wlr-layer-shell-unstable-v1")
	wayland_generate_protocol("xdg-shell")
	wayland_generate_protocol("vicinae-hotkey-v1")

	list(APPEND LIBS xkbcommon)

	# Common Wayland support files
	list(APPEND SRCS
		src/internal/wayland/globals.cpp
		src/internal/wayland/display.cpp
		src/internal/wayland/registry.cpp
		src/internal/wayland/seat.cpp
		src/services/window-material/ext-background-effect-v1-manager.cpp
		src/services/window-material/kde-background-effect-manager.cpp
		src/services/shortcut-inhibit/wayland-shortcut-inhibit-manager.cpp
		src/services/clipboard/data-control/data-control-clipboard-server.hpp
		src/services/clipboard/data-control/data-control-clipboard-server.cpp

		src/services/global-shortcuts/vicinae-hotkey-global-shortcut-backend.hpp
		src/services/global-shortcuts/vicinae-hotkey-global-shortcut-backend.cpp

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
	src/qml/qml/LauncherWindow.qml
	src/qml/qml/PopupMaterial.qml
	src/qml/qml/ViciPopover.qml
	src/qml/qml/SearchBar.qml
	src/qml/qml/GenericListView.qml
	src/qml/qml/HoverResetOnModelChange.qml
	src/qml/qml/HoverResetOnShow.qml
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
	src/qml/qml/TextBadge.qml
	src/qml/qml/CommandListView.qml
	src/qml/qml/CalcHistoryListView.qml
	src/qml/qml/CalculatorResultDelegate.qml
	src/qml/qml/ThemeListView.qml
	src/qml/qml/ThemeItemDelegate.qml
	src/qml/qml/GenericGridView.qml
	src/qml/qml/EmptyView.qml
	src/qml/qml/EmojiGridView.qml
	src/qml/qml/ViciButton.qml
	src/qml/qml/ViciScrollBar.qml
	src/qml/qml/ViciDivider.qml
	src/qml/qml/ViciToolTip.qml
	src/qml/qml/ExtensionView.qml
	src/qml/qml/MarkdownDetailView.qml
	src/qml/qml/MarkdownDetailContent.qml
	src/qml/qml/QuickAIView.qml
	src/qml/qml/ExtensionGridView.qml
	src/qml/qml/ActionPanelPopover.qml
	src/qml/qml/ActionListPanel.qml
	src/qml/qml/ActionItemDelegate.qml
	src/qml/qml/SearchableDropdown.qml
	src/qml/qml/ClipboardHistoryView.qml
	src/qml/qml/ClipboardFilterAccessory.qml
	src/qml/qml/EmojiCategoryFilterAccessory.qml
	src/qml/qml/DetailPanel.qml
	src/qml/qml/MetadataBar.qml
	src/qml/qml/AlertDialog.qml
	src/qml/qml/FormView.qml
	src/qml/qml/FormField.qml
	src/qml/qml/FormInputBackground.qml
	src/qml/qml/FormTextInput.qml
	src/qml/qml/FormTextArea.qml
	src/qml/qml/FormCheckbox.qml
	src/qml/qml/FormSeparator.qml
	src/qml/qml/SnippetFormView.qml
	src/qml/qml/AliasFormView.qml
	src/qml/qml/ShortcutFormView.qml
	src/qml/qml/FormCompletedInput.qml
	src/qml/qml/FormCompletedTextArea.qml
	src/qml/qml/PlaceholderCompleter.qml
	src/qml/qml/CompletionPopup.qml
	src/qml/qml/CreateExtensionFormView.qml
	src/qml/qml/ArgCompleter.qml
	src/qml/qml/DetailListView.qml
	src/qml/qml/StoreListingView.qml
	src/qml/qml/AlertBanner.qml
	src/qml/qml/StoreDetailView.qml
	src/qml/qml/StoreIntroView.qml
	src/qml/qml/ImageViewer.qml
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
	src/qml/qml/FontDemoView.qml
	src/qml/qml/CategoryFilterAccessory.qml
	src/qml/qml/ExtensionFormView.qml
	src/qml/qml/FormFilePicker.qml
	src/qml/qml/FormAppSelector.qml
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
	src/qml/qml/SettingsSidebar.qml
	src/qml/qml/SettingsRow.qml
	src/qml/qml/SettingsGroup.qml
	src/qml/qml/SettingsSectionLabel.qml
	src/qml/qml/InlineEditableText.qml
	src/qml/qml/SettingsToggle.qml
	src/qml/qml/GeneralSettingsPage.qml
	src/qml/qml/AppearanceSettingsPage.qml
	src/qml/qml/AdvancedSettingsPage.qml
	src/qml/qml/ShortcutsSettingsPage.qml
	src/qml/qml/ExtensionSettingsPage.qml
	src/qml/qml/AboutSettingsPage.qml
	src/qml/qml/ShortcutRecorderField.qml
	src/qml/qml/ShortcutField.qml
	src/qml/qml/PreferenceFormView.qml
	src/qml/qml/SettingsPreferenceForm.qml
	src/qml/qml/MissingPreferenceView.qml
	src/qml/qml/OAuthOverlayView.qml
	src/qml/qml/HudWindow.qml
	src/qml/qml/OnboardingWindow.qml
)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
	list(APPEND VICINAE_QML_FILES src/qml/qml/ButtonShowcase.qml src/qml/qml/UIShowcase.qml)
endif()

if(WAYLAND_LAYER_SHELL)
	list(APPEND VICINAE_QML_FILES
		src/qml/qml/LauncherWindowLayerShell.qml
		src/qml/qml/HudWindowLayerShell.qml
	)
endif()

if(APPLE)
	list(APPEND VICINAE_QML_FILES
		src/qml/qml/LauncherWindowMacOS.qml
		src/qml/qml/SettingsWindowMacOS.qml
		src/qml/qml/OnboardingWindowMacOS.qml
		src/qml/qml/HudWindowMacOS.qml
		src/qml/qml/CompletionPanelMacOS.qml
		src/qml/qml/PopupMaterialMacOS.qml
	)
endif()

# Strip directory prefix so resources are qrc:/Vicinae/Foo.qml
foreach(qml_file IN LISTS VICINAE_QML_FILES)
	get_filename_component(alias ${qml_file} NAME)
	set_source_files_properties(${qml_file} PROPERTIES QT_RESOURCE_ALIAS ${alias})
endforeach()

set(VICINAE_QML_SOURCES
	src/qml/source-blend-rect.hpp
	src/qml/vici-image-item.hpp
	src/qml/hover-activation.hpp
	src/qml/hover-activation.cpp
	src/qml/completion-model.hpp
	src/qml/markdown/markdown-model.hpp
	src/qml/markdown/text-selection-controller.hpp
	src/qml/window-material-attached.hpp
	src/qml/window-material-attached.cpp
	src/qml/popup-placement-attached.hpp
	src/qml/popup-placement-attached.cpp
	src/qml/shortcut-inhibitor-attached.hpp
	src/qml/shortcut-inhibitor-attached.cpp
	${CMAKE_SOURCE_DIR}/vendor/kirigami-wheelhandler/wheelhandler.h
	${CMAKE_SOURCE_DIR}/vendor/kirigami-wheelhandler/wheelhandler.cpp
)

if(APPLE)
	list(APPEND VICINAE_QML_SOURCES
		src/qml/macos-chrome-attached.hpp
		src/qml/macos-chrome-attached.mm
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

qt6_add_shaders(${TARGET} "sourceblend_shaders"
	BATCHABLE
	PREFIX "/shaders"
	GLSL "100es,120,150,330,440"
	FILES
		src/qml/shaders/sourceblend.vert
		src/qml/shaders/sourceblend.frag
)

if(Qt6_FOUND)
	qt_import_plugins(${TARGET}
        INCLUDE Qt6::QWaylandIntegrationPlugin
    )
endif()

target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/qml ${CMAKE_CURRENT_SOURCE_DIR}/src/qml/markdown ${CMAKE_SOURCE_DIR}/vendor/kirigami-wheelhandler)
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

if(NOT APPLE)
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
	target_link_libraries(${TEST_TARGET} PRIVATE qalculate Qt6::Concurrent)
	target_link_libraries(${TEST_TARGET} PRIVATE Catch2::Catch2WithMain Qt6::Core Qt6::Gui)
	target_compile_features(${TEST_TARGET} PUBLIC cxx_std_23)
endif()
