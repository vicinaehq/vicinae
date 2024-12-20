#include "app.hpp"
#include "command-object.hpp"
#include "command.hpp"
#include "commands/index/index-command.hpp"
#include "extension_manager.hpp"
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
#include <stdexcept>

void AppWindow::pushCommandObject(std::shared_ptr<ICommandFactory> factory) {
  if (commandStack.empty())
    throw std::runtime_error(
        "Tried to push command to an empty command stack. At least one command "
        "(the root one) should be present.");

  CommandObject *top = commandStack.top();
  CommandObject *cmd = (*factory)(this);

  layout->replaceWidget(top->widget, cmd->widget);

  // clean top
  topBar->input->removeEventFilter(top);
  top->widget->setParent(nullptr);
  top->setParent(nullptr);
  top->widget->hide();

  // setup next
  statusBar->setActiveCommand(cmd->name(), cmd->icon());
  cmd->setParent(this);
  topBar->input->installEventFilter(cmd);

  commandStack.push(cmd);
  cmd->onAttach();

  if (commandStack.size() == 2) {
    topBar->showBackButton();
  }
}

void AppWindow::popToRoot() {
  while (commandStack.size() > 1) {
    popCommandObject();
  }
}

void AppWindow::popCommandObject() {
  if (commandStack.size() < 2)
    throw std::runtime_error("Cannot pop stack < 2 ");

  CommandObject *prev = commandStack.top();
  commandStack.pop();
  CommandObject *next = commandStack.top();

  qDebug() << "pop " << prev->name() << " for " << next->name();

  layout->replaceWidget(prev->widget, next->widget);

  // tear down prev
  topBar->input->removeEventFilter(prev);
  prev->onDetach();
  prev->setParent(nullptr);

  // restore next
  topBar->input->setReadOnly(false);
  topBar->input->show();
  topBar->input->setFocus();
  topBar->input->installEventFilter(next);
  next->setParent(this);
  next->onAttach();
  next->widget->show();

  if (commandStack.size() == 1) {
    statusBar->reset();
    topBar->hideBackButton();
  }

  prev->deleteLater();
}

bool AppWindow::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);
    auto key = keyEvent->key();

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

    if (keyEvent->modifiers().testFlag(Qt::ControlModifier)) {
      qDebug() << "control";
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

  disconnectView(*previous);

  auto next = navigationStack.top();

  layout->replaceWidget(previous->widget, next->widget);
  previous->widget->setParent(nullptr);
  previous->setParent(nullptr);

  connectView(*next);
  next->setParent(this);
  next->widget->show();

  previous->deleteLater();

  if (navigationStack.size() == 1) {
    currentCommand->unload(*this);
    currentCommand->deleteLater();
    currentCommand = nullptr;
    topBar->hideBackButton();
  }
}

void AppWindow::popToRootView() {
  while (!navigationStack.empty()) {
    popCurrentView();
  }
}

void AppWindow::disconnectView(View &view) {
  disconnect(topBar->input, &QLineEdit::textChanged, &view,
             &View::onSearchChanged);

  // view->app
  disconnect(&view, &View::pushView, this, &AppWindow::pushView);
  disconnect(&view, &View::pop, this, &AppWindow::popCurrentView);
  disconnect(&view, &View::popToRoot, this, &AppWindow::popToRootView);
  disconnect(&view, &View::launchCommand, this, &AppWindow::launchCommand);

  topBar->input->removeEventFilter(&view);
}

void AppWindow::connectView(View &view) {
  // app->view
  connect(topBar->input, &QLineEdit::textChanged, &view,
          &View::onSearchChanged);

  // view->app
  connect(&view, &View::pushView, this, &AppWindow::pushView);
  connect(&view, &View::pop, this, &AppWindow::popCurrentView);
  connect(&view, &View::popToRoot, this, &AppWindow::popToRootView);
  connect(&view, &View::launchCommand, this, &AppWindow::launchCommand);

  topBar->input->installEventFilter(&view);
}

void AppWindow::pushView(View *view) {
  if (navigationStack.size() > 0) {
    auto old = navigationStack.top();

    disconnectView(*old);
  }

  if (navigationStack.size() == 1) {
    topBar->showBackButton();
  }

  connectView(*view);

  if (navigationStack.empty()) {
    layout->replaceWidget(defaultWidget, view->widget);
  } else {
    auto cur = navigationStack.top();

    layout->replaceWidget(cur->widget, view->widget);
    cur->widget->setParent(nullptr);
    cur->widget->hide();
  }

  navigationStack.push(view);
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

AppWindow::AppWindow(QWidget *parent)
    : QMainWindow(parent), topBar(new TopBar()), statusBar(new StatusBar()),
      actionPopover(new ActionPopover(this)) {
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);

  setMinimumWidth(850);
  setMinimumHeight(550);

  auto config = loadConfig("config.toml");

  extensionManager = std::make_unique<ExtensionManager>();

  quicklinkDatabase = std::make_unique<QuicklistDatabase>(
      Config::dirPath() + QDir::separator() + "quicklinks.db");
  calculatorDatabase = std::make_unique<CalculatorDatabase>(
      Config::dirPath() + QDir::separator() + "calculator.db");
  appDb = std::make_unique<AppDatabase>();
  clipboardService = std::make_unique<ClipboardService>();

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

  /*
  auto index = new IndexCommand(this);

  index->setParent(this);
  index->onAttach();
  */

  layout->setAlignment(Qt::AlignTop);
  installEventFilter(this);
  topBar->input->installEventFilter(this);
  // topBar->input->installEventFilter(index);
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
}

template <>
Service<QuicklistDatabase> AppWindow::service<QuicklistDatabase>() const {
  return *quicklinkDatabase;
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
