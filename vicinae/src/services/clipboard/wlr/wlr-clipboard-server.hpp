#pragma once
#include "proto/wlr-clipboard.pb.h"
#include "services/clipboard/clipboard-server.hpp"
#include <qprocess.h>

class WlrClipboardServer : public AbstractClipboardServer {
public:
  static constexpr const char *ENTRYPOINT = "wlr-clip";

  WlrClipboardServer();
  bool start() override;
  bool stop() override;
  bool isActivatable() const override;
  QString id() const override;
  int activationPriority() const override;

private:
  bool isAlive() const override;
  void handleMessage(const proto::ext::wlrclip::Selection &selection);
  void handleRead();
  void handleReadError();
  void handleExit(int code, QProcess::ExitStatus status);

  QProcess m_process;
  std::vector<uint8_t> _message;
  uint32_t _messageLength = 0;
};
