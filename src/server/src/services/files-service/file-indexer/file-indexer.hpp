#pragma once
#include <QByteArray>
#include <QObject>
#include <QProcess>
#include <qlogging.h>
#include "common/common.hpp"
#include "generated/file-indexer-client.hpp"
#include "services/files-service/abstract-file-indexer.hpp"

class FileIndexerBus : public QObject, public file_indexer_gen::AbstractTransport {
  Q_OBJECT

  struct MessageBuffer {
    QByteArray data;
  };

  MessageBuffer m_message;
  QIODevice *m_device = nullptr;
  void send(std::string_view data) override;
  void readyRead();

public:
  FileIndexerBus(QIODevice *device);

signals:
  void messageReceived(const QByteArray &msg);
};

class FileIndexer : public AbstractFileIndexer {
  Q_OBJECT

public:
  static constexpr int MAX_RESTART_ATTEMPTS = 5;
  static constexpr int BASE_RESTART_DELAY_MS = 1000;

  FileIndexer();

  void start() override;
  void rebuildIndex() override;
  void preferenceValuesChanged(const QJsonObject &preferences) override;
  QFuture<std::vector<IndexerFileResult>> queryAsync(std::string_view view,
                                                     const QueryParams &params = {}) override;

  bool isRunning() const { return m_process.state() == QProcess::ProcessState::Running; }

private:
  void startProcess();
  void stopProcess();
  void handleCrash();
  void handleStderr();
  void sendConfigure();

  QProcess m_process;
  FileIndexerBus m_bus;
  file_indexer_gen::RpcTransport m_rpc;
  file_indexer_gen::Client m_client;
  file_indexer_gen::IndexerConfig m_config;
  QByteArray m_stderrBuf;
  int m_crashCount = 0;
  bool m_wantRunning = false;
};
