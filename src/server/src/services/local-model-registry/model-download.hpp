#pragma once
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <QFile>
#include <QNetworkReply>
#include <QObject>
#include <QTimer>
#include <QUrl>
#include "common/types.hpp"

struct ModelDownloadRequest {
  QUrl url;
  std::filesystem::path destination;
  std::string sha256;
  std::uint64_t expectedSize = 0;
  std::optional<QString> bearer;
};

/**
 * Resumable, verified file download.
 *
 * Bytes go to `<destination>.part`. An interrupted download resumes from the partial file with a Range
 * request. Transient network errors and HTTP 429/5xx are retried with backoff, honouring Retry-After.
 * Once complete the file is hashed and renamed into place only if size and SHA-256 match.
 *
 * The object stays alive until the owner deletes it, so late subscribers can read the current state.
 */
class ModelDownload : public QObject, NonCopyable {
  Q_OBJECT

signals:
  void progress(qint64 received, qint64 total);
  void retryScheduled(int attempt, int delaySeconds, const QString &reason);
  void finished(const std::filesystem::path &path);
  void failed(const QString &error);
  void cancelled();

public:
  enum class State : std::uint8_t { Idle, Downloading, WaitingRetry, Verifying, Finished, Failed, Cancelled };

  explicit ModelDownload(ModelDownloadRequest request, QObject *parent = nullptr);
  ~ModelDownload() override;

  void start();
  void cancel();

  State state() const { return m_state; }
  bool isActive() const;
  qint64 bytesReceived() const { return m_received; }
  qint64 bytesTotal() const { return m_total; }
  const QString &error() const { return m_error; }
  const std::filesystem::path &destination() const { return m_request.destination; }
  std::filesystem::path partialPath() const;

private:
  static constexpr int MAX_ATTEMPTS = 6;
  static constexpr auto STALL_TIMEOUT = std::chrono::seconds(30);
  static constexpr auto MAX_RETRY_DELAY = std::chrono::minutes(5);
  static constexpr auto BASE_RETRY_DELAY = std::chrono::seconds(2);

  void sendRequest();
  void handleMetaData();
  void handleReplyFinished();
  void truncatePartial();
  void scheduleRetry(const QString &reason, std::optional<std::chrono::seconds> delay);
  void verifyAndCommit();
  void commit();
  void fail(const QString &error);
  void updateProgress(qint64 received, qint64 total);

  static bool isTransient(QNetworkReply::NetworkError error);
  static std::optional<std::chrono::seconds> retryAfter(const QNetworkReply &reply);

  ModelDownloadRequest m_request;
  State m_state = State::Idle;
  QFile m_file;
  QNetworkReply *m_reply = nullptr;
  QTimer m_retryTimer;
  qint64 m_offset = 0;
  qint64 m_received = 0;
  qint64 m_total = 0;
  int m_attempt = 0;
  QString m_error;
};
