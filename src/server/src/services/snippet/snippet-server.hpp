#pragma once
#include <QProcess>
#include <qlogging.h>
#include <QObject>
#include <netinet/in.h>
#include "common/common.hpp"
#include "generated/snippet-client.hpp"

class SnippetServerBus : public QObject, public snippet_gen::AbstractTransport {
  Q_OBJECT

  struct MessageBuffer {
    QByteArray data;
  };

  MessageBuffer m_message;
  QIODevice *m_device = nullptr;
  void send(std::string_view data) override;
  void readyRead();

public:
  SnippetServerBus(QIODevice *device);

signals:
  void messageReceived(const QByteArray &msg);
};

class SnippetServer : public QObject {
  Q_OBJECT

signals:
  void keywordTriggered(std::string trigger) const;
  void undoTriggered(std::string trigger) const;
  void serverStopped();

public:
  SnippetServer();

  void start();
  void stop() { m_process.close(); }

  void registerSnippet(snippet_gen::CreateSnippetRequest payload);
  void unregisterSnippet(std::string_view keyword);
  void setKeymap(snippet_gen::LayoutInfo info);
  void resetContext();

  void injectExpand(int charsToDelete, int prePasteDelayUs, bool terminal, int cursorLeftMoves);
  void injectUndo(int backspaceCount, const std::string &trigger);
  void injectPaste(bool terminal);
  void setKeyDelay(int us);
  bool supportsKeyInjection() const { return m_supportsInjection; }

  bool isRunning() const { return m_process.state() == QProcess::ProcessState::Running; }

private:
  void handleError();

  QProcess m_process;
  SnippetServerBus m_bus;
  snippet_gen::RpcTransport m_rpc;
  snippet_gen::Client m_client;
  bool m_supportsInjection = false;
};
