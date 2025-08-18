#include "command-controller.hpp"
#include "daemon/ipc-client.hpp"
#include "favicon/favicon-service.hpp"
#include "navigation-controller.hpp"
#include "ui/launcher-window/launcher-window.hpp"
#include <QStyleHints>
#include "common.hpp"
#include "ipc-command-server.hpp"
#include "ipc-command-handler.hpp"
#include "overlay-controller/overlay-controller.hpp"
#include "services/app-service/app-service.hpp"
#include "command-database.hpp"
#include "root-search/apps/app-root-provider.hpp"
#include "services/shortcut/shortcut-service.hpp"
#include <QApplication>
#include "services/calculator-service/calculator-service.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/config/config-service.hpp"
#include "font-service.hpp"
#include <QFontDatabase>
#include <QSurfaceFormat>
#include <memory>
#include "services/window-manager/window-manager.hpp"
#include <QtSql/QtSql>
#include "root-extension-manager.hpp"
#include <QXmlStreamReader>
#include <QtSql/qsqldatabase.h>
#include <arpa/inet.h>
#include <csignal>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <qapplication.h>
#include <qbuffer.h>
#include <qdebug.h>
#include <qfontdatabase.h>
#include <qlist.h>
#include <qlocalserver.h>
#include <qlocalsocket.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qprocess.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include "extension/manager/extension-manager.hpp"
#include "services/emoji-service/emoji-service.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "services/files-service/file-service.hpp"
#include "services/local-storage/local-storage-service.hpp"
#include "vicinae.hpp"
#include "process-manager-service.hpp"
#include "services/oauth/oauth-service.hpp"
#include "services/raycast/raycast-store.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "root-search/apps/app-root-provider.hpp"
#include "root-search/shortcuts/shortcut-root-provider.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "settings-controller/settings-controller.hpp"
#include "settings/settings-window.hpp"
#include "theme.hpp"
#include "utils/utils.hpp"
#include "log/message-handler.hpp"

int startDaemon() {
  std::filesystem::create_directories(Omnicast::runtimeDir());
  auto pidFile = Omnicast::pidFile();

  {
    std::ifstream ifs(pidFile);
    pid_t pid;

    if (ifs.is_open()) {
      ifs >> pid;

      if (kill(pid, SIGKILL) == 0) { qInfo() << "Killed existing vicinae instance with pid" << pid; }
    }
  }

  {
    std::ofstream ofs(pidFile);

    if (!ofs.is_open()) {
      qDebug() << "failed to open pid file for writing";
      return 1;
    }

    ofs << qApp->applicationPid();
  }

  {
    auto registry = ServiceRegistry::instance();
    auto omniDb = std::make_unique<OmniDatabase>(Omnicast::dataDir() / "vicinae.db");
    auto localStorage = std::make_unique<LocalStorageService>(*omniDb);
    auto rootItemManager = std::make_unique<RootItemManager>(*omniDb.get());
    auto commandDb = std::make_unique<OmniCommandDatabase>();
    auto extensionManager = std::make_unique<ExtensionManager>(*commandDb);
    auto windowManager = std::make_unique<WindowManager>();
    auto appService = std::make_unique<AppService>(*omniDb.get());
    auto clipboardManager =
        std::make_unique<ClipboardService>(Omnicast::dataDir() / "clipboard.db", *windowManager, *appService);
    auto processManager = std::make_unique<ProcessManagerService>();
    auto fontService = std::make_unique<FontService>();
    auto configService = std::make_unique<ConfigService>();
    auto shortcutService = std::make_unique<ShortcutService>(*omniDb.get());
    auto toastService = std::make_unique<ToastService>();
    auto currentConfig = configService->value();
    auto rootExtMan = std::make_unique<RootExtensionManager>(*rootItemManager.get(), *commandDb.get());
    auto emojiService = std::make_unique<EmojiService>(*omniDb.get());
    auto calculatorService = std::make_unique<CalculatorService>(*omniDb.get());
    auto fileService = std::make_unique<FileService>();
    auto extensionRegistry = std::make_unique<ExtensionRegistry>(*commandDb, *localStorage);
    auto raycastStore = std::make_unique<RaycastStoreService>();

    if (!extensionManager->start()) {
      qCritical() << "Failed to load extension manager. Extensions will not work";
    }

    fileService->indexer()->setEntrypoints({{.root = homeDir()}});
    fileService->indexer()->start();

    registry->setFileService(std::move(fileService));
    registry->setToastService(std::move(toastService));
    registry->setShortcutService(std::move(shortcutService));
    registry->setConfig(std::move(configService));
    registry->setRootItemManager(std::move(rootItemManager));
    registry->setCalculatorService(std::move(calculatorService));
    registry->setAppDb(std::move(appService));
    registry->setOmniDb(std::move(omniDb));
    registry->setCommandDb(std::move(commandDb));
    registry->setLocalStorage(std::move(localStorage));
    registry->setExtensionManager(std::move(extensionManager));
    registry->setClipman(std::move(clipboardManager));
    registry->setWindowManager(std::move(windowManager));
    registry->setFontService(std::move(fontService));
    registry->setEmojiService(std::move(emojiService));
    registry->setRaycastStore(std::move(raycastStore));
    registry->setExtensionRegistry(std::move(extensionRegistry));
    registry->setOAuthService(std::make_unique<OAuthService>());

    auto p = rootExtMan.get();

    registry->setRootExtMan(std::move(rootExtMan));

    p->start();

    auto builtinCommandDb = std::make_unique<CommandDatabase>();

    for (const auto &repo : builtinCommandDb->repositories()) {
      registry->commandDb()->registerRepository(repo);
    }

    auto reg = ServiceRegistry::instance()->extensionRegistry();

    QObject::connect(reg, &ExtensionRegistry::extensionsChanged, [reg]() {
      for (const auto &manifest : reg->scanAll()) {
        auto extension = std::make_shared<Extension>(manifest);

        ServiceRegistry::instance()->commandDb()->registerRepository(extension);
      }
    });

    QObject::connect(reg, &ExtensionRegistry::extensionUninstalled, [reg](const QString &id) {
      ServiceRegistry::instance()->commandDb()->removeRepository(id);
    });

    for (const auto &manifest : reg->scanAll()) {
      auto extension = std::make_shared<Extension>(manifest);

      ServiceRegistry::instance()->commandDb()->registerRepository(extension);
    }

    // this one needs to be set last

    registry->rootItemManager()->addProvider(std::make_unique<AppRootProvider>(*registry->appDb()));
    registry->rootItemManager()->addProvider(std::make_unique<ShortcutRootProvider>(*registry->shortcuts()));

    // Force reload providers to make sure items that depend on them are shown
    registry->rootItemManager()->reloadProviders();
  }

  FaviconService::initialize(new FaviconService(Omnicast::dataDir() / "favicon"));

  QApplication::setApplicationName("vicinae");
  QApplication::setQuitOnLastWindowClosed(false);

  ApplicationContext ctx;

  ctx.navigation = std::make_unique<NavigationController>(ctx);
  ctx.command = std::make_unique<CommandController>(&ctx);
  ctx.overlay = std::make_unique<OverlayController>(&ctx);
  ctx.settings = std::make_unique<SettingsController>();
  ctx.services = ServiceRegistry::instance();

  IpcCommandServer commandServer;

  commandServer.setHandler(new IpcCommandHandler(ctx));
  commandServer.start(Omnicast::commandSocketPath());

  QObject::connect(ServiceRegistry::instance()->config(), &ConfigService::configChanged,
                   [&ctx](const ConfigService::Value &next, const ConfigService::Value &prev) {
                     if (next.theme.name.value_or("") != prev.theme.name.value_or("")) {
                       ThemeService::instance().setTheme(*next.theme.name);
                     }

                     ctx.navigation->setPopToRootOnClose(next.popToRootOnClose);

                     FaviconService::instance()->setService(next.faviconService);

                     if (auto icon = next.theme.iconTheme) { QIcon::setThemeName(icon.value()); }

                     if (next.font.normal && *next.font.normal != prev.font.normal.value_or("")) {
                       QApplication::setFont(*next.font.normal);
                       qApp->setStyleSheet(qApp->styleSheet());
                     }
                   });

  SettingsWindow settings(&ctx);
  LauncherWindow launcher(ctx);

  qInfo() << "Vicinae server successfully started. Call vicinae without an argument to toggle the window";

  return qApp->exec();
}

int main(int argc, char **argv) {
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
  QApplication qapp(argc, argv);

  qInstallMessageHandler(coloredMessageHandler);

  if (qapp.arguments().size() == 2 && qapp.arguments().at(1) == "server") { return startDaemon(); }

  DaemonIpcClient daemonClient;

  if (!daemonClient.connect()) {
    qCritical() << "Could not connect to vicinae daemon: is vicinae running? You can spawn the vicinae "
                   "server by running the \"vicinae server\" command.";
    return 1;
  }

  if (argc == 1) {
    daemonClient.toggle();
    return 0;
  }

  QUrl url(argv[1]);

  if (url.isValid()) {
    daemonClient.passUrl(url);
    return 0;
  }
}
