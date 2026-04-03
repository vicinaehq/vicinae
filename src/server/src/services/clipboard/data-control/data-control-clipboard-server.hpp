#pragma once
#include "services/clipboard/clipboard-server.hpp"
#include "generated/wlr-clipboard-client.hpp"
#include <qprocess.h>

class DataControlBus : public QObject, public wlrclip::AbstractTransport {
  Q_OBJECT

  struct MessageBuffer {
    QByteArray data;
  };

  MessageBuffer m_message;
  QIODevice *m_device = nullptr;
  void send(std::string_view data) override;
  void readyRead();

public:
  DataControlBus(QIODevice *device);

signals:
  void messageReceived(const QByteArray &msg);
};

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
  void handleReadError();
  void handleExit(int code, QProcess::ExitStatus status);

  QProcess m_process;
  DataControlBus m_bus;
  wlrclip::RpcTransport m_rpc;
  wlrclip::Client m_client;
};
