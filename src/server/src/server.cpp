#include "config/config.hpp"
#include "environment.hpp"
#include "services/ai/ai-tool.hpp"
#include <QStyleHints>
#include "services/ai/agentic-loop.hpp"
#include "extension/extension.hpp"
#include "root-search/browser-tabs/browser-tabs-provider.hpp"
#include "root-search/scripts/script-root-provider.hpp"
#include "extension/manager/extension-manager.hpp"
#include "favicon/favicon-service.hpp"
#include "font-service.hpp"
#ifdef Q_OS_LINUX
#include "icon-theme-db/icon-theme-db.hpp"
#endif
#include "extension-interval-scheduler.hpp"
#include "ipc-command-server.hpp"
#include "keyboard/keybind-manager.hpp"
#include "log/message-handler.hpp"
#include "overlay-controller/overlay-controller.hpp"
#include "extensions/root/root-command.hpp"
#include "root-search/apps/app-root-provider.hpp"
#include "root-search/extensions/extension-root-provider.hpp"
#include "root-search/shortcuts/shortcut-root-provider.hpp"
#ifdef Q_OS_MACOS
#include "root-search/macos-settings/macos-settings-root-provider.hpp"
#endif
#include "service-registry.hpp"
#include "services/window-material/window-material-manager.hpp"
#include "qml/window-material-attached.hpp"
#include "services/ai/ai-provider.hpp"
#include "services/shortcut-inhibit/shortcut-inhibit-manager.hpp"
#include "qml/shortcut-inhibitor-attached.hpp"
#include "services/file-chooser/file-chooser-service.hpp"
#include "services/browser-extension-service.hpp"
#ifdef AUTO_INSTALL_BROWSER_MANIFESTS
#include "services/browser-extension/native-host-installer.hpp"
#endif
#include "services/calculator-service/calculator-service.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "db/database-key.hpp"
#include "services/glyph-service/glyph-service.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "services/files-service/file-service.hpp"
#include "services/local-storage/local-storage-service.hpp"
#include "services/oauth/oauth-service.hpp"
#include "services/ai/ollama/ollama-ai-provider.hpp"
#include "services/power-manager/power-manager.hpp"
#include "services/raycast/raycast-store.hpp"
#include "services/extension-store/vicinae-store.hpp"
#include "services/script-command/script-command-service.hpp"
#include "services/shortcut/shortcut-service.hpp"
#include "services/news/news-service.hpp"
#include "services/telemetry/telemetry-service.hpp"
#include "services/update/update-service.hpp"
#ifdef Q_OS_MACOS
#include "services/update/macos-update-installer.hpp"
#else
#include "services/update/null-update-installer.hpp"
#endif
#include "services/toast/toast-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include "services/wallpaper/wallpaper-manager.hpp"
#include "services/app-runtime/app-runtime.hpp"
#include "services/snippet/snippet-service.hpp"
#include "services/global-shortcuts/global-shortcut-service.hpp"
#include "services/global-shortcuts/global-shortcut-backend-factory.hpp"
#ifdef Q_OS_LINUX
#include "services/input-server/linux-input-server.hpp"
#include "services/snippet/linux-snippet-server.hpp"
#elif defined(Q_OS_MACOS)
#include "services/snippet/macos-snippet-server.hpp"
#else
#include "services/snippet/null-snippet-server.hpp"
#endif
#include "services/audio-control/audio-control-service.hpp"
#include "services/paste/paste-service.hpp"
#include "services/paste/dummy-paste-service.hpp"
#ifdef Q_OS_LINUX
#include "services/paste/linux-paste-service.hpp"
#endif
#ifdef Q_OS_MACOS
#include "services/paste/macos-paste-service.hpp"
#endif
#include "services/ai/ai-service.hpp"
#include "settings-controller/settings-controller.hpp"
#include "services/tray/tray-service.hpp"
#include "qml/launcher-window.hpp"
#include "qml/onboarding-window.hpp"
#include "utils.hpp"
#include "vicinae.hpp"
#include "generated/version.h"
#include <filesystem>
#include <QGuiApplication>
#include <QPointer>
#include <QQuickWindow>
#include <QString>
#include <qlockfile.h>
#include <qlogging.h>
#include <QtQuickControls2/QQuickStyle>
#include "server.hpp"

#ifdef Q_OS_MACOS
#include "ipc-command-handler.hpp"
#include "qml/macos-chrome-attached.hpp"
#include <QFileOpenEvent>
#endif

#ifdef AUTO_ENABLE_AUTOSTART
#include "services/autostart/macos-login-item.hpp"
#endif

#ifdef Q_OS_MACOS
class UrlSchemeOpenFilter : public QObject {
public:
  UrlSchemeOpenFilter(ApplicationContext &ctx) : m_ctx(ctx) {}

protected:
  bool eventFilter(QObject *watched, QEvent *event) override {
    if (event->type() == QEvent::FileOpen) {
      const QUrl url = static_cast<QFileOpenEvent *>(event)->url();
      if (Omnicast::APP_SCHEMES.contains(url.scheme())) {
        IpcCommandHandler handler(m_ctx);
        if (auto res = handler.handleUrl(url); !res) {
          qWarning() << "Failed to handle deeplink" << url.toString() << ":" << res.error();
        }
        return true;
      }
    }
    return QObject::eventFilter(watched, event);
  }

private:
  ApplicationContext &m_ctx;
};
#endif

static void applyTextRenderingMode(const config::FontConfig &fontConfig) {
  if (fontConfig.rendering == "qt") {
    QQuickWindow::setTextRenderType(QQuickWindow::QtTextRendering);
  } else {
    QQuickWindow::setTextRenderType(QQuickWindow::NativeTextRendering);
  }
}

static constexpr QFont::Weight UI_FONT_WEIGHT = QFont::Medium;

static QFont resolveAppFont(const config::FontConfig &fontConfig) {
  QFont font;
  const auto &family = fontConfig.normal.family;
  if (family == "auto") {
    auto builtin = ServiceRegistry::instance()->fontService()->builtinFontFamily();
    if (!builtin.isEmpty()) font.setFamily(builtin);
  } else if (family != "system") {
    font.setFamily(QString::fromStdString(family));
  }
  font.setPointSizeF(fontConfig.normal.size);
  font.setWeight(UI_FONT_WEIGHT);
  return font;
}

int startServer(const ServerLaunchOptions &launchOpts) {
  qInstallMessageHandler(coloredMessageHandler);

#ifdef AUTO_INSTALL_BROWSER_MANIFESTS
  // always refresh manifests
  vicinae::browser::installNativeHostManifests();
#endif

  // Cheap single-instance probe before building any Qt state. macOS spawns
  // a fresh process on every `open` of the .app; without this guard, each
  // launch would proceed and steal focus.
  if (const auto sock = Omnicast::commandSocketPath(); std::filesystem::exists(sock)) {
    QLocalSocket probe;
    probe.connectToServer(sock.c_str());
    if (probe.waitForConnected(100)) {
      probe.disconnectFromServer();
      qWarning() << "another vicinae instance is already running";
      return 0;
    }
  }

#ifdef Q_OS_MACOS
  if (!qEnvironmentVariableIsSet("QT_MAC_SET_RAISE_PROCESS")) qputenv("QT_MAC_SET_RAISE_PROCESS", "0");
#endif

  int argc = 1;
  static char *argv[] = {strdup("command"), nullptr};
  QGuiApplication const qapp(argc, argv);
  QGuiApplication::setApplicationName("vicinae");
  QQuickWindow::setTextRenderType(QQuickWindow::NativeTextRendering);

  // macOS defaults to cycling Tab focus between text controls only, skipping form fields whose root
  // is a plain Item delegating focus inward.
  QGuiApplication::styleHints()->setTabFocusBehavior(Qt::TabFocusAllControls);

#ifdef Q_OS_MACOS
  macosSetAccessoryActivationPolicy();
  macosReleaseMenuShortcuts();
#endif

  auto m_config = launchOpts.config.empty() ? Omnicast::configDir() / "settings.json"
                                            : std::filesystem::path{launchOpts.config};

  if (const auto launcher = Environment::detectAppLauncher()) {
    qInfo() << "Detected launch prefix:" << *launcher;
  }

  QQuickStyle::setStyle(QStringLiteral("Basic"));

  Omnicast::ensureDirectories();

#ifdef AUTO_ENABLE_AUTOSTART
  vicinae::macos::registerLoginItemOnce();
#endif

  {
    auto registry = ServiceRegistry::instance();

    auto configService = std::make_unique<config::Manager>(m_config);
    auto currentConfig = configService->value();

    const auto vicinaeDbPath = Omnicast::dataDir() / "vicinae.db";
    const auto clipboardDbPath = Omnicast::dataDir() / "clipboard.db";
    auto keys = db::prepareEncryption(currentConfig.encryptSensitiveData, {vicinaeDbPath, clipboardDbPath});

    auto omniDb = std::make_unique<OmniDatabase>(vicinaeDbPath, keys.database);
    auto localStorage = std::make_unique<LocalStorageService>(*omniDb);
    auto extensionManager = std::make_unique<ExtensionManager>();
    auto windowManager = std::make_unique<WindowManager>();
    auto appService = std::make_unique<AppService>(*omniDb.get());
    auto appRuntime = std::make_unique<AppRuntime>(*windowManager, *appService);
    auto clipboardManager = std::make_unique<ClipboardService>(clipboardDbPath, keys.database);
    clipboardManager->setEncryptionKey(keys.clipboard);
#ifdef Q_OS_LINUX
    auto inputServer = std::make_unique<LinuxInputServer>();
    auto snippetServer = std::make_unique<LinuxSnippetServer>(*inputServer);
    auto platformPaste =
        std::unique_ptr<AbstractPasteService>(std::make_unique<LinuxPasteService>(*inputServer));
#elif defined(Q_OS_MACOS)
    auto snippetServer = std::make_unique<MacosSnippetServer>();
    auto platformPaste = std::unique_ptr<AbstractPasteService>(std::make_unique<MacosPasteService>());
#else
    auto snippetServer = std::make_unique<NullSnippetServer>();
    auto platformPaste = std::unique_ptr<AbstractPasteService>(std::make_unique<DummyPasteService>());
#endif
    auto snippetService =
        std::make_unique<SnippetService>(Omnicast::dataDir() / "snippets" / "snippets.json", *snippetServer,
                                         *windowManager, *appRuntime, *clipboardManager);
    auto pasteService = std::make_unique<PasteService>(*clipboardManager, *windowManager, *appService,
                                                       std::move(platformPaste));
    auto fontService = std::make_unique<FontService>();
    auto rootItemManager = std::make_unique<RootItemManager>(*configService, *localStorage);
    auto globalShortcutService = std::make_unique<GlobalShortcutService>(*configService, *rootItemManager,
                                                                         createGlobalShortcutBackend());
    auto shortcutService =
        std::make_unique<ShortcutService>(Omnicast::dataDir() / "shortcuts" / "shortcuts.json", omniDb.get());
    auto toastService = std::make_unique<ToastService>();
    auto glyphService =
        std::make_unique<GlyphService>(Omnicast::dataDir() / "emojis" / "emojis.json", omniDb.get());
    auto calculatorService = std::make_unique<CalculatorService>(*omniDb.get());
    auto fileService = std::make_unique<FileService>(*omniDb);
    auto oauthService = std::make_unique<OAuthService>(*omniDb);
    auto extensionRegistry = std::make_unique<ExtensionRegistry>(*localStorage);
    auto raycastStore = std::make_unique<RaycastStoreService>();
    auto vicinaeStore = std::make_unique<VicinaeStoreService>();

#ifdef HAS_TYPESCRIPT_EXTENSIONS
    if (!launchOpts.noExtensionRuntime) {
      if (!extensionManager->start()) {
        qCritical() << "Failed to load extension manager. Extensions will not work";
      }
    } else {
      qWarning() << "--no-extension-runtime flag was passed, third-party Typescript extensions will not run.";
    }
#else
    qInfo() << "Not starting extension manager has support for typescript extensions has been disabled for "
               "this build.";
#endif

    registry->setFileService(std::move(fileService));
    registry->setToastService(std::move(toastService));
    registry->setShortcutService(std::move(shortcutService));
    registry->setConfig(std::move(configService));
    registry->setRootItemManager(std::move(rootItemManager));
    registry->setCalculatorService(std::move(calculatorService));
    registry->setAppDb(std::move(appService));
    registry->setOmniDb(std::move(omniDb));
    registry->setLocalStorage(std::move(localStorage));
    registry->setExtensionManager(std::move(extensionManager));
    registry->setClipman(std::move(clipboardManager));
    registry->setPasteService(std::move(pasteService));
#ifdef Q_OS_LINUX
    registry->setInputServer(std::move(inputServer));
#endif
    registry->setSnippetServerBackend(std::move(snippetServer));
    registry->setSnippetService(std::move(snippetService));
    registry->setWindowManager(std::move(windowManager));
    registry->setAppRuntime(std::move(appRuntime));
    registry->setFontService(std::move(fontService));
    registry->setGlyphService(std::move(glyphService));
    registry->setRaycastStore(std::move(raycastStore));
    registry->setVicinaeStore(std::move(vicinaeStore));
    registry->setExtensionRegistry(std::move(extensionRegistry));
    registry->setOAuthService(std::move(oauthService));
    registry->setPowerManager(std::make_unique<PowerManager>());
    registry->setGlobalShortcuts(std::move(globalShortcutService));
    registry->setAudioControl(std::make_unique<AudioControlService>());
    registry->setScriptDb(std::make_unique<ScriptCommandService>());
    registry->setBrowserExtension(std::make_unique<BrowserExtensionService>());
    registry->setWindowMaterialManager(std::make_unique<WindowMaterialManager>());
    WindowMaterial::setManager(registry->windowMaterialManager());
    registry->setShortcutInhibitManager(std::make_unique<ShortcutInhibitManager>());
    ShortcutInhibitor::setManager(registry->shortcutInhibitManager());
    registry->setFileChooserService(std::make_unique<FileChooserService>());
    registry->setNewsService(std::make_unique<NewsService>(*registry->config()));
    registry->setTelemetry(std::make_unique<TelemetryService>(*registry->config()));
#ifdef Q_OS_MACOS
    auto updateInstaller = std::unique_ptr<AbstractUpdateInstaller>(std::make_unique<MacosUpdateInstaller>());
#else
    auto updateInstaller = std::unique_ptr<AbstractUpdateInstaller>(std::make_unique<NullUpdateInstaller>());
#endif
    registry->setUpdateService(
        std::make_unique<UpdateService>(*registry->toastService(), std::move(updateInstaller)));
    registry->setWallpaperManager(std::make_unique<WallpaperManager>());
    registry->setAI(std::make_unique<AI::Service>());

    auto root = registry->rootItemManager();
    auto builtinCommandDb = std::make_unique<CommandDatabase>();

    for (const auto &repo : builtinCommandDb->repositories()) {
      root->loadProvider(std::make_unique<ExtensionRootProvider>(repo));
    }

    auto reg = ServiceRegistry::instance()->extensionRegistry();

    QObject::connect(reg, &ExtensionRegistry::extensionsChanged, [reg]() {
      auto reg = ServiceRegistry::instance()->extensionRegistry();
      auto root = ServiceRegistry::instance()->rootItemManager();
      std::set<QString> scanned;

      for (const auto &manifest : reg->scanAll()) {
        auto extension = std::make_unique<ExtensionRootProvider>(std::make_shared<Extension>(manifest));

        scanned.insert(extension->repositoryId());
        root->loadProvider(std::move(extension));
      }

      std::vector<QString> removed;

      for (const auto &provider : root->providers()) {
        if (auto extp = dynamic_cast<ExtensionRootProvider *>(provider)) {
          if (!extp->isBuiltin() && !scanned.contains(extp->uniqueId())) {
            removed.emplace_back(extp->uniqueId());
          }
        }
      }

      for (const auto &id : removed) {
        root->uninstallProvider(id);
      }

      root->updateIndex();
    });

    for (const auto &manifest : reg->scanAll()) {
      auto extension = std::make_shared<Extension>(manifest);

      root->loadProvider(std::make_unique<ExtensionRootProvider>(extension));
    }

    // this one needs to be set last

    root->loadProvider(std::make_unique<AppRootProvider>(*registry->appDb()));
    root->loadProvider(std::make_unique<ShortcutRootProvider>(*registry->shortcuts()));
    root->loadProvider(std::make_unique<ScriptRootProvider>(*registry->scriptDb()));
    root->loadProvider(std::make_unique<BrowserTabProvider>(*registry->browserExtension()));
#ifdef Q_OS_MACOS
    root->loadProvider(std::make_unique<MacSettingsRootProvider>());
#endif

    // Force reload providers to make sure items that depend on them are shown
    root->updateIndex();
  }

  FaviconService::initialize(new FaviconService(Omnicast::dataDir() / "favicon"));
  QGuiApplication::setQuitOnLastWindowClosed(false);
  ApplicationContext ctx;

  ctx.navigation = std::make_unique<NavigationController>(ctx);
  ctx.overlay = std::make_unique<OverlayController>(&ctx);
  ctx.settings = std::make_unique<SettingsController>(ctx);
  ctx.services = ServiceRegistry::instance();

  IpcCommandServer commandServer(&ctx);

  commandServer.start(Omnicast::commandSocketPath());

#ifdef Q_OS_MACOS
  UrlSchemeOpenFilter urlSchemeOpenFilter(ctx);
  qApp->installEventFilter(&urlSchemeOpenFilter);
#endif

  QObject::connect(
      ctx.services->fileService()->indexer(), &AbstractFileIndexer::scanStatusChanged,
      ctx.services->toastService(),
      [toasts = ctx.services->toastService(),
       toast = QPointer<Toast>()](const AbstractFileIndexer::ScanStatus &status) mutable {
        auto *current = toasts->currentToast();

        if (status.isTerminal()) {
          if (toast && toast == current) toast->close();
          return;
        }

        if (current && current != toast) return;

        QString const title =
            QString("Indexing %1").arg(QString::fromStdString(compressPath(status.entrypoint).string()));
        QString const message =
            status.processedFileCount > 0
                ? QString("%1 files").arg(formatCount(static_cast<int>(status.processedFileCount)))
                : QString();

        toasts->dynamic(title, message);
        toast = toasts->currentToast();
      });

  ExtensionIntervalScheduler intervalScheduler(ctx);
  intervalScheduler.rebuild();

  auto configChanged = [&](const config::ConfigValue &next, const config::ConfigValue &prev) {
    auto &theme = ThemeService::instance();
    auto &nextTheme = next.systemTheme();
    auto &prevTheme = prev.systemTheme();
    bool const themeChangeRequired = nextTheme.name != prevTheme.name;

    applyTextRenderingMode(next.font);

    theme.setFontBasePointSize(next.font.normal.size);

    bool const fontChanged =
        next.font.normal.size != prev.font.normal.size || next.font.normal.family != prev.font.normal.family;

    if (fontChanged) { QGuiApplication::setFont(resolveAppFont(next.font)); }

    if (themeChangeRequired) {
      theme.setTheme(nextTheme.name.c_str());
    } else if (fontChanged) {
      theme.reloadCurrentTheme();
    }

    ctx.navigation->setPopToRootOnClose(next.popToRootOnClose);
    ctx.navigation->setCloseOnFocusLoss(next.closeOnFocusLoss);
#ifdef Q_OS_LINUX
    ctx.services->inputServer()->setEnabled(next.inputServer.enabled);
#endif

    KeybindManager::instance()->mergeBinds({next.keybinds.begin(), next.keybinds.end()});
    FaviconService::instance()->setService(next.faviconService.c_str());

    if (nextTheme.iconTheme != "auto") {
      QIcon::setThemeName(nextTheme.iconTheme.c_str());
    }
#ifdef Q_OS_LINUX
    else if (QIcon::themeName() == "hicolor") {
      IconThemeDatabase const iconThemeDb;
      QIcon::setThemeName(iconThemeDb.guessBestTheme());
    }
#endif

    ServiceRegistry::instance()->telemetry()->setEnabled(next.telemetry.systemInfo);
  };

  auto cfgService = ServiceRegistry::instance()->config();

  QObject::connect(cfgService, &config::Manager::configLoadingError, [&ctx](std::string_view message) {
    ctx.navigation->confirmAlert("Failed to load config", qStringFromStdView(message), []() {});
  });

  QObject::connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, [&]() {
    auto &value = cfgService->value();
    auto &theme = value.systemTheme();

    if (theme.iconTheme != "auto") {
      QIcon::setThemeName(theme.iconTheme.c_str());
    }
#ifdef Q_OS_LINUX
    else if (QIcon::themeName() == "hicolor") {
      IconThemeDatabase const iconThemeDb;
      QIcon::setThemeName(iconThemeDb.guessBestTheme());
    }
#endif

    ThemeService::instance().setTheme(theme.name.c_str());
  });

  QObject::connect(
      KeybindManager::instance(), &KeybindManager::keybindChanged,
      [cfgService](Keybind, const Keyboard::Shortcut &shortcut) {
        auto info = KeybindManager::instance()->findBoundInfo(shortcut);
        cfgService->mergeWithUser(
            {.keybinds = config::KeybindMap{{info->id.toStdString(), shortcut.toString().toStdString()}}});
      });

  QObject::connect(cfgService, &config::Manager::configChanged, configChanged);

  if (auto *globalShortcuts = ServiceRegistry::instance()->globalShortcuts()) {
    QObject::connect(globalShortcuts, &GlobalShortcutService::toggleLauncherRequested,
                     [&ctx](quint64) { ctx.navigation->toggleWindow(); });
    QObject::connect(globalShortcuts, &GlobalShortcutService::commandActivated,
                     [&ctx](const EntrypointId &id, quint64) { ctx.navigation->activateEntrypoint(id); });
  }

  QIcon::setFallbackSearchPaths(Environment::fallbackIconSearchPaths());

  QGuiApplication::setFont(resolveAppFont(cfgService->value().font));

  /*
  QTimer::singleShot(2000, [&ctx]() {
    auto agent = new AI::Agent(ctx);
    qDebug() << "agent setup";
    agent->addMessage("Tell me a fun fact about apples");
  });
  */

  configChanged(cfgService->value(), {});

  auto tray = createTrayService();
  if (tray) {
    tray->setVersion(QStringLiteral(VICINAE_GIT_TAG " [" VICINAE_GIT_COMMIT_HASH "]"));
    QObject::connect(tray.get(), &TrayService::toggleRequested, [&ctx]() { ctx.navigation->toggleWindow(); });
    QObject::connect(tray.get(), &TrayService::openSettingsRequested, [&ctx](const QString &tab) {
      if (tab.isEmpty()) {
        ctx.settings->openWindow();
      } else {
        ctx.settings->openTab(tab);
      }
    });
    auto *updates = ServiceRegistry::instance()->updateService();

    QObject::connect(tray.get(), &TrayService::checkForUpdatesRequested, [&ctx, updates]() {
      if (updates->available()) {
        ctx.navigation->popToRoot();
        ctx.navigation->showWindow();
      } else {
        updates->checkNow();
      }
    });

    auto syncTrayUpdate = [tray = tray.get(), updates]() {
      tray->setAvailableUpdate(updates->available() ? updates->available()->tag : QString());
    };
    QObject::connect(updates, &UpdateService::updateChanged, tray.get(), syncTrayUpdate);
    syncTrayUpdate();
    tray->setCheckForUpdatesVisible(updates->checksSupported());
    QObject::connect(tray.get(), &TrayService::quitRequested, []() { QCoreApplication::quit(); });
    tray->show();
  }

  LauncherWindow const qmlWindow(ctx);

  ctx.navigation->launch(std::make_shared<RootCommand>());

  OnboardingWindow onboardingWindow(ctx);
  if (OnboardingWindow::shouldShow()) { onboardingWindow.show(); }

  if (launchOpts.open) {
    ctx.navigation->showWindow();
  } else {
    qInfo() << "Vicinae server successfully started. Call \"vicinae toggle\" to toggle the window";
  }

  auto ret = qApp->exec();
  ctx.services->clipman()->clipboardServer()->stop();
  ctx.services->extensionManager()->stop();
  return ret;
}
