#include "command-controller.hpp"
#include "daemon/ipc-client.hpp"
#include "favicon/favicon-service.hpp"
#include "ui/launcher-window/launcher-window.hpp"
#include <QStyleHints>
#include "common.hpp"
#include "ipc-command-server.hpp"
#include "ipc-command-handler.hpp"
#include "overlay-controller/overlay-controller.hpp"
#include "services/app-service/app-service.hpp"
#include "command-database.hpp"
#include "root-search/apps/app-root-provider.hpp"
#include "services/bookmark/bookmark-service.hpp"
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
#include "root-search/bookmarks/bookmark-root-provider.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "settings-controller/settings-controller.hpp"
#include "settings/settings-window.hpp"
#include "theme.hpp"
#include "utils/utils.hpp"

#ifdef WAYLAND_LAYER_SHELL
#include <LayerShellQt/window.h>
#include <LayerShellQt/shell.h>
#endif

void coloredMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  // ANSI color codes
  const char *BLACK = "\033[30m";
  const char *RED = "\033[31m";
  const char *GREEN = "\033[32m";
  const char *YELLOW = "\033[33m";
  const char *BLUE = "\033[34m";
  const char *MAGENTA = "\033[35m";
  const char *CYAN = "\033[36m";
  const char *WHITE = "\033[37m";
  const char *RESET = "\033[0m";

  QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
  QString contextInfo = "";

  if (context.file) {
    std::filesystem::path file(context.file);

    contextInfo = QString("(%1%2:%3%4)").arg(BLUE).arg(file.filename().c_str()).arg(context.line).arg(RESET);
  }

  QString color;
  QString levelName;

  switch (type) {
  case QtDebugMsg:
    color = CYAN;
    levelName = "debug";
    break;
  case QtInfoMsg:
    color = GREEN;
    levelName = "info ";
    break;
  case QtWarningMsg:
    color = YELLOW;
    levelName = "warn ";
    break;
  case QtCriticalMsg:
    color = RED;
    levelName = "error";
    break;
  case QtFatalMsg:
    color = MAGENTA;
    levelName = "fatal";
    break;
  }

  // Format: [time] LEVEL message (file:line)
  QString formattedMessage = QString("%1[%2] %3%4%5  -  %6 %7%8\n")
                                 .arg(WHITE)
                                 .arg(timestamp)
                                 .arg(color)
                                 .arg(levelName)
                                 .arg(RESET)
                                 .arg(msg)
                                 .arg(contextInfo)
                                 .arg(RESET);

  std::cerr << formattedMessage.toStdString();

  if (type == QtFatalMsg) { abort(); }
}

int startDaemon() {
  std::filesystem::create_directories(Omnicast::runtimeDir());
  auto pidFile = Omnicast::pidFile();

  {
    std::ifstream ifs(pidFile);
    pid_t pid;

    if (ifs.is_open()) {
      ifs >> pid;

      qDebug() << "Kill existing omnicast instance with pid" << pid;

      if (kill(pid, SIGKILL) < 0) {
        qDebug() << "Failed to kill existing omnicast instance with pid" << pid << strerror(errno);
      }
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
    auto omniDb = std::make_unique<OmniDatabase>(Omnicast::dataDir() / "omnicast.db");
    auto localStorage = std::make_unique<LocalStorageService>(*omniDb);
    auto rootItemManager = std::make_unique<RootItemManager>(*omniDb.get());
    auto commandDb = std::make_unique<OmniCommandDatabase>();
    auto extensionManager = std::make_unique<ExtensionManager>(*commandDb);
    auto clipboardManager = std::make_unique<ClipboardService>(Omnicast::dataDir() / "clipboard.db");
    auto processManager = std::make_unique<ProcessManagerService>();
    auto windowManager = std::make_unique<WindowManager>();
    auto fontService = std::make_unique<FontService>();
    auto appService = std::make_unique<AppService>(*omniDb.get());
    auto configService = std::make_unique<ConfigService>();
    auto bookmarkService = std::make_unique<BookmarkService>(*omniDb.get());
    auto toastService = std::make_unique<ToastService>();
    auto currentConfig = configService->value();
    auto rootExtMan = std::make_unique<RootExtensionManager>(*rootItemManager.get(), *commandDb.get());
    auto emojiService = std::make_unique<EmojiService>(*omniDb.get());
    auto calculatorService = std::make_unique<CalculatorService>(*omniDb.get());
    auto fileService = std::make_unique<FileService>();
    auto extensionRegistry = std::make_unique<ExtensionRegistry>(*commandDb);
    auto raycastStore = std::make_unique<RaycastStoreService>();

    if (auto name = currentConfig.theme.name) {
      if (!ThemeService::instance().setTheme(*name)) {
        qCritical() << "Could not set theme with id" << *name
                    << "as it does not exist. Falling back to default theme instead.";
        ThemeService::instance().setDefaultTheme();
      }
    } else {
      ThemeService::instance().setDefaultTheme();
    }

    if (!extensionManager->start()) {
      qCritical() << "Failed to load extension manager. Extensions will not work";
    }

    // fileService->indexer()->setEntrypoints({{.root = "/home/aurelle/Downloads"}});
    fileService->indexer()->setEntrypoints({{.root = homeDir()}});
    // fileService->indexer()->start();

    registry->setFileService(std::move(fileService));
    registry->setToastService(std::move(toastService));
    registry->setBookmarkService(std::move(bookmarkService));
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

    QObject::connect(reg, &ExtensionRegistry::extensionAdded, [reg](const QString &id) {
      for (const auto &manifest : reg->scanAll()) {
        auto extension = std::make_shared<Extension>(manifest);

        ServiceRegistry::instance()->commandDb()->registerRepository(extension);
      }
    });

    for (const auto &manifest : extensionRegistry->scanAll()) {
      auto extension = std::make_shared<Extension>(manifest);

      ServiceRegistry::instance()->commandDb()->registerRepository(extension);
    }

    // this one needs to be set last

    registry->rootItemManager()->addProvider(std::make_unique<AppRootProvider>(*registry->appDb()));
    registry->rootItemManager()->addProvider(std::make_unique<BookmarkRootProvider>(*registry->bookmarks()));
  }

  FaviconService::initialize(new FaviconService(Omnicast::dataDir() / "favicon"));

  /*
  app.createWinId();

#ifdef WAYLAND_LAYER_SHELL
  qDebug() << "Initializing layer shell surface";
  if (auto lshell = LayerShellQt::Window::get(app.windowHandle())) {
    lshell->setLayer(LayerShellQt::Window::LayerOverlay);
    lshell->setScope("omnicast");
    lshell->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityExclusive);
    lshell->setExclusiveZone(-1);
    lshell->setAnchors(LayerShellQt::Window::AnchorNone);
  } else {
    qCritical() << "Unable apply layer shell rules to main window: LayerShellQt::Window::get() returned null";
  }
#endif
  */

  QObject::connect(ServiceRegistry::instance()->config(), &ConfigService::configChanged,
                   [](const ConfigService::Value &next, const ConfigService::Value &prev) {
                     if (next.theme.name.value_or("") != prev.theme.name.value_or("")) {
                       ThemeService::instance().setTheme(*next.theme.name);
                     }

                     if (QIcon::themeName() == "hicolor") {
                       if (ThemeService::instance().theme().appearance == "light") {
                         QIcon::setThemeName("Reversal");
                       } else {
                         QIcon::setThemeName("Reversal-dark");
                       }
                     }

                     if (next.font.normal && *next.font.normal != prev.font.normal.value_or("")) {
                       QApplication::setFont(*next.font.normal);
                       qApp->setStyleSheet(qApp->styleSheet());
                     }
                   });

  int fontId = QFontDatabase::addApplicationFont(":assets/fonts/SF-Pro-Text-Regular.otf");
  fontId = QFontDatabase::addApplicationFont(":assets/fonts/SF-Pro-Text-Light.otf");
  fontId = QFontDatabase::addApplicationFont(":assets/fonts/SF-Pro-Text-Bold.otf");

  QFont font("SF Pro Text");

  font.setHintingPreference(QFont::HintingPreference::PreferNoHinting);

  QApplication::setFont(font);
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

  SettingsWindow settings(&ctx);
  LauncherWindow launcher(ctx);

  launcher.show();

  // Print it

  return qApp->exec();
}

int main(int argc, char **argv) {
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
  QApplication qapp(argc, argv);

  qInstallMessageHandler(coloredMessageHandler);

  if (qapp.arguments().size() == 2 && qapp.arguments().at(1) == "server") { return startDaemon(); }

  DaemonIpcClient daemonClient;

  daemonClient.connect();

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
