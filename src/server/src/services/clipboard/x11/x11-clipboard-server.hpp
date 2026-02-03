#pragma once
#include "../qt/qt-clipboard-server.hpp"
#include <qapplication.h>
#include <qbuffer.h>
#include <qclipboard.h>
#include <QUrl>
#include <QMimeData>

class X11ClipboardServer : public AbstractQtClipboardServer {
public:
  QString id() const override { return "x11"; }
  bool isActivatable() const override { return QApplication::platformName() == "xcb"; }
};
