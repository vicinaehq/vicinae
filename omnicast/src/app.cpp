#include "app.hpp"
#include "app/xdg-app-database.hpp"
#include "clipboard/clipboard-service.hpp"
#include "command-database.hpp"
#include "command-server.hpp"
#include <QGraphicsBlurEffect>
#include "clipboard/clipboard-server-factory.hpp"
#include "command.hpp"
#include "config.hpp"
#include "extension_manager.hpp"
#include "favicon/favicon-service.hpp"
#include "image-fetcher.hpp"
#include "process-manager-service.hpp"
#include "quicklink-seeder.hpp"
#include "root-command.hpp"
#include "ui/action-pannel/action-section.hpp"
#include "ui/action_popover.hpp"
#include "theme.hpp"
#include "ui/top_bar.hpp"
#include <QLabel>
#include <QMainWindow>
#include <QThread>
#include "clipboard/clipboard-server.hpp"
#include <QVBoxLayout>
#include <memory>
#include <qboxlayout.h>
#include <qevent.h>
#include <qgraphicseffect.h>
#include <qlogging.h>
#include <qmainwindow.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>

bool AppWindow::event(QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);
    auto key = keyEvent->key();
    bool isEsc = keyEvent->key() == Qt::Key_Escape;

    if (navigationStack.size() > 1) {
      if (isEsc || (keyEvent->key() == Qt::Key_Backspace && topBar->input->text().isEmpty())) {
        popCurrentView();
        return true;
      }
    }

    if (isEsc) {
      qDebug() << "esc";
      if (topBar->input->text().isEmpty()) {
        hide();
      } else {
        qDebug() << "reset text";
        clearSearch();
      }
      return true;
    }

    if (auto action = actionPannel->findBoundAction(keyEvent)) {
      executeAction(action);
      return true;
    }

    if (keyEvent->modifiers().testFlag(Qt::ControlModifier) && key == Qt::Key_B) {
      actionPannel->showActions();

      return true;
    }

    // QApplication::sendEvent(navigationStack.top().view, event);
    return true;
  }

  return QWidget::event(event);
}

void AppWindow::clearSearch() {
  topBar->input->clear();
  emit topBar->input->textEdited("");
}

void AppWindow::popCurrentView() {
  if (commandStack.isEmpty()) {
    qDebug() << "AppWindow::popCurrentView: commandStack is empty";
    return;
  }

  auto &activeCommand = commandStack.top();

  if (activeCommand.viewStack.isEmpty()) return;

  if (navigationStack.size() == 1) return;

  auto previous = navigationStack.top();
  navigationStack.pop();

  disconnectView(*previous.view);
  previous.view->deleteLater();

  auto next = navigationStack.top();

  previous.view->widget->setParent(nullptr);
  previous.view->setParent(nullptr);

  connectView(*next.view);
  next.view->setParent(this);
  viewDisplayer->setWidget(next.view->widget);
  next.view->widget->show();

  previous.view->deleteLater();

  actionPannel->restoreViewStack(next.actionViewStack);

  if (auto action = actionPannel->primaryAction()) {
    statusBar->setAction(*action);
  } else {
    statusBar->clearAction();
  }

  topBar->destroyQuicklinkCompleter();
  topBar->input->setReadOnly(false);
  topBar->input->show();
  topBar->input->setFocus();
  topBar->input->setText(next.query);
  topBar->input->setPlaceholderText(next.placeholderText);
  topBar->input->selectAll();

  if (next.completer) { topBar->activateQuicklinkCompleter(*next.completer); }

  if (navigationStack.size() == 1) { topBar->hideBackButton(); }

  QTimer::singleShot(0, [next]() { next.view->onRestore(); });

  qDebug() << "view stack size" << activeCommand.viewStack.size();

  if (activeCommand.viewStack.size() == 1) {
    activeCommand.command->unload(*this);
    activeCommand.command->deleteLater();
    commandStack.pop();
    qDebug() << "popping cmd stack now" << commandStack.size();
  } else {
    activeCommand.viewStack.pop();
  }

  if (navigationStack.size() == 1) { statusBar->reset(); }

  emit currentViewPoped();
}

void AppWindow::popToRootView() {
  while (navigationStack.size() > 1) {
    popCurrentView();
  }
}

void AppWindow::disconnectView(View &view) {
  disconnect(topBar->input, &SearchBar::debouncedTextEdited, &view, &View::onSearchChanged);

  // view->app
  disconnect(&view, &View::pushView, this, &AppWindow::pushView);
  disconnect(&view, &View::pop, this, &AppWindow::popCurrentView);
  disconnect(&view, &View::popToRoot, this, &AppWindow::popToRootView);
  disconnect(&view, &View::launchCommand, this, &AppWindow::launchCommand);
  disconnect(&view, &View::activatePrimaryAction, this, &AppWindow::selectPrimaryAction);

  view.removeEventFilter(this);
  topBar->input->removeEventFilter(&view);
}

void AppWindow::connectView(View &view) {
  // app->view
  connect(topBar->input, &SearchBar::debouncedTextEdited, &view, &View::onSearchChanged);

  // view->app
  connect(&view, &View::pushView, this, &AppWindow::pushView);
  connect(&view, &View::pop, this, &AppWindow::popCurrentView);
  connect(&view, &View::popToRoot, this, &AppWindow::popToRootView);
  connect(&view, &View::launchCommand, this, &AppWindow::launchCommand);
  connect(&view, &View::activatePrimaryAction, this, &AppWindow::selectPrimaryAction);

  view.onAttach();
  view.installEventFilter(this);
  topBar->input->installEventFilter(&view);
}

void AppWindow::pushView(View *view, const PushViewOptions &opts) {
  if (commandStack.empty()) {
    qDebug() << "AppWindow::pushView called with empty command stack";
    return;
  }

  auto &currentCommand = commandStack.top();

  if (navigationStack.size() > 0) {
    auto &old = navigationStack.top();

    disconnectView(*old.view);
  }

  if (navigationStack.size() == 1) { topBar->showBackButton(); }

  if (navigationStack.empty()) {
    viewDisplayer->setWidget(view->widget);
  } else {
    auto &cur = navigationStack.top();

    cur.query = topBar->input->text();
    cur.placeholderText = topBar->input->placeholderText();
    cur.actionViewStack = actionPannel->takeViewStack();
    cur.completer.reset();

    if (topBar->quickInput && topBar->completerData) {
      cur.completer = CompleterData{
          .placeholders = topBar->completerData->placeholders,
          .values = topBar->quickInput->collectArgs(),
          .iconUrl = topBar->completerData->iconUrl,
      };
    }

    cur.view->widget->setParent(nullptr);
    cur.view->widget->hide();
    viewDisplayer->setWidget(view->widget);

    if (opts.navigation) statusBar->setNavigation(opts.navigation->title, opts.navigation->iconUrl);
  }

  connectView(*view);

  actionPannel->setSignalActions({});
  statusBar->clearAction();
  currentCommand.viewStack.push({.view = view});
  navigationStack.push({.view = view});

  topBar->destroyQuicklinkCompleter();
  topBar->input->setReadOnly(false);
  topBar->input->show();
  topBar->input->setFocus();
  topBar->input->setText(opts.searchQuery);
  emit topBar->input->textEdited(opts.searchQuery);
  qDebug() << "view pushed";

  QTimer::singleShot(0, [view]() { view->onMount(); });
}

void AppWindow::launchCommand(ViewCommand *cmd, const LaunchCommandOptions &opts) {
  commandStack.push({.command = cmd});

  auto view = cmd->load(*this);

  if (view) { pushView(view, {.searchQuery = opts.searchQuery, .navigation = opts.navigation}); }
}

void AppWindow::resizeEvent(QResizeEvent *event) { QMainWindow::resizeEvent(event); }

void AppWindow::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  int borderRadius = 10;
  int borderWidth = 1;
  QColor finalBgColor = theme.colors.mainBackground;
  QPainter painter(this);

  finalBgColor.setAlphaF(0.98);

  painter.setRenderHint(QPainter::Antialiasing, true);

  QPainterPath path;
  path.addRoundedRect(rect(), borderRadius, borderRadius);

  painter.setClipPath(path);

  painter.fillPath(path, finalBgColor);

  QPen pen(theme.colors.border, borderWidth); // Border with a thickness of 2
  painter.setPen(pen);

  painter.drawPath(path);
}

std::variant<CommandResponse, CommandError> AppWindow::handleCommand(const CommandMessage &message) {
  if (message.type == "ping") { return "pong"; }
  if (message.type == "toggle") {
    setVisible(!isVisible());
    return true;
  }

  if (message.type == "clipboard.store") {
    auto values = message.params.asArray();

    if (values.size() != 2) { return CommandError{"Ill-formed clipboard.store request"}; }

    auto data = values[0].asString();
    auto options = values[1].asDict();
    auto mimeName = options["mime"].asString();
    // auto copied = clipboardService->copy(QByteArray(data.data(), data.size()));

    return {};
  }

  if (message.type == "command.list") {
    Proto::Array results;

    for (const auto &cmd : commandDb->list()) {
      Proto::Dict result;

      result["id"] = cmd.id.toUtf8().constData();
      result["name"] = cmd.name.toUtf8().constData();
      results.push_back(result);
    }

    return results;
  }

  if (message.type == "command.push") {
    auto args = message.params.asArray();

    if (args.empty()) { return CommandError{"Ill-formed command.push request"}; }

    auto id = args.at(0).asString();

    if (auto cmd = commandDb->findById(id.c_str())) {
      emit launchCommand(cmd->factory(*this, ""),
                         {.navigation = NavigationStatus{.title = cmd->name, .iconUrl = cmd->iconUrl}});
      setVisible(true);

      return true;
    }

    return CommandError{"No such command"};
  }

  return CommandError{"Unknowm command"};
}

void AppWindow::selectPrimaryAction() {
  if (auto action = actionPannel->primaryAction()) { executeAction(action); }

  // if (auto first = actionPannel->actionnable(0)) { executeAction(first); }
}

void AppWindow::selectSecondaryAction() {
  // if (auto second = actionPannel->actionnable(1)) { executeAction(second); }
}

void AppWindow::executeAction(AbstractAction *action) {
  auto executor = commandStack.top().viewStack.top().view;

  if (!action->isPushView()) { actionPannel->close(); }

  action->execute(*this);

  if (!action->isPushView()) {
    emit action->didExecute();
    executor->onActionActivated(action);

    if (auto cb = action->executionCallback()) { cb(); }
  }
}

void AppWindow::closeWindow(bool withPopToRoot) {
  hide();
  clearSearch();
  topBar->destroyQuicklinkCompleter();

  if (withPopToRoot) popToRootView();
}

AppWindow::AppWindow(QWidget *parent)
    : QMainWindow(parent), topBar(new TopBar()), viewDisplayer(new ViewDisplayer), statusBar(new StatusBar()),
      actionPannel(new ActionPannelWidget(this)) {
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground, true);

  setMinimumWidth(850);
  setMinimumHeight(550);

  topBar->setFixedHeight(55);

  QDir::root().mkpath(Config::dirPath());
  ThemeService::instance().setTheme("Ayu Mirage");
  FaviconService::initialize(new FaviconService(Config::dirPath() + QDir::separator() + "favicon.db"));

  extensionManager = std::make_unique<ExtensionManager>();

  quicklinkDatabase =
      std::make_unique<QuicklistDatabase>(Config::dirPath() + QDir::separator() + "quicklinks.db");
  calculatorDatabase =
      std::make_unique<CalculatorDatabase>(Config::dirPath() + QDir::separator() + "calculator.db");
  appDb = std::make_unique<XdgAppDatabase>();
  clipboardService =
      std::make_unique<ClipboardService>(Config::dirPath() + QDir::separator() + "clipboard.db");
  // indexer = std::make_unique<IndexerService>(Config::dirPath() + QDir::separator() + "files.db");
  processManagerService = std::make_unique<ProcessManagerService>();
  commandDb = std::make_unique<CommandDatabase>();

  _commandServer = new CommandServer(this);
  QString socketPath = QDir::temp().absoluteFilePath("omnicast.sock");

  if (!_commandServer->start(socketPath)) {
    qDebug() << "could not start the command server";
    return;
  }

  _commandServer->setHandler(this);

  connect(topBar->input, &SearchBar::pop, this, &AppWindow::popCurrentView);
  connect(actionPannel, &ActionPannelWidget::actionExecuted, this, &AppWindow::executeAction);
  connect(statusBar, &StatusBar::actionButtonClicked, this, [this]() { actionPannel->showActions(); });
  connect(statusBar, &StatusBar::currentActionButtonClicked, this, [this]() { selectPrimaryAction(); });
  connect(actionPannel, &ActionPannelWidget::closed, this,
          [this]() { statusBar->setActionButtonHighlight(false); });
  connect(actionPannel, &ActionPannelWidget::opened, this,
          [this]() { statusBar->setActionButtonHighlight(true); });

  ImageFetcher::instance();

  {
    auto seeder = std::make_unique<QuickLinkSeeder>(*appDb, *quicklinkDatabase);

    if (quicklinkDatabase->list().isEmpty()) { seeder->seed(); }
  }

  AbstractClipboardServer *clipboardServer = ClipboardServerFactory().createFirstActivatable(this);

  connect(clipboardServer, &AbstractClipboardServer::selection, clipboardService.get(),
          &ClipboardService::saveSelection);

  clipboardServer->start();

  extensionManager->start();

  layout = new QVBoxLayout();

  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);

  layout->setAlignment(Qt::AlignTop);
  topBar->input->installEventFilter(this);
  layout->addWidget(topBar);
  layout->addWidget(new HDivider);
  layout->addWidget(viewDisplayer, 1);
  layout->addWidget(new HDivider);
  layout->addWidget(statusBar);

  // commandStack.push(index);

  auto widget = new QWidget();

  widget->setLayout(layout);

  setCentralWidget(widget);

  launchCommand(new RootCommand);
}

template <> Service<QuicklistDatabase> AppWindow::service<QuicklistDatabase>() const {
  return *quicklinkDatabase;
}

/*
template <> Service<IndexerService> AppWindow::service<IndexerService>() const { return *indexer; }
*/

template <> Service<CalculatorDatabase> AppWindow::service<CalculatorDatabase>() const {
  return *calculatorDatabase;
}

template <> Service<AbstractAppDatabase> AppWindow::service<AbstractAppDatabase>() const { return *appDb; }

template <> Service<ClipboardService> AppWindow::service<ClipboardService>() const {
  return *clipboardService;
}

template <> Service<ExtensionManager> AppWindow::service<ExtensionManager>() const {
  return *extensionManager;
}

template <> Service<ProcessManagerService> AppWindow::service<ProcessManagerService>() const {
  return *processManagerService;
}

template <> Service<CommandDatabase> AppWindow::service<CommandDatabase>() const { return *commandDb; }
