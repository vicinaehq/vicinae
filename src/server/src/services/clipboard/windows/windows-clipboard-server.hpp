#pragma once

#include "services/clipboard/clipboard-server.hpp"

class WindowsClipboardServer : public AbstractClipboardServer {
  Q_OBJECT

public:
  QString id() const override { return "windows"; }
  bool isActivatable() const override { return QGuiApplication::platformName() == "windows"; }
  bool isAlive() const override { return true; }
  bool start() override;
  bool stop() override;

private:
  static constexpr int RETRY_DELAY_MS = 100;

  bool writeClipboard(QMimeData *data, const Clipboard::CopyOptions &options) override;
  void handleChange(bool isRetry);
  std::optional<QString> clipboardOwnerApp() const;
};
