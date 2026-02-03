#pragma once
#include "proto/wlr-clipboard.pb.h"
#include "services/clipboard/clipboard-server.hpp"
#include <qprocess.h>

class DataControlClipboardServer : public AbstractClipboardServer {
public:
  DataControlClipboardServer();
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
  std::string m_message;
};
