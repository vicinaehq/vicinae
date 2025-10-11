#pragma once
#include "services/clipboard/clipboard-server.hpp"
#include <qapplication.h>
#include <qclipboard.h>

class X11ClipboardServer : public AbstractClipboardServer {
public:
  bool start() override {
    auto clip = QApplication::clipboard();
    connect(clip, &QClipboard::dataChanged, this, &X11ClipboardServer::dataChanged);
    return true;
  }

  bool stop() override {
    disconnect(QApplication::clipboard());
    return true;
  }

  QString id() const override { return "x11"; }
  bool isActivatable() const override { return QApplication::platformName() == "x11"; }
  bool isAlive() const override { return true; }

private:
  void dataChanged() {}
};
