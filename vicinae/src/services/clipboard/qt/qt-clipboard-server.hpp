#pragma once
#include "services/clipboard/clipboard-server.hpp"
#include <QClipboard>
#include <QApplication>
#include <QMimeData>

class QtClipboardServer : public AbstractClipboardServer {
  Q_OBJECT

private:
  // Qt clipboard monitoring
  QClipboard *m_qtClipboard = nullptr;

  // State tracking
  bool m_qtFallbackActive = false;
  QByteArray m_lastClipboardHash;

  // Connection state
  bool m_isConnected = false;

  // Qt clipboard methods
  bool setupQtFallback();
  void disconnectQtFallback();

  // Content processing
  ClipboardSelection parseQtMimeData(const QMimeData *mimeData) const;
  QByteArray calculateContentHash(const ClipboardSelection &selection) const;
  bool hasContentChanged(const QMimeData *mimeData) const;

private slots:
  void handleQtClipboardChange();

public:
  bool start() override;
  bool isActivatable() const override;
  bool isAlive() const override;
  QString id() const override;
  int activationPriority() const override;

  QtClipboardServer();
  ~QtClipboardServer() override;
};
