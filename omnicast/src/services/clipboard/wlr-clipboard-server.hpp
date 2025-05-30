#pragma once
#include "services/clipboard/clipboard-server.hpp"
#include "proto.hpp"
#include <qprocess.h>

class WlrClipboardServer : public AbstractClipboardServer {
  std::vector<uint8_t> _message;
  uint32_t _messageLength = 0;
  Proto::Marshaler _marshaler;
  QProcess *process = nullptr;

  bool isAlive() const override;

  void handleMessage(const std::string &message, const Proto::Variant &data);
  void handleRead();
  void handleExit(int code, QProcess::ExitStatus status);

public:
  bool start() override;
  bool isActivatable() const override;

  WlrClipboardServer();
};
