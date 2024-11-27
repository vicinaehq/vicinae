#include "calculator-database.hpp"
#include "command-database.hpp"
#include "command-object.hpp"
#include "commands/index/index-command.hpp"
#include "common.hpp"
#include "omnicast.hpp"
#include "quicklist-database.hpp"
#include "ui/action_popover.hpp"
#include "xdg-desktop-database.hpp"
#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QThread>
#include <QVBoxLayout>
#include <memory>
#include <optional>
#include <qboxlayout.h>
#include <qevent.h>
#include <qlogging.h>
#include <qmainwindow.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <stdexcept>

void AppWindow::setCommandObject(CommandObject *cmd) {
  /*
auto oldCmd = command;

layout->replaceWidget(oldCmd->widget, cmd->widget);
oldCmd->deleteLater();
command = cmd;
*/
}

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

void AppWindow::resetCommand() {
  topBar->hideBackButton();
  currentCommand = std::nullopt;
  // setCommandObject(new IndexCommand());
  statusBar->reset();
}

void AppWindow::setCommand(const Command *cmd) {
  currentCommand = cmd;
  topBar->showBackButton();
  // statusBar->setActiveCommand(cmd->name, cmd->iconName);
}

bool AppWindow::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);
    auto key = keyEvent->key();

    if (obj == topBar->input && commandStack.size() > 1) {
      bool isEsc = keyEvent->key() == Qt::Key_Escape;

      if (isEsc || (keyEvent->key() == Qt::Key_Backspace &&
                    topBar->input->text().isEmpty())) {
        popCommandObject();
        return true;
      }
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

AppWindow::AppWindow(QWidget *parent)
    : QMainWindow(parent), topBar(new TopBar()), statusBar(new StatusBar()),
      actionPopover(new ActionPopover(this)) {
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);

  setMinimumWidth(850);
  setMinimumHeight(550);

  auto config = loadConfig("config.toml");

  /*
  auto extman = new ExtensionManager(config->extensions);
  QThread *extmanThread = new QThread(this);

  extman->moveToThread(extmanThread);

  connect(extman, &ExtensionManager::render, this, [this, extman](auto json) {
    root = renderComponentTree(extman, root, json);

    if (root->ui == centralWidget()) {
      update();
    } else {
      setCentralWidget(root->ui);
    }
  });

  connect(extmanThread, &QThread::started, extman,
          &ExtensionManager::startServer);

  extmanThread->start();
  */

  quicklinkDatabase = std::make_shared<QuicklistDatabase>(
      Config::dirPath() + QDir::separator() + "quicklinks.db");
  calculatorDatabase = std::make_shared<CalculatorDatabase>(
      Config::dirPath() + QDir::separator() + "calculator.db");
  xdd = std::make_shared<XdgDesktopDatabase>();

  layout = new QVBoxLayout();

  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);

  auto index = new IndexCommand(this);

  index->setParent(this);
  index->onAttach();

  layout->setAlignment(Qt::AlignTop);
  topBar->input->installEventFilter(this);
  topBar->input->installEventFilter(index);
  layout->addWidget(topBar);
  layout->addWidget(new HDivider);
  layout->addWidget(index->widget, 1);
  layout->addWidget(statusBar);

  commandStack.push(index);

  auto widget = new QWidget();

  widget->setLayout(layout);

  setCentralWidget(widget);

  connect(topBar->input, &QLineEdit::textChanged,
          [this](auto arg) { commandStack.top()->onSearchChanged(arg); });
  connect(actionPopover, &ActionPopover::actionActivated,
          [this](auto arg) { commandStack.top()->onActionActivated(arg); });
}

template <>
std::shared_ptr<QuicklistDatabase>
AppWindow::service<QuicklistDatabase>() const {
  return quicklinkDatabase;
}

template <>
std::shared_ptr<XdgDesktopDatabase>
AppWindow::service<XdgDesktopDatabase>() const {
  return xdd;
}

template <>
std::shared_ptr<CalculatorDatabase>
AppWindow::service<CalculatorDatabase>() const {
  return calculatorDatabase;
}
