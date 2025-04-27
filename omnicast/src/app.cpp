#include "app.hpp"
#include "ai/ollama-ai-provider.hpp"
#include "app/xdg-app-database.hpp"
#include "clipboard/clipboard-service.hpp"
#include "command-builder.hpp"
#include "command-database.hpp"
#include "command-server.hpp"
#include <QGraphicsBlurEffect>
#include "clipboard/clipboard-server-factory.hpp"
#include "extension/missing-extension-preference-view.hpp"
#include "command.hpp"
#include "config.hpp"
#include "extension/extension.hpp"
#include "extension_manager.hpp"
#include "favicon/favicon-service.hpp"
#include "image-fetcher.hpp"
#include "omni-command-db.hpp"
#include "omni-database.hpp"
#include "omnicast.hpp"
#include "process-manager-service.hpp"
#include "quicklink-seeder.hpp"
#include "root-command.hpp"
#include "theme.hpp"
#include "ui/dialog.hpp"
#include "ui/horizontal-loading-bar.hpp"
#include "ui/toast.hpp"
#include "ui/top_bar.hpp"
#include <QLabel>
#include <QMainWindow>
#include <QThread>
#include "clipboard/clipboard-server.hpp"
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

  connectView(*next.view);
  centerView = next.view;
  next.view->setGeometry(viewGeometry());
  next.view->show();

  previous.view->deleteLater();

  qDebug() << "restored actions" << next.actionViewStack.size();
  actionPannel->restoreViewStack(next.actionViewStack);

  if (auto action = actionPannel->primaryAction()) {
    statusBar->setAction(*action);
  } else {
    statusBar->clearAction();
  }

  topBar->destroyQuicklinkCompleter();
  _loadingBar->setStarted(false);
  topBar->input->setReadOnly(false);
  topBar->input->show();
  topBar->input->setFocus();
  topBar->input->setText(next.query);
  topBar->input->setPlaceholderText(next.placeholderText);

  if (QWidget *accessory = topBar->accessoryWidget(); accessory && accessory != next.searchAccessory) {
    accessory->deleteLater();
  }

  qDebug() << "setting accessory widget to" << next.searchAccessory;
  topBar->setAccessoryWidget(next.searchAccessory);
  topBar->input->selectAll();

  if (next.completer) { topBar->activateQuicklinkCompleter(*next.completer); }

  if (navigationStack.size() == 1) { topBar->hideBackButton(); }

  next.view->onRestore();

  qDebug() << "view stack size" << activeCommand.viewStack.size();

  if (activeCommand.viewStack.size() == 1) {
    activeCommand.command->unload();
    activeCommand.command->deleteLater();
    commandStack.pop();
    qDebug() << "popping cmd stack now" << commandStack.size();
  } else {
    activeCommand.viewStack.pop();
  }

  if (navigationStack.size() == 1) {
    statusBar->reset();
  } else {
    statusBar->setNavigationTitle(next.navigation.title);
    statusBar->setNavigationIcon(next.navigation.icon);
  }

  emit currentViewPoped();
}

void AppWindow::popToRoot() {
  while (navigationStack.size() > 1) {
    popCurrentView();
  }
}

void AppWindow::disconnectView(View &view) {
  disconnect(topBar->input, &SearchBar::textEdited, &view, &View::onSearchChanged);

  // view->app
  disconnect(&view, &View::pushView, this, &AppWindow::pushView);
  disconnect(&view, &View::pop, this, &AppWindow::popCurrentView);
  disconnect(&view, &View::popToRoot, this, &AppWindow::popToRoot);
  disconnect(&view, &View::activatePrimaryAction, this, &AppWindow::selectPrimaryAction);

  view.removeEventFilter(this);
  topBar->input->removeEventFilter(&view);
}

void AppWindow::connectView(View &view) {
  // app->view
  connect(topBar->input, &SearchBar::textEdited, &view, &View::onSearchChanged);

  // view->app
  connect(&view, &View::pushView, this, &AppWindow::pushView);
  connect(&view, &View::pop, this, &AppWindow::popCurrentView);
  connect(&view, &View::popToRoot, this, &AppWindow::popToRoot);
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

  view->setGeometry(viewGeometry());
  view->lower();

  if (!navigationStack.empty()) {
    auto &cur = navigationStack.top();

    cur.query = topBar->input->text();
    cur.placeholderText = topBar->input->placeholderText();
    cur.actionViewStack = actionPannel->takeViewStack();
    cur.searchAccessory = topBar->accessoryWidget();
    cur.navigation = {
        .icon = statusBar->navigationIcon(),
        .title = statusBar->navigationTitle(),
    };

    topBar->accessoryWidget()->hide();

    qDebug() << "pushed " << cur.actionViewStack.size() << "actions";
    cur.completer.reset();

    if (topBar->quickInput && topBar->completerData) {
      cur.completer = CompleterData{
          .placeholders = topBar->completerData->placeholders,
          .values = topBar->quickInput->collectArgs(),
          .iconUrl = topBar->completerData->iconUrl,
      };
    }

    cur.view->hide();

    if (opts.navigation) statusBar->setNavigation(opts.navigation->title, opts.navigation->iconUrl);
  }

  centerView = view;
  view->show();

  connectView(*view);

  actionPannel->setSignalActions({});
  statusBar->clearAction();
  currentCommand.viewStack.push({.view = view});
  navigationStack.push({.view = view});
  _loadingBar->setStarted(false);

  topBar->destroyQuicklinkCompleter();
  topBar->input->setReadOnly(false);
  topBar->input->show();
  topBar->input->setFocus();
  topBar->input->setText(opts.searchQuery);
  emit topBar->input->textEdited(opts.searchQuery);

  view->onMount();
}

void AppWindow::unloadCurrentCommand() { popToRoot(); }

void AppWindow::launchCommand(const std::shared_ptr<AbstractCmd> &command, const LaunchCommandOptions &opts,
                              const LaunchProps &props) {
  if (topBar->m_completer->isVisible()) {
    for (int i = 0; i != topBar->m_completer->m_args.size(); ++i) {
      auto &arg = topBar->m_completer->m_args.at(i);
      auto input = topBar->m_completer->m_inputs.at(i);

      qCritical() << "required" << arg.required << input->text();

      if (arg.required && input->text().isEmpty()) {
        input->setFocus();
        return;
      }
    }
  }

  auto preferenceValues = commandDb->getPreferenceValues(command->id());

  for (const auto &preference : command->preferences()) {
    if (preference->isRequired() && !preferenceValues.contains(preference->name())) {
      if (command->type() == CommandType::CommandTypeExtension) {
        auto extensionCommand = std::static_pointer_cast<ExtensionCommand>(command);

        pushView(new MissingExtensionPreferenceView(*this, extensionCommand),
                 {.navigation = NavigationStatus{.title = command->name(), .iconUrl = command->iconUrl()}});
        return;
      }

      qDebug() << "MISSING PREFERENCE" << preference->title();
    }
  }

  qDebug() << "preference values for command with" << command->preferences().size() << "preferences"
           << command->id() << preferenceValues;

  if (commandStack.size() > 1 && commandStack.top().viewStack.empty()) {
    qWarning() << "unloading hanging command";
    commandStack.top().command->unload();
    commandStack.top().command->deleteLater();
    commandStack.pop();
  }

  auto ctx = command->createContext(*this, command, opts.searchQuery);

  if (!ctx) {
    statusBar->setToast("No context returned by command", ToastPriority::Danger);
    return;
  }

  commandStack.push({.command = ctx});
  ctx->load(props);
}

void AppWindow::launchCommand(const QString &id, const LaunchCommandOptions &opts) {
  if (auto command = commandDb->findCommand(id)) { launchCommand(command->command, opts); }
}

void AppWindow::resizeEvent(QResizeEvent *event) {
  QMainWindow::resizeEvent(event);
  auto windowSize = event->size();
  auto topHeight = Omnicast::TOP_BAR_HEIGHT;
  auto statusHeight = Omnicast::STATUS_BAR_HEIGHT;

  centerView->setGeometry({0, topHeight, windowSize.width(), windowSize.height() - topHeight - statusHeight});
  topBar->setGeometry({0, 0, windowSize.width(), topHeight});
  _loadingBar->setGeometry({0, topHeight, windowSize.width(), 1});
  centerView->setGeometry({0, topHeight, windowSize.width(), windowSize.height() - topHeight - statusHeight});
  statusBar->setGeometry({0, windowSize.height() - statusHeight, windowSize.width(), statusHeight});

  topBar->show();
  statusBar->show();
}

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

  QPen pen(theme.colors.border, borderWidth);
  painter.setPen(pen);

  painter.drawPath(path);
}

std::variant<CommandResponse, CommandError> AppWindow::handleCommand(const CommandMessage &message) {
  qDebug() << "received message type" << message.type;

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

    for (const auto &entry : commandDb->commands()) {
      Proto::Dict result;

      result["id"] = entry.command->id().toUtf8().constData();
      result["name"] = entry.command->name().toUtf8().constData();
      results.push_back(result);
    }

    return results;
  }

  if (message.type == "command.push") {
    auto args = message.params.asArray();

    if (args.empty()) { return CommandError{"Ill-formed command.push request"}; }

    auto id = args.at(0).asString();

    /*
if (auto cmd = commandDb->findCommand(id.c_str())) {
emit launchCommand(cmd->factory(*this, ""),
           {.navigation = NavigationStatus{.title = cmd->name, .iconUrl = cmd->iconUrl}});
  setVisible(true);

  return true;
}
    */

    return CommandError{"No such command"};
  }

  return CommandError{"Unknowm command"};
}

void AppWindow::selectPrimaryAction() {
  if (auto action = actionPannel->primaryAction()) { executeAction(action); }
}

void AppWindow::selectSecondaryAction() {}

void AppWindow::executeAction(AbstractAction *action) {
  if (commandStack.size() > 1 && commandStack.top().viewStack.empty()) {
    qWarning() << "unloading hanging command";
    commandStack.top().command->unload();
    commandStack.top().command->deleteLater();
    commandStack.pop();
  }

  auto &command = commandStack.top();
  auto executor = command.viewStack.empty() ? nullptr : &command.viewStack.at(0);

  qDebug() << "executing" << action->title;
  auto executorCommand = commandStack.top().command;

  if (!action->isPushView()) { actionPannel->close(); }

  action->execute(*this);

  if (!action->isPushView()) {
    emit action->didExecute();
    emit actionExecuted(action);
    executorCommand->onActionExecuted(action);

    if (auto cb = action->executionCallback()) { cb(); }
  }
}

void AppWindow::closeWindow(bool withPopToRoot) {
  hide();
  clearSearch();
  topBar->destroyQuicklinkCompleter();

  if (withPopToRoot) popToRoot();
}

AppWindow::AppWindow(QWidget *parent)
    : QMainWindow(parent), topBar(new TopBar(this)), statusBar(new StatusBar(this)),
      actionPannel(new ActionPannelWidget(this)), _dialog(new DialogWidget(this)),
      _loadingBar(new HorizontalLoadingBar(this)) {
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground, true);

  setMinimumSize(Omnicast::WINDOW_SIZE);
  topBar->setFixedHeight(Omnicast::TOP_BAR_HEIGHT);
  statusBar->setFixedHeight(Omnicast::STATUS_BAR_HEIGHT);

  QDir::root().mkpath(Config::dirPath());
  ThemeService::instance().setTheme("Solarized Osaka");
  FaviconService::initialize(new FaviconService(Config::dirPath() + QDir::separator() + "favicon.db"));

  quicklinkDatabase =
      std::make_unique<QuicklistDatabase>(Config::dirPath() + QDir::separator() + "quicklinks.db");
  calculatorDatabase =
      std::make_unique<CalculatorDatabase>(Config::dirPath() + QDir::separator() + "calculator.db");
  appDb = std::make_unique<XdgAppDatabase>();

  omniDb = std::make_unique<OmniDatabase>(Config::dirPath() + QDir::separator() + "omni.db");
  commandDb = std::make_unique<OmniCommandDatabase>(*omniDb.get());
  localStorage = std::make_unique<LocalStorageService>(*omniDb.get());
  extensionManager = std::make_unique<ExtensionManager>(*commandDb.get());

  clipboardService =
      std::make_unique<ClipboardService>(Config::dirPath() + QDir::separator() + "clipboard.db");
  // indexer = std::make_unique<IndexerService>(Config::dirPath() + QDir::separator() + "files.db");
  processManagerService = std::make_unique<ProcessManagerService>();
  auto builtinCommandDb = std::make_unique<CommandDatabase>();

  for (const auto &repo : builtinCommandDb->repositories()) {
    commandDb->registerRepository(repo);
  }

  _commandServer = new CommandServer(this);

  if (!_commandServer->start(Omnicast::commandSocketPath())) {
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

  auto ollamaProvider = std::make_unique<OllamaAiProvider>();

  ollamaProvider->setInstanceUrl(QUrl("http://localhost:11434"));

  aiProvider = std::make_unique<AI::Manager>(*omniDb.get());
  aiProvider->registerProvider(std::move(ollamaProvider), 1);

  extensionManager->start();

  _loadingBar->setFixedHeight(1);
  _loadingBar->setBarWidth(100);

  // topBar->input->installEventFilter(this);

  auto rootCommand = CommandBuilder("root").toSingleView<RootView>();

  launchCommand(rootCommand);
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

template <> Service<OmniCommandDatabase> AppWindow::service<OmniCommandDatabase>() const {
  return *commandDb;
}
