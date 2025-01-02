#include "app.hpp"
#include "command.hpp"
#include "extension_manager.hpp"
#include "filesystem-database.hpp"
#include "image-fetcher.hpp"
#include "indexer-service.hpp"
#include "quicklink-seeder.hpp"
#include "root-command.hpp"
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

void AppWindow::popToRoot() {
  while (commandStack.size() > 1) {
  }
}

bool AppWindow::eventFilter(QObject *obj, QEvent *event) {
  if (obj == topBar->input && event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);
    auto key = keyEvent->key();

    // qDebug() << "key event from app filter";

    if (actionPopover->findBoundAction(keyEvent)) {
      return true;
    }

    bool isEsc = keyEvent->key() == Qt::Key_Escape;

    if (navigationStack.size() > 1) {
      if (isEsc || (keyEvent->key() == Qt::Key_Backspace &&
                    topBar->input->text().isEmpty())) {
        popCurrentView();
        return true;
      }
    } else if (isEsc) {
      qDebug() << "esc";
      if (topBar->input->text().isEmpty()) {
        hide();
      } else {
        topBar->input->clear();
      }
      return true;
    }

    if (keyEvent->modifiers().testFlag(Qt::ControlModifier) &&
        key == Qt::Key_B) {
      actionPopover->toggleActions();
      return true;
    }
  }

  return false;
}

void AppWindow::popCurrentView() {
  if (navigationStack.size() == 1) {
    qDebug() << "Attempted to pop root search";
    return;
  }

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

  qDebug() << "restore action model and delegate";
  actionPopover->setActionData(next.actions);
  topBar->destroyQuicklinkCompleter();
  topBar->input->setReadOnly(false);
  topBar->input->show();
  topBar->input->setFocus();
  topBar->input->installEventFilter(next.view);
  topBar->input->setText(next.query);
  topBar->input->setPlaceholderText(next.placeholderText);
  topBar->input->selectAll();

  if (navigationStack.size() == 1) {
    if (currentCommand) {
      currentCommand->unload(*this);
      currentCommand->deleteLater();
      currentCommand = nullptr;
    }
    topBar->hideBackButton();
  }

  emit currentViewPoped();
}

void AppWindow::popToRootView() {
  while (!navigationStack.empty()) {
    popCurrentView();
  }
}

void AppWindow::disconnectView(View &view) {
  disconnect(topBar->input, &QLineEdit::textEdited, &view,
             &View::onSearchChanged);
  disconnect(actionPopover, &ActionPopover::actionPressed, &view,
             &View::onActionActivated);

  // view->app
  disconnect(&view, &View::pushView, this, &AppWindow::pushView);
  disconnect(&view, &View::pop, this, &AppWindow::popCurrentView);
  disconnect(&view, &View::popToRoot, this, &AppWindow::popToRootView);
  disconnect(&view, &View::launchCommand, this, &AppWindow::launchCommand);
  disconnect(&view, &View::activatePrimaryAction, actionPopover,
             &ActionPopover::selectPrimary);

  topBar->input->removeEventFilter(&view);
}

void AppWindow::connectView(View &view) {
  // app->view
  connect(topBar->input, &QLineEdit::textEdited, &view, &View::onSearchChanged);
  connect(actionPopover, &ActionPopover::actionPressed, &view,
          &View::onActionActivated);

  // view->app
  connect(&view, &View::pushView, this, &AppWindow::pushView);
  connect(&view, &View::pop, this, &AppWindow::popCurrentView);
  connect(&view, &View::popToRoot, this, &AppWindow::popToRootView);
  connect(&view, &View::launchCommand, this, &AppWindow::launchCommand);
  connect(&view, &View::activatePrimaryAction, actionPopover,
          &ActionPopover::selectPrimary);

  view.onAttach();
  topBar->input->installEventFilter(&view);
}

void AppWindow::pushView(View *view) {
  if (navigationStack.size() > 0) {
    auto &old = navigationStack.top();

    disconnectView(*old.view);
  }

  if (navigationStack.size() == 1) {
    topBar->showBackButton();
  }

  if (navigationStack.empty()) {
    layout->replaceWidget(defaultWidget, view->widget);
  } else {
    auto &cur = navigationStack.top();

    cur.query = topBar->input->text();
    cur.placeholderText = topBar->input->placeholderText();
    cur.actions = actionPopover->actions();

    layout->replaceWidget(cur.view->widget, view->widget);
    cur.view->widget->setParent(nullptr);
    cur.view->widget->hide();
  }

  connectView(*view);

  navigationStack.push({.view = view});

  topBar->input->setReadOnly(false);
  topBar->input->show();
  topBar->input->setFocus();
  topBar->input->clear();

  emit topBar->input->textEdited("");

  view->onMount();
}

void AppWindow::launchCommand(ViewCommand *cmd) {
  if (currentCommand) {
    currentCommand->unload(*this);
    currentCommand->deleteLater();
  }

  currentCommand = cmd;

  auto view = cmd->load(*this);

  if (view) {
    pushView(view);
  }
}

void AppWindow::executeAction(AbstractAction *action) {
  action->execute(*this);
}

AppWindow::AppWindow(QWidget *parent)
    : QMainWindow(parent), topBar(new TopBar()), statusBar(new StatusBar()),
      actionPopover(new ActionPopover(this)) {
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);

  setMinimumWidth(850);
  setMinimumHeight(550);

  extensionManager = std::make_unique<ExtensionManager>();

  quicklinkDatabase = std::make_unique<QuicklistDatabase>(
      Config::dirPath() + QDir::separator() + "quicklinks.db");
  calculatorDatabase = std::make_unique<CalculatorDatabase>(
      Config::dirPath() + QDir::separator() + "calculator.db");
  appDb = std::make_unique<AppDatabase>();
  clipboardService = std::make_unique<ClipboardService>();
  iconCache = std::make_unique<IconCacheService>();
  indexer = std::make_unique<IndexerService>(Config::dirPath() +
                                             QDir::separator() + "files.db");

  connect(actionPopover, &ActionPopover::actionExecuted, this,
          &AppWindow::executeAction);

  ImageFetcher::instance();

  {
    auto seeder = std::make_unique<QuickLinkSeeder>(*appDb, *quicklinkDatabase);

    if (quicklinkDatabase->list().isEmpty()) {
      seeder->seed();
    }
  }

  extensionManager->start();

  layout = new QVBoxLayout();

  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);

  layout->setAlignment(Qt::AlignTop);
  installEventFilter(this);
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
  indexer->index(QDir::homePath() + QDir::separator() + "Downloads");
}

template <>
Service<QuicklistDatabase> AppWindow::service<QuicklistDatabase>() const {
  return *quicklinkDatabase;
}

template <> Service<IndexerService> AppWindow::service<IndexerService>() const {
  return *indexer;
}

template <>
Service<CalculatorDatabase> AppWindow::service<CalculatorDatabase>() const {
  return *calculatorDatabase;
}

template <> Service<AppDatabase> AppWindow::service<AppDatabase>() const {
  return *appDb;
}

template <>
Service<ClipboardService> AppWindow::service<ClipboardService>() const {
  return *clipboardService;
}

template <>
Service<ExtensionManager> AppWindow::service<ExtensionManager>() const {
  return *extensionManager;
}

template <>
Service<IconCacheService> AppWindow::service<IconCacheService>() const {
  return *iconCache;
}
