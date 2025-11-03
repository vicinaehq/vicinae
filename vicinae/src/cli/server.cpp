#include "daemon/ipc-client.hpp"
#include "environment.hpp"
#include "extension/manager/extension-manager.hpp"
#include "favicon/favicon-service.hpp"
#include "font-service.hpp"
#include "icon-theme-db/icon-theme-db.hpp"
#include "ipc-command-handler.hpp"
#include "ipc-command-server.hpp"
#include "keyboard/keybind-manager.hpp"
#include "log/message-handler.hpp"
#include "overlay-controller/overlay-controller.hpp"
#include "pid-file/pid-file.hpp"
#include "extensions/root/root-command.hpp"
#include "root-search/apps/app-root-provider.hpp"
#include "root-search/extensions/extension-root-provider.hpp"
#include "root-search/shortcuts/shortcut-root-provider.hpp"
#include "service-registry.hpp"
#include "services/calculator-service/calculator-service.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/config/config-service.hpp"
#include "services/emoji-service/emoji-service.hpp"
#include "services/files-service/file-service.hpp"
#include "services/local-storage/local-storage-service.hpp"
#include "services/oauth/oauth-service.hpp"
#include "services/power-manager/power-manager.hpp"
#include "services/raycast/raycast-store.hpp"
#include "services/extension-store/vicinae-store.hpp"
#include "services/shortcut/shortcut-service.hpp"
#include "services/toast/toast-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include "settings-controller/settings-controller.hpp"
#include "settings/settings-window.hpp"
#include "ui/launcher-window/launcher-window.hpp"
#include "vicinae.hpp"
#include <QString>
#include <qlogging.h>
#include <qpixmapcache.h>
#include <qstylefactory.h>
#include "lib/CLI11.hpp"
#include "server.hpp"

static char *argv[] = {strdup("command"), nullptr};

void CliServerCommand::setup(CLI::App *app) {
  app->add_flag("--open", m_open, "Open the main window once the server is started");
  app->add_flag("--no-replace", m_noReplace, "Exit with non-zero error code if a server is already running");
}

void CliServerCommand::run(CLI::App *app) {
  int argc = 1;
  PidFile pidFile(Omnicast::APP_ID.toStdString());
  DaemonIpcClient client;
  bool killed = false;

  qInstallMessageHandler(coloredMessageHandler);

  if (client.connect() && client.ping() && pidFile.exists()) {
    if (m_noReplace) {
      std::cerr
          << "A server is already running. Omit --no-replace if you want to replace the existing instance."
          << std::endl;
      exit(1);
      if (m_open) { client.open(); }
      return;
    }

    pidFile.kill();
    qInfo() << "Killed existing vicinae instance";
  }

  QApplication qapp(argc, argv);

  // discard system specific qt theming
  qapp.setStyle(QStyleFactory::create("fusion"));
  pidFile.write(qApp->applicationPid());
  std::filesystem::create_directories(Omnicast::runtimeDir());

  {
    auto registry = ServiceRegistry::instance();
    auto omniDb = std::make_unique<OmniDatabase>(Omnicast::dataDir() / "vicinae.db");
    auto localStorage = std::make_unique<LocalStorageService>(*omniDb);
    auto rootItemManager = std::make_unique<RootItemManager>(*omniDb.get());
    auto extensionManager = std::make_unique<ExtensionManager>();
    auto windowManager = std::make_unique<WindowManager>();
    auto appService = std::make_unique<AppService>(*omniDb.get());
    auto clipboardManager =
        std::make_unique<ClipboardService>(Omnicast::dataDir() / "clipboard.db", *windowManager, *appService);
    auto fontService = std::make_unique<FontService>();
    auto configService = std::make_unique<ConfigService>();
    auto shortcutService = std::make_unique<ShortcutService>(*omniDb.get());
    auto toastService = std::make_unique<ToastService>();
    auto currentConfig = configService->value();
    auto emojiService = std::make_unique<EmojiService>(*omniDb.get());
    auto calculatorService = std::make_unique<CalculatorService>(*omniDb.get());
    auto fileService = std::make_unique<FileService>(*omniDb);
    auto extensionRegistry = std::make_unique<ExtensionRegistry>(*localStorage);
    auto raycastStore = std::make_unique<RaycastStoreService>();
    auto vicinaeStore = std::make_unique<VicinaeStoreService>();

#ifdef HAS_TYPESCRIPT_EXTENSIONS
    if (!extensionManager->start()) {
      qCritical() << "Failed to load extension manager. Extensions will not work";
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
    registry->setWindowManager(std::move(windowManager));
    registry->setFontService(std::move(fontService));
    registry->setEmojiService(std::move(emojiService));
    registry->setRaycastStore(std::move(raycastStore));
    registry->setVicinaeStore(std::move(vicinaeStore));
    registry->setExtensionRegistry(std::move(extensionRegistry));
    registry->setOAuthService(std::make_unique<OAuthService>());
    registry->setPowerManager(std::make_unique<PowerManager>());

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
        if (!provider->isExtension()) continue;

        auto extp = static_cast<ExtensionRootProvider *>(provider);

        if (!extp->isBuiltin() && !scanned.contains(extp->repositoryId())) {
          removed.emplace_back(extp->uniqueId());
        }
      }

      for (const auto &id : removed) {
        root->uninstallProvider(id);
      }

      root->updateIndex();
    });

    QObject::connect(reg, &ExtensionRegistry::extensionUninstalled, [reg](const QString &id) {
      auto root = ServiceRegistry::instance()->rootItemManager();
      root->uninstallProvider(QString("extension.%1").arg(id));
      root->updateIndex();
    });

    for (const auto &manifest : reg->scanAll()) {
      auto extension = std::make_shared<Extension>(manifest);

      root->loadProvider(std::make_unique<ExtensionRootProvider>(extension));
    }

    // this one needs to be set last

    root->loadProvider(std::make_unique<AppRootProvider>(*registry->appDb()));
    root->loadProvider(std::make_unique<ShortcutRootProvider>(*registry->shortcuts()));

    // Force reload providers to make sure items that depend on them are shown
    root->updateIndex();
  }

  QPixmapCache::setCacheLimit(Environment::pixmapCacheLimit());
  FaviconService::initialize(new FaviconService(Omnicast::dataDir() / "favicon"));
  QApplication::setApplicationName("vicinae");
  QApplication::setQuitOnLastWindowClosed(false);
  ApplicationContext ctx;

  ctx.navigation = std::make_unique<NavigationController>(ctx);
  ctx.overlay = std::make_unique<OverlayController>(&ctx);
  ctx.settings = std::make_unique<SettingsController>();
  ctx.services = ServiceRegistry::instance();

  IpcCommandServer commandServer;

  commandServer.setHandler(new IpcCommandHandler(ctx));
  commandServer.start(Omnicast::commandSocketPath());

  auto configChanged = [&](const ConfigService::Value &next, const ConfigService::Value &prev) {
    auto &theme = ThemeService::instance();
    bool themeChangeRequired =
        next.theme.name && next.theme.name.value_or("") != prev.theme.name.value_or("");
    bool iconThemeChangeRequired =
        next.theme.iconTheme && next.theme.iconTheme.value_or("") != prev.theme.iconTheme.value_or("");
    IconThemeDatabase iconThemeDb;

    theme.setFontBasePointSize(next.font.baseSize);

    if (next.font.baseSize != prev.font.baseSize) {
      if (!themeChangeRequired) { theme.reloadCurrentTheme(); }
    }

    if (themeChangeRequired) { theme.setTheme(*next.theme.name); }

    ctx.navigation->setPopToRootOnClose(next.popToRootOnClose);
    ctx.navigation->setCloseOnFocusLoss(next.closeOnFocusLoss);

    FaviconService::instance()->setService(next.faviconService);

    if (next.theme.iconTheme) {
      QIcon::setThemeName(*next.theme.iconTheme);
    } else if (QIcon::themeName() == "hicolor") {
      QIcon::setThemeName(iconThemeDb.guessBestTheme());
    }

    if (next.font.normal && *next.font.normal != prev.font.normal.value_or("")) {
      QApplication::setFont(*next.font.normal);
      qApp->setStyleSheet(qApp->styleSheet());
    }
  };

  if (!Environment::isLayerShellEnabled() && Environment::isCosmicDesktop()) {
    qWarning() << "Vicinae doesn't use layer-shell on the Cosmic desktop as it is currently broken. See "
                  "https://github.com/pop-os/cosmic-comp/issues/1590. If you want to force enable it, you "
                  "can set USE_LAYER_SHELL=1 in your environment.";
  }

  auto cfgService = ServiceRegistry::instance()->config();

  QObject::connect(KeybindManager::instance(), &KeybindManager::keybindChanged, [cfgService]() {
    cfgService->updateConfig(
        [](ConfigService::Value &value) { value.keybinds = KeybindManager::instance()->toSerializedMap(); });
  });

  QObject::connect(cfgService, &ConfigService::configChanged, configChanged);
  QIcon::setFallbackSearchPaths(Environment::fallbackIconSearchPaths());

  configChanged(cfgService->value(), {});

  KeybindManager::instance()->fromSerializedMap(cfgService->value().keybinds);

  LauncherWindow launcher(ctx);
  SettingsWindow settings(&ctx);

  ctx.navigation->launch(std::make_shared<RootCommand>());

  if (m_open) {
    ctx.navigation->showWindow();
  } else {
    qInfo() << "Vicinae server successfully started. Call \"vicinae toggle\" to toggle the window";
  }

  qApp->exec();
}
