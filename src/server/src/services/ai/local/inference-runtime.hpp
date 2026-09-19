#pragma once
#include <chrono>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <QByteArray>
#include <QObject>
#include <QProcess>
#include <QPromise>
#include <QTimer>
#include <qfuture.h>
#include "generated/inference-client.hpp"
#include "services/ai/ai-provider.hpp"
#include "services/audio/audio-recorder.hpp"
#include "services/local-model-registry/local-model-catalogue.hpp"

namespace AI {

class InferenceBus : public QObject, public inference_gen::AbstractTransport {
  Q_OBJECT

signals:
  void messageReceived(const QByteArray &msg);

public:
  explicit InferenceBus(QIODevice *device);

private:
  void send(std::string_view data) override;
  void readyRead();

  QByteArray m_buffer;
  QIODevice *m_device = nullptr;
};

/**
 * Runs local speech models and decides when they stay loaded. A preload is a promise of an upcoming
 * request: the idle timeout only counts from the last request, never from a pending preload.
 */
class InferenceRuntime : public QObject {
  Q_OBJECT

public:
  struct Transcription {
    LocalEngine engine;
    std::filesystem::path model;
    std::optional<std::string> language;
    std::optional<std::string> initialPrompt;
  };

  explicit InferenceRuntime(QObject *parent = nullptr);
  ~InferenceRuntime() override;

  // std::nullopt keeps models loaded until Vicinae quits, zero releases them after each request
  void setIdleTimeout(std::optional<std::chrono::seconds> timeout);

  void preload(LocalEngine engine, const std::filesystem::path &model);
  void cancelPreload();
  QFuture<TranscriptionResult> transcribe(const Audio::Recording &recording, const Transcription &request);
  void release();

private:
  using Pending = std::shared_ptr<QPromise<TranscriptionResult>>;

  bool ensureRunning();
  void requestStarted();
  void requestFinished();
  bool idle() const { return m_inflight == 0 && m_holds == 0; }
  void rearmIdle();
  void timeOut(const Pending &pending);
  void handleExit(int code, QProcess::ExitStatus status);
  void handleStderr();

  QProcess m_process;
  InferenceBus m_bus;
  inference_gen::RpcTransport m_rpc;
  inference_gen::Client m_client;
  QTimer m_idleTimer;
  std::optional<std::chrono::seconds> m_idleTimeout = std::chrono::seconds{60};
  int m_inflight = 0;
  int m_holds = 0;
  bool m_releasing = false;
  std::vector<Pending> m_pending;
  QByteArray m_stderrBuffer;
};

} // namespace AI
