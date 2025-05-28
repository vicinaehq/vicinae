#include "app.hpp"
#include "base-view.hpp"
#include "command-builder.hpp"
#include "command-database.hpp"
#include "command-server.hpp"
#include <QGraphicsBlurEffect>
#include "clipboard/clipboard-server-factory.hpp"
#include "common.hpp"
#include "config-service.hpp"
#include "extension/missing-extension-preference-view.hpp"
#include "command.hpp"
#include "config.hpp"
#include "service-registry.hpp"
#include "wm/window-manager-factory.hpp"
#include "extension_manager.hpp"
#include "favicon/favicon-service.hpp"
#include "image-fetcher.hpp"
#include "omni-command-db.hpp"
#include "omnicast.hpp"
#include "root-command.hpp"
#include "theme.hpp"
#include <QLabel>
#include <QMainWindow>
#include <QThread>
#include <QVBoxLayout>
#include <csetjmp>
#include <memory>
#include <qboxlayout.h>
#include <qevent.h>
#include <qfuturewatcher.h>
#include <qgraphicseffect.h>
#include <qlogging.h>
#include <qmainwindow.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpixmapcache.h>
#include <qtmetamacros.h>
#include <qwidget.h>

bool AppWindow::event(QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);
    auto key = keyEvent->key();
    bool isEsc = keyEvent->key() == Qt::Key_Escape;

    if (navigationStack.size() > 1) {
      if (isEsc || (keyEvent->key() == Qt::Key_Backspace)) {
        popCurrentView();
        return true;
      }
    }

    if (isEsc) {
      qDebug() << "esc";

      hide();
      return true;
    }

    return true;
  }

  return QWidget::event(event);
}

void AppWindow::clearSearch() {}

void AppWindow::showEvent(QShowEvent *event) { QMainWindow::showEvent(event); }

void AppWindow::popCurrentView() {
  auto &activeCommand = commandStack.at(commandStack.size() - 1);

  if (activeCommand.viewStack.empty()) return;

  if (navigationStack.size() == 1) return;

  auto previous = navigationStack.top();
  navigationStack.pop();

  previous.view->deactivate();
  disconnectView(*previous.view);

  auto next = navigationStack.top();

  connectView(*next.view);
  next.view->activate();
  next.view->setFixedSize(size());
  ServiceRegistry::instance()->UI()->setTopView(next.view);
  next.view->show();

  previous.view->deleteLater();

  if (activeCommand.viewStack.size() == 1) {
    activeCommand.command->unload();
    activeCommand.command->deleteLater();
    commandStack.pop_back();
    qDebug() << "popping cmd stack now" << commandStack.size();
  } else {
    activeCommand.viewStack.pop();
  }

  currentViewPoped();
}

void AppWindow::popToRoot() {
  while (navigationStack.size() > 1) {
    popCurrentView();
  }
}

void AppWindow::disconnectView(BaseView &view) { view.removeEventFilter(this); }

void AppWindow::connectView(BaseView &view) { view.installEventFilter(this); }

void AppWindow::pushView(BaseView *view, const PushViewOptions &opts) {
  if (commandStack.empty()) {
    qDebug() << "AppWindow::pushView called with empty command stack";
    return;
  }

  auto &currentCommand = commandStack.at(commandStack.size() - 1);

  if (navigationStack.size() > 0) {
    auto &old = navigationStack.top();

    disconnectView(*old.view);
  }

  if (navigationStack.size() > 0) { navigationStack.top().view->hide(); }

  connectView(*view);
  view->setParent(this);
  view->setFixedSize(size());

  currentCommand.viewStack.push({.view = view});
  ServiceRegistry::instance()->UI()->setTopView(view);
  navigationStack.push({.view = view});
  if (auto navigation = opts.navigation) {
    view->setNavigationTitle(navigation->title);
    view->setNavigationIcon(navigation->iconUrl);
  }

  view->show();
  view->initialize();
  view->setSearchText(opts.searchQuery);
  view->onActivate();
}

void AppWindow::unloadCurrentCommand() { popToRoot(); }

void AppWindow::unloadHangingCommand() {
  if (commandStack.size() > 1) {
    auto &command = commandStack.at(commandStack.size() - 1);

    if (command.viewStack.empty()) {
      command.command->unload();
      commandStack.pop_back();
      qWarning() << "unloading hanging command";
    }
  }
}

void AppWindow::launchCommand(const std::shared_ptr<AbstractCmd> &command, const LaunchCommandOptions &opts,
                              const LaunchProps &props) {

  auto commandDb = ServiceRegistry::instance()->commandDb();
  auto preferenceValues = commandDb->getPreferenceValues(command->uniqueId());

  for (const auto &preference : command->preferences()) {
    if (preference->isRequired() && !preferenceValues.contains(preference->name())) {
      if (command->type() == CommandType::CommandTypeExtension) {
        auto extensionCommand = std::static_pointer_cast<ExtensionCommand>(command);

        /*
pushView(new MissingExtensionPreferenceView(*this, extensionCommand),
         {.navigation = NavigationStatus{.title = command->name(), .iconUrl = command->iconUrl()}});
        */
        return;
      }

      qDebug() << "MISSING PREFERENCE" << preference->title();
    }
  }

  qDebug() << "preference values for command with" << command->preferences().size() << "preferences"
           << command->uniqueId() << preferenceValues;

  unloadHangingCommand();

  auto ctx = command->createContext(command);

  if (!ctx) { return; }

  if (command->isNoView() && command->type() == CommandType::CommandTypeBuiltin) {
    qCritical() << "Running no view command";
    ctx->load(props);
  } else {
    commandStack.push_back({.command = std::unique_ptr<CommandContext>(ctx)});
    ctx->load(props);
  }
}

void AppWindow::launchCommand(const QString &id, const LaunchCommandOptions &opts) {
  auto commandDb = ServiceRegistry::instance()->commandDb();

  if (auto command = commandDb->findCommand(id)) { launchCommand(command->command, opts); }
}

void AppWindow::resizeEvent(QResizeEvent *event) {
  QMainWindow::resizeEvent(event);
  if (!navigationStack.empty()) { navigationStack.top().view->setFixedSize(event->size()); }
}

void AppWindow::paintEvent(QPaintEvent *event) {
  auto &config = ServiceRegistry::instance()->config()->value();
  auto &theme = ThemeService::instance().theme();
  int borderWidth = 1;
  QColor finalBgColor = theme.colors.mainBackground;
  QPainter painter(this);

  finalBgColor.setAlphaF(config.window.opacity);

  painter.setRenderHint(QPainter::Antialiasing, true);

  QPainterPath path;
  path.addRoundedRect(rect(), config.window.rounding, config.window.rounding);

  painter.setClipPath(path);

  painter.fillPath(path, finalBgColor);

  QPen pen(theme.colors.border, borderWidth);
  painter.setPen(pen);

  painter.drawPath(path);
}

std::variant<CommandResponse, CommandError> AppWindow::handleCommand(const CommandMessage &message) {
  qDebug() << "received message type" << message.type;
  auto commandDb = ServiceRegistry::instance()->commandDb();
  auto extensionManager = ServiceRegistry::instance()->extensionManager();

  if (message.type == "ping") { return "pong"; }
  if (message.type == "toggle") {
    setVisible(!isVisible());
    return true;
  }

  if (message.type == "url-scheme-handler") {
    QUrl url(message.params.asString().c_str());

    if (url.path() == "/api/extensions/develop/start") {
      QUrlQuery query(url.query());
      QString id = query.queryItemValue("id");

      extensionManager->startDevelopmentSession(id);
      qDebug() << "start develop id" << query.queryItemValue("id");
    }

    else if (url.path() == "/api/extensions/develop/refresh") {
      QUrlQuery query(url.query());
      QString id = query.queryItemValue("id");

      extensionManager->refreshDevelopmentSession(id);
      qDebug() << "refresh develop id" << id;
    }

    qDebug() << "handling URL in daemon" << url.toString();
    return {};
  }

  if (message.type == "command.list") {
    Proto::Array results;

    for (const auto &entry : commandDb->commands()) {
      Proto::Dict result;

      result["id"] = entry.command->uniqueId().toUtf8().constData();
      result["name"] = entry.command->name().toUtf8().constData();
      results.push_back(result);
    }

    return results;
  }

  if (message.type == "command.push") {
    auto args = message.params.asArray();

    if (args.empty()) { return CommandError{"Ill-formed command.push request"}; }

    auto id = args.at(0).asString();

    return CommandError{"No such command"};
  }

  return CommandError{"Unknowm command"};
}

void AppWindow::closeWindow(bool withPopToRoot) {
  hide();
  if (withPopToRoot) popToRoot();
}

AppWindow::AppWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground, true);
  setMinimumSize(Omnicast::WINDOW_SIZE);

  QDir::root().mkpath(Config::dirPath());
  FaviconService::initialize(new FaviconService(Config::dirPath() + QDir::separator() + "favicon.db"));

  _commandServer = new CommandServer(this);

  if (!_commandServer->start(Omnicast::commandSocketPath())) {
    qDebug() << "could not start the command server";
    return;
  }

  _commandServer->setHandler(this);

  ImageFetcher::instance();

  connect(ServiceRegistry::instance()->config(), &ConfigService::configChanged, this,
          [](const ConfigService::Value &next, const ConfigService::Value &prev) {
            if (next.theme.name.value_or("") != prev.theme.name.value_or("")) {
              ThemeService::instance().setTheme(*next.theme.name);
            }

            if (next.font.normal && *next.font.normal != prev.font.normal.value_or("")) {
              QApplication::setFont(*next.font.normal);
              qApp->setStyleSheet(qApp->styleSheet());
            }
          });

  auto rootCommand =
      CommandBuilder("root").withIcon(BuiltinOmniIconUrl("omnicast")).toSingleView<RootCommandV2>();

  connect(ServiceRegistry::instance()->UI(), &UIController::popToRootRequested, this, [this]() {
    popToRoot();
    navigationStack.top().view->clearSearchBar();
  });
  connect(ServiceRegistry::instance()->UI(), &UIController::launchCommandRequested, this,
          [this](const auto &cmd) { launchCommand(cmd, {}, {}); });
  connect(ServiceRegistry::instance()->UI(), &UIController::popViewRequested, this,
          [this]() { popCurrentView(); });
  connect(ServiceRegistry::instance()->UI(), &UIController::pushViewRequested, this,
          [this](BaseView *view, const PushViewOptions &opts) { pushView(view, opts); });
  connect(ServiceRegistry::instance()->UI(), &UIController::closeWindowRequested, this,
          [this]() { closeWindow(false); });

  launchCommand(rootCommand);
}
