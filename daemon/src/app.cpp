#include "app.hpp"
#include "extension_manager.hpp"
#include "render.hpp"
#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QThread>
#include <QVBoxLayout>
#include <qboxlayout.h>
#include <qmainwindow.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>

AppWindow::AppWindow(QWidget *parent) : QMainWindow(parent) {
  // setFixedWidth(600);
  // setFixedHeight(400);

  auto config = loadConfig("config.toml");

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
}
