#pragma once
#include "services/clipboard/clipboard-server.hpp"
#include <qapplication.h>
#include <qbuffer.h>
#include <qclipboard.h>
#include <QUrl>
#include <QMimeData>

class AbstractQtClipboardServer : public AbstractClipboardServer {
public:
  bool start() override;
  bool stop() override;
  virtual QString id() const override = 0;
  virtual bool isActivatable() const override = 0;
  bool isAlive() const override { return true; }

private:
  static bool isLegacyContentType(const QString &str);
  void dataChanged();
};
