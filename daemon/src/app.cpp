#include "app.hpp"
#include "block-device-service.hpp"
#include "command.hpp"
#include "extension_manager.hpp"
#include "image-fetcher.hpp"
#include "indexer-service.hpp"
#include "process-manager-service.hpp"
#include "quicklink-seeder.hpp"
#include "root-command.hpp"
#include "ui/action_popover.hpp"
#include <QLabel>
#include <QMainWindow>
#include <QThread>
#include <QVBoxLayout>
#include <memory>
#include <qboxlayout.h>
#include <qevent.h>
#include <qlogging.h>
#include <qmainwindow.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>

bool AppWindow::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);
    auto key = keyEvent->key();
    qDebug() << "app filter" << QKeySequence(key).toString();

    if (actionPopover->findBoundAction(keyEvent)) return true;

    if (keyEvent->modifiers().testFlag(Qt::ControlModifier) && key == Qt::Key_B) {
      actionPopover->toggleActions();
      return true;
    }
  }

  return false;
}

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
        topBar->input->clear();
        emit topBar->input->textEdited("");
      }
      return true;
    }

    QApplication::sendEvent(navigationStack.top().view, event);
    return true;
  }

  QWidget::event(event);

  return false;
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

  auto next = navigationStack.top();

  layout->replaceWidget(previous.view->widget, next.view->widget);
  previous.view->widget->setParent(nullptr);
  previous.view->setParent(nullptr);

  connectView(*next.view);
  next.view->setParent(this);
  next.view->widget->show();

  previous.view->deleteLater();

  actionPopover->setSignalActions(next.actions);
  topBar->destroyQuicklinkCompleter();
  topBar->input->setReadOnly(false);
  topBar->input->show();
  topBar->input->setFocus();
  topBar->input->setText(next.query);
  topBar->input->setPlaceholderText(next.placeholderText);
  topBar->input->selectAll();

  if (navigationStack.size() == 1) { topBar->hideBackButton(); }

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
  disconnect(topBar->input, &QLineEdit::textEdited, &view, &View::onSearchChanged);
  disconnect(actionPopover, &ActionPopover::actionPressed, &view, &View::onActionActivated);

  // view->app
  disconnect(&view, &View::pushView, this, &AppWindow::pushView);
  disconnect(&view, &View::pop, this, &AppWindow::popCurrentView);
  disconnect(&view, &View::popToRoot, this, &AppWindow::popToRootView);
  disconnect(&view, &View::launchCommand, this, &AppWindow::launchCommand);
  disconnect(&view, &View::activatePrimaryAction, this, &AppWindow::selectPrimaryAction);

  view.removeEventFilter(this);
}

void AppWindow::connectView(View &view) {
  // app->view
  connect(topBar->input, &QLineEdit::textEdited, &view, &View::onSearchChanged);
  connect(actionPopover, &ActionPopover::actionPressed, &view, &View::onActionActivated);

  // view->app
  connect(&view, &View::pushView, this, &AppWindow::pushView);
  connect(&view, &View::pop, this, &AppWindow::popCurrentView);
  connect(&view, &View::popToRoot, this, &AppWindow::popToRootView);
  connect(&view, &View::launchCommand, this, &AppWindow::launchCommand);
  connect(&view, &View::activatePrimaryAction, this, &AppWindow::selectPrimaryAction);

  view.onAttach();
  view.installEventFilter(this);
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
    layout->replaceWidget(defaultWidget, view->widget);
  } else {
    auto &cur = navigationStack.top();

    cur.query = topBar->input->text();
    cur.placeholderText = topBar->input->placeholderText();
    cur.actions = actionPopover->signalActions;

    layout->replaceWidget(cur.view->widget, view->widget);
    cur.view->widget->setParent(nullptr);
    cur.view->widget->hide();

    if (opts.navigation) statusBar->setNavigationTitle(opts.navigation->title, opts.navigation->icon);
  }

  connectView(*view);

  actionPopover->setSignalActions({});
  currentCommand.viewStack.push({.view = view});
  navigationStack.push({.view = view});

  topBar->input->setReadOnly(false);
  topBar->input->show();
  topBar->input->setFocus();
  topBar->input->setText(opts.searchQuery);
  emit topBar->input->textEdited(opts.searchQuery);
  view->onMount();
}

void AppWindow::launchCommand(ViewCommand *cmd, const LaunchCommandOptions &opts) {
  commandStack.push({.command = cmd});

  auto view = cmd->load(*this);

  if (view) { pushView(view, {.searchQuery = opts.searchQuery, .navigation = opts.navigation}); }
}

void AppWindow::selectPrimaryAction() {
  if (actionPopover->signalActions.isEmpty()) return;

  executeAction(actionPopover->signalActions.at(0));
}

void AppWindow::selectSecondaryAction() {
  if (actionPopover->signalActions.size() > 1) return;

  executeAction(actionPopover->signalActions.at(1));
}

void AppWindow::executeAction(AbstractAction *action) { action->execute(*this); }

void AppWindow::closeWindow(bool withPopToRoot) {
  hide();
  topBar->input->clear();
  topBar->destroyQuicklinkCompleter();

  if (withPopToRoot) popToRootView();
}

AppWindow::AppWindow(QWidget *parent)
    : QMainWindow(parent), topBar(new TopBar()), statusBar(new StatusBar()),
      actionPopover(new ActionPopover(this)) {
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);

  setMinimumWidth(850);
  setMinimumHeight(550);

  extensionManager = std::make_unique<ExtensionManager>();

  quicklinkDatabase =
      std::make_unique<QuicklistDatabase>(Config::dirPath() + QDir::separator() + "quicklinks.db");
  calculatorDatabase =
      std::make_unique<CalculatorDatabase>(Config::dirPath() + QDir::separator() + "calculator.db");
  appDb = std::make_unique<AppDatabase>();
  clipboardService = std::make_unique<ClipboardService>();
  iconCache = std::make_unique<IconCacheService>();
  indexer = std::make_unique<IndexerService>(Config::dirPath() + QDir::separator() + "files.db");
  processManagerService = std::make_unique<ProcessManagerService>();

  for (const auto &proc : processManagerService->list()) {
    qDebug() << proc.comm << proc.pid;
  }

  connect(topBar->input, &SearchBar::pop, this, &AppWindow::popCurrentView);
  connect(actionPopover, &ActionPopover::actionExecuted, this, &AppWindow::executeAction);

  ImageFetcher::instance();

  {
    auto seeder = std::make_unique<QuickLinkSeeder>(*appDb, *quicklinkDatabase);

    if (quicklinkDatabase->list().isEmpty()) { seeder->seed(); }
  }

  auto blk = std::make_unique<BlockDeviceService>();

  qDebug() << "listing mountpoints";

  for (const auto mnt : blk->deviceMountpoints()) {
    qDebug() << mnt.devicePath << mnt.mountpointPath;
  }

  extensionManager->start();

  layout = new QVBoxLayout();

  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);

  layout->setAlignment(Qt::AlignTop);
  topBar->input->installEventFilter(this);
  layout->addWidget(topBar);
  layout->addWidget(new HDivider);
  layout->addWidget(defaultWidget, 1);
  layout->addWidget(statusBar);

  // commandStack.push(index);

  auto widget = new QWidget();

  widget->setLayout(layout);

  setCentralWidget(widget);

  launchCommand(new RootCommand);

  /*
  connect(topBar->input, &QLineEdit::textChanged, [this](auto arg) {
    commandStack.top()->onSearchChanged(arg);
    emit navigationStack.top()->onSearchChanged(arg);
  });
  connect(actionPopover, &ActionPopover::actionActivated,
          [this](auto arg) { commandStack.top()->onActionActivated(arg); });
 */

  qDebug() << QDir::homePath();
  // indexer->index(QDir::homePath() + QDir::separator() +
  //"prog/perso/getsslnow/");
  // indexer->index(QDir::homePath());
}

template <> Service<QuicklistDatabase> AppWindow::service<QuicklistDatabase>() const {
  return *quicklinkDatabase;
}

template <> Service<IndexerService> AppWindow::service<IndexerService>() const { return *indexer; }

template <> Service<CalculatorDatabase> AppWindow::service<CalculatorDatabase>() const {
  return *calculatorDatabase;
}

template <> Service<AppDatabase> AppWindow::service<AppDatabase>() const { return *appDb; }

template <> Service<ClipboardService> AppWindow::service<ClipboardService>() const {
  return *clipboardService;
}

template <> Service<ExtensionManager> AppWindow::service<ExtensionManager>() const {
  return *extensionManager;
}

template <> Service<IconCacheService> AppWindow::service<IconCacheService>() const { return *iconCache; }

template <> Service<ProcessManagerService> AppWindow::service<ProcessManagerService>() const {
  return *processManagerService;
}
