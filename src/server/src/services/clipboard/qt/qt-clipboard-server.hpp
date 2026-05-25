#pragma once
#include "services/clipboard/clipboard-server.hpp"
#include <QGuiApplication>
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

  bool setClipboardContent(QMimeData *data, const Clipboard::CopyOptions &options = {}) override;

  /**
   * Builds a normalized ClipboardSelection from a QMimeData. Picks a single preferred
   * image format, uses structured access for text/html/urls to avoid redundant offers,
   * and filters Qt-internal (application/x-qt-image) and X11 legacy target types.
   *
   * Returns std::nullopt when the selection carries a self-write / concealed marker
   * (vicinae/concealed) and should be silently dropped.
   */
  static std::optional<ClipboardSelection> selectionFromMimeData(const QMimeData *mimeData);

private:
  static bool isLegacyContentType(const QString &str);
  void dataChanged();
};
