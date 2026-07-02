#pragma once

#include "services/clipboard/clipboard-server.hpp"
#include <QTimer>

class MacosClipboardServer : public AbstractClipboardServer {
  Q_OBJECT

public:
  MacosClipboardServer();
  ~MacosClipboardServer() override;

  bool start() override;
  bool stop() override;
  bool isActivatable() const override;
  bool isAlive() const override;
  QString id() const override;
  bool setClipboardContent(QMimeData *data, const Clipboard::CopyOptions &options = {}) override;

private:
  static constexpr int POLL_INTERVAL_MS = 500;

  void poll();
  long long currentChangeCount() const;
  bool shouldDropCurrentSelection() const;
  std::optional<QString> pasteboardSourceApp() const;
  std::optional<QString> frontmostBundleId() const;
  void beginAppNapActivity();
  void endAppNapActivity();

  QTimer m_timer;
  long long m_lastChangeCount = -1;
  void *m_appNapActivity = nullptr;
};
