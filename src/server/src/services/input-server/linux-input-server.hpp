#pragma once
#include <QObject>
#include <QProcess>
#include <QTimer>
#include <netinet/in.h>
#include <qlogging.h>
#include "common/common.hpp"
#include "generated/snippet-client.hpp"

class InputServerBus : public QObject, public snippet_gen::AbstractTransport {
  Q_OBJECT

  struct MessageBuffer {
    QByteArray data;
  };

  MessageBuffer m_message;
  QIODevice *m_device = nullptr;
  void send(std::string_view data) override;
  void readyRead();

public:
  InputServerBus(QIODevice *device);

signals:
  void messageReceived(const QByteArray &msg);
};

class LinuxInputServer : public QObject {
  Q_OBJECT

signals:
  void keywordTriggered(std::string trigger) const;
  void undoTriggered(std::string trigger) const;
  void serverReady();

public:
  static constexpr int MAX_RESTART_ATTEMPTS = 5;
  static constexpr int STABILITY_THRESHOLD_MS = 30000;
  static constexpr int BASE_RESTART_DELAY_MS = 1000;

  LinuxInputServer();

  void setEnabled(bool value);

  void start();
  void stop() { m_process.close(); }

  void registerSnippet(snippet_gen::CreateSnippetRequest payload);
  void unregisterSnippet(std::string_view keyword);
  void setKeymap(snippet_gen::LayoutInfo info);
  void resetContext();

  void injectExpand(unsigned charsToDelete, unsigned prePasteDelayUs, bool terminal,
                    unsigned cursorLeftMoves);
  void injectUndo(unsigned backspaceCount, const std::string &trigger);
  void injectPaste(bool terminal);
  void setKeyDelay(int us);
  bool supportsKeyInjection() const { return m_supportsInjection; }

  bool isRunning() const { return m_process.state() == QProcess::ProcessState::Running; }

private:
  void handleError();
  void handleCrash();
  void scheduleStabilityReset();

  QProcess m_process;
  InputServerBus m_bus;
  snippet_gen::RpcTransport m_rpc;
  snippet_gen::Client m_client;
  bool m_supportsInjection = false;
  std::optional<bool> m_enabled;
  int m_crashCount = 0;
};
