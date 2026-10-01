#pragma once

#include <memory>
#include "services/clipboard/clipboard-server.hpp"

class WindowsImageMime;

class WindowsClipboardServer : public AbstractClipboardServer {
  Q_OBJECT

public:
  WindowsClipboardServer();
  ~WindowsClipboardServer() override;

  QString id() const override { return "windows"; }
  bool isActivatable() const override { return QGuiApplication::platformName() == "windows"; }
  bool isAlive() const override { return true; }
  bool start() override;
  bool stop() override;

private:
  static constexpr int RETRY_DELAY_MS = 100;

  std::unique_ptr<WindowsImageMime> m_imageMime;

  bool writeClipboard(QMimeData *data, const Clipboard::CopyOptions &options) override;
  void handleChange(bool isRetry);
  std::optional<QString> clipboardOwnerApp() const;
};
