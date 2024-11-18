#include "command-database.hpp"
#include "index-command.hpp"
#include "omnicast.hpp"
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

void AppWindow::setCommandWidget(CommandWidget *widget) {
  auto oldCmd = command;

  layout->replaceWidget(oldCmd, widget);
  oldCmd->deleteLater();
  command = widget;
}

void AppWindow::resetCommand() {
  currentCommand = std::nullopt;
  setCommandWidget(new IndexCommand(this));
}

void AppWindow::setCommand(const Command *cmd) {
  currentCommand = cmd;
  setCommandWidget(cmd->widgetFactory(this));
}

bool AppWindow::eventFilter(QObject *obj, QEvent *event) {
  if (obj == topBar->input && event->type() == QEvent::KeyPress &&
      currentCommand) {
    auto keyEvent = static_cast<QKeyEvent *>(event);
    auto key = keyEvent->key();
    bool isEsc = keyEvent->key() == Qt::Key_Escape;

    if (isEsc || (keyEvent->key() == Qt::Key_Backspace &&
                  topBar->input->text().isEmpty())) {
      resetCommand();
      return true;
    }
  }

  return false;
}

AppWindow::AppWindow(QWidget *parent)
    : QMainWindow(parent), topBar(new TopBar()), statusBar(new StatusBar()) {
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);

  setMinimumWidth(850);
  setMinimumHeight(550);

  // auto config = loadConfig("config.toml");

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
  layout->addWidget(command, 1);
  layout->addWidget(statusBar);

  connect(command, &CommandWidget::replaceCommand, this,
          &AppWindow::setCommand);

  auto widget = new QWidget();

  widget->setLayout(layout);

  setCentralWidget(widget);
}
