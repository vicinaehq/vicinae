#include "config/config.hpp"
#include "daemon/ipc-client.hpp"
#include "environment.hpp"
#include <QStyleHints>
#include "extension/manager/extension-manager.hpp"
#include "favicon/favicon-service.hpp"
#include "font-service.hpp"
#include "icon-theme-db/icon-theme-db.hpp"
#include "ipc-command-handler.hpp"
#include "ipc-command-server.hpp"
#include "keyboard/keybind-manager.hpp"
#include "log/message-handler.hpp"
#include "overlay-controller/overlay-controller.hpp"
#include "extensions/root/root-command.hpp"
#include "root-search/apps/app-root-provider.hpp"
#include "root-search/extensions/extension-root-provider.hpp"
#include "root-search/shortcuts/shortcut-root-provider.hpp"
#include "service-registry.hpp"
#include "services/calculator-service/calculator-service.hpp"
#include "services/clipboard/clipboard-service.hpp"
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
#include "ui/launcher-window/launcher-window.hpp"
#include "utils.hpp"
#include "vicinae.hpp"
#include <filesystem>
#include <qapplication.h>
#include <signal.h>
#include <QString>
#include <qlockfile.h>
#include <qlogging.h>
#include <qpixmapcache.h>
#include <qstylefactory.h>
#include <system_error>
#include "lib/CLI11.hpp"
#include "server.hpp"

namespace fs = std::filesystem;

void CliServerCommand::setup(CLI::App *app) {
  app->add_flag("--open", m_open, "Open the main window once the server is started");
  app->add_flag("--replace", m_replace, "Replace the currently running instance if there is one");
  app->add_option("--config", m_config, "Path to the main config file")
      ->default_val(Omnicast::configDir() / "settings.json")
      ->check([](const std::string &path) {
        std::error_code ec;
        if (!fs::is_regular_file(path, ec)) { return "not a valid file"; }
        return "";
      });
}

void CliServerCommand::run(CLI::App *app) {
  using namespace std::chrono_literals;
  fs::path lockPath = Omnicast::runtimeDir() / "vicinae.lock";
  QLockFile lock(lockPath.c_str());

  qInstallMessageHandler(coloredMessageHandler);

  // we are dealing with a long running process so we dont want to use time
  // as an indicator that the lock file might be stale.
  lock.setStaleLockTime(0ms);

  if (!lock.tryLock()) {
    DaemonIpcClient client;
    bool isServerReachable = client.connect() && client.ping();

    if (!isServerReachable) {
      lock.removeStaleLockFile();
      lock.lock();
    } else {
      if (!m_replace) {
        qWarning()
            << "A server is already running. Pass --replace if you want to replace the existing instance.";
        exit(1);
      }

      qInfo() << "Killing existing vicinae server...";

      // if we fail to kill gracefully, force kill by sending SIGKILL
      if (!client.kill()) {
        QString _;
        qint64 pid = 0;

        if (!lock.getLockInfo(&pid, &_, &_)) {
          qCritical() << "Failed to get lock file info";
          exit(1);
        }

        if (kill(pid, SIGKILL) != 0) {
          qCritical() << "Failed to kill process with pid" << pid;
          exit(1);
        }

        lock.removeStaleLockFile();
      }

      qInfo() << "Waiting for lock to released...";
      lock.lock();
    }
  }

  qInfo() << "Initializing vicinae server...";

  if (auto launcher = Environment::detectAppLauncher()) {
    qInfo() << "Detected launch prefix:" << *launcher;
  }

  int argc = 1;
  static char *argv[] = {strdup("command"), nullptr};
  QApplication qapp(argc, argv);

  // discard system specific qt theming
  qapp.setStyle(QStyleFactory::create("fusion"));

  std::filesystem::create_directories(Omnicast::runtimeDir());

  {
    auto registry = ServiceRegistry::instance();
    auto omniDb = std::make_unique<OmniDatabase>(Omnicast::dataDir() / "vicinae.db");
    auto localStorage = std::make_unique<LocalStorageService>(*omniDb);
    auto extensionManager = std::make_unique<ExtensionManager>();
    auto windowManager = std::make_unique<WindowManager>();
    auto appService = std::make_unique<AppService>(*omniDb.get());
    auto clipboardManager =
        std::make_unique<ClipboardService>(Omnicast::dataDir() / "clipboard.db", *windowManager, *appService);
    auto fontService = std::make_unique<FontService>();
    auto configService = std::make_unique<config::Manager>(m_config);
    auto rootItemManager = std::make_unique<RootItemManager>(*configService, *localStorage);
    auto shortcutService = std::make_unique<ShortcutService>(*omniDb.get());
    auto toastService = std::make_unique<ToastService>();
    auto currentConfig = configService->value();
    auto emojiService = std::make_unique<EmojiService>(*omniDb.get());
    auto calculatorService = std::make_unique<CalculatorService>(*omniDb.get());
    auto fileService = std::make_unique<FileService>(*omniDb);
    auto oauthService = std::make_unique<OAuthService>(*omniDb);
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
    registry->setOAuthService(std::move(oauthService));
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

        if (!extp->isBuiltin() && !scanned.contains(extp->uniqueId())) {
          removed.emplace_back(extp->uniqueId());
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
  ctx.settings = std::make_unique<SettingsController>(ctx);
  ctx.services = ServiceRegistry::instance();

  IpcCommandServer commandServer;

  commandServer.setHandler(new IpcCommandHandler(ctx));
  commandServer.start(Omnicast::commandSocketPath());

  auto configChanged = [&](const config::ConfigValue &next, const config::ConfigValue &prev) {
    auto &theme = ThemeService::instance();
    auto nextTheme = next.systemTheme();
    auto prevTheme = prev.systemTheme();
    bool themeChangeRequired = nextTheme.name != prevTheme.name;

    bool iconThemeChangeRequired = nextTheme.iconTheme != prevTheme.iconTheme;
    IconThemeDatabase iconThemeDb;

    theme.setFontBasePointSize(next.font.normal.size);

    if (next.font.normal.size != prev.font.normal.size) {
      if (!themeChangeRequired) { theme.reloadCurrentTheme(); }
    }

    if (themeChangeRequired) { theme.setTheme(nextTheme.name.c_str()); }

    ctx.navigation->setPopToRootOnClose(next.popToRootOnClose);
    ctx.navigation->setCloseOnFocusLoss(next.closeOnFocusLoss);

    KeybindManager::instance()->mergeBinds({next.keybinds.begin(), next.keybinds.end()});
    FaviconService::instance()->setService(next.faviconService.c_str());

    if (nextTheme.iconTheme != "auto") {
      QIcon::setThemeName(nextTheme.iconTheme.c_str());
    } else if (QIcon::themeName() == "hicolor") {
      QIcon::setThemeName(iconThemeDb.guessBestTheme());
    }

    if (next.font.normal.family != prev.font.normal.family) {
      auto family = next.font.normal.family;
      if (family == "auto") {
        QApplication::setFont(QFont());
      } else {
        QApplication::setFont(QFont(family.c_str()));
      }

      qApp->setStyleSheet(qApp->styleSheet());
    }
  };

  if (!Environment::isLayerShellEnabled() && Environment::isCosmicDesktop()) {
    qWarning() << "Vicinae doesn't use layer-shell on the Cosmic desktop as it is currently broken. See "
                  "https://github.com/pop-os/cosmic-comp/issues/1590. If you want to force enable it, you "
                  "can set USE_LAYER_SHELL=1 in your environment.";
  }

  auto cfgService = ServiceRegistry::instance()->config();

  QObject::connect(cfgService, &config::Manager::configLoadingError, [&ctx](std::string_view message) {
    ctx.navigation->confirmAlert("Failed to load config", qStringFromStdView(message), []() {});
  });

  QObject::connect(QApplication::styleHints(), &QStyleHints::colorSchemeChanged, [&]() {
    IconThemeDatabase iconThemeDb;
    auto &value = cfgService->value();
    auto &theme = value.systemTheme();

    if (theme.iconTheme != "auto") {
      QIcon::setThemeName(theme.iconTheme.c_str());
    } else if (QIcon::themeName() == "hicolor") {
      QIcon::setThemeName(iconThemeDb.guessBestTheme());
    }

    ThemeService::instance().setTheme(theme.name.c_str());
    qApp->setStyle(QStyleFactory::create("fusion"));
    qApp->setStyleSheet(qApp->styleSheet());
  });

  QObject::connect(
      KeybindManager::instance(), &KeybindManager::keybindChanged,
      [cfgService](Keybind bind, const Keyboard::Shortcut &shortcut) {
        auto info = KeybindManager::instance()->findBoundInfo(shortcut);
        cfgService->mergeWithUser(
            {.keybinds = config::KeybindMap{{info->id.toStdString(), shortcut.toString().toStdString()}}});
      });

  QObject::connect(cfgService, &config::Manager::configChanged, configChanged);
  QIcon::setFallbackSearchPaths(Environment::fallbackIconSearchPaths());

  configChanged(cfgService->value(), {});

  // KeybindManager::instance()->fromSerializedMap(cfgService->value().keybinds);

  LauncherWindow launcher(ctx);

  ctx.navigation->launch(std::make_shared<RootCommand>());

  if (m_open) {
    ctx.navigation->showWindow();
  } else {
    qInfo() << "Vicinae server successfully started. Call \"vicinae toggle\" to toggle the window";
  }

  qApp->exec();
  // make sure child processes are terminated
  ctx.services->clipman()->clipboardServer()->stop();
  ctx.services->extensionManager()->stop();
  lock.unlock();
}
