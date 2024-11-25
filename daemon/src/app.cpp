#include "command-database.hpp"
#include "command-object.hpp"
#include "index-command.hpp"
#include "omnicast.hpp"
#include "ui/action_popover.hpp"
#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QThread>
#include <QVBoxLayout>
#include <optional>
#include <qboxlayout.h>
#include <qevent.h>
#include <qlogging.h>
#include <qmainwindow.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>

void AppWindow::setCommandObject(CommandObject *cmd) {
  auto oldCmd = command;

  layout->replaceWidget(oldCmd->widget, cmd->widget);
  oldCmd->deleteLater();
  command = cmd;
}

void AppWindow::resetCommand() {
  topBar->hideBackButton();
  currentCommand = std::nullopt;
  setCommandObject(new IndexCommand(this));
  statusBar->reset();
}

void AppWindow::setCommand(const Command *cmd) {
  currentCommand = cmd;
  topBar->showBackButton();
  setCommandObject(cmd->widgetFactory(this));
  statusBar->setActiveCommand(cmd->name, cmd->iconName);
}

bool AppWindow::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);
    auto key = keyEvent->key();

    if (obj == topBar->input && currentCommand) {
      bool isEsc = keyEvent->key() == Qt::Key_Escape;

      if (isEsc || (keyEvent->key() == Qt::Key_Backspace &&
                    topBar->input->text().isEmpty())) {
        resetCommand();
        return true;
      }
    }

    if (keyEvent->modifiers().testFlag(Qt::ControlModifier)) {
      qDebug() << "control";
    }

    if (keyEvent->modifiers().testFlag(Qt::ControlModifier) &&
        key == Qt::Key_B) {
      actionPopover->showActions();
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

  layout = new QVBoxLayout();

  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);

  command = new IndexCommand(this);

  layout->setAlignment(Qt::AlignTop);
  topBar->input->installEventFilter(this);
  layout->addWidget(topBar);
  layout->addWidget(command->widget, 1);
  layout->addWidget(statusBar);

  auto widget = new QWidget();

  widget->setLayout(layout);

  setCentralWidget(widget);
}
