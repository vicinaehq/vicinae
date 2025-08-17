#pragma once
#include "build/wlr-clip/proto/wlr-clipboard.pb.h"
#include "services/clipboard/clipboard-server.hpp"
#include <qprocess.h>

class WlrClipboardServer : public AbstractClipboardServer {
  std::vector<uint8_t> _message;
  uint32_t _messageLength = 0;
  QProcess *process = nullptr;

  bool isAlive() const override;

  void handleMessage(const proto::ext::wlrclip::Selection &selection);
  void handleRead();
  void handleReadError();
  void handleExit(int code, QProcess::ExitStatus status);

public:
  bool start() override;
  bool isActivatable() const override;
  QString id() const override;

  WlrClipboardServer();
};
