#pragma once
#include "proto/wlr-clipboard.pb.h"
#include "services/clipboard/clipboard-server.hpp"
#include <qprocess.h>

/**
 * Generic data control clipboard server connector.
 * This is used as a base to communicate with the wlr-clip and the ext-clip, which are almost identical.
 * The activation condition is reimplemented by each.
 */
class DataControlClipboardServer : public AbstractClipboardServer {
public:
  static constexpr const char *ENTRYPOINT = "wlr-clip";

  DataControlClipboardServer(const std::string &entrypoint);
  bool start() override;
  bool stop() override;
  virtual bool isActivatable() const override = 0;
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
  std::string m_entrypoint;
};
