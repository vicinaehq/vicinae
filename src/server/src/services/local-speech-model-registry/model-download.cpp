#include "model-download.hpp"
#include <algorithm>
#include <cmath>
#include <system_error>
#include <utility>
#include <QCryptographicHash>
#include <QNetworkRequest>
#include <QtConcurrent/QtConcurrent>
#include <qlogging.h>
#include "internal/http-client.hpp"

namespace fs = std::filesystem;

namespace {

constexpr int HTTP_OK = 200;
constexpr int HTTP_PARTIAL_CONTENT = 206;
constexpr int HTTP_RANGE_NOT_SATISFIABLE = 416;
constexpr int HTTP_TOO_MANY_REQUESTS = 429;
constexpr int HTTP_SERVER_ERROR = 500;

int httpStatus(const QNetworkReply &reply) {
  return reply.attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
}

QByteArray sha256Hex(const QString &path) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly)) return {};
  QCryptographicHash hash(QCryptographicHash::Sha256);
  if (!hash.addData(&file)) return {};
  return hash.result().toHex();
}

} // namespace

ModelDownload::ModelDownload(ModelDownloadRequest request, QObject *parent)
    : QObject(parent), m_request(std::move(request)) {
  m_retryTimer.setSingleShot(true);
  connect(&m_retryTimer, &QTimer::timeout, this, &ModelDownload::sendRequest);
}

ModelDownload::~ModelDownload() {
  if (m_reply) {
    m_reply->disconnect(this);
    m_reply->abort();
    m_reply->deleteLater();
  }
}

bool ModelDownload::isActive() const {
  return m_state == State::Downloading || m_state == State::WaitingRetry || m_state == State::Verifying;
}

fs::path ModelDownload::partialPath() const {
  auto path = m_request.destination;
  path += ".part";
  return path;
}

void ModelDownload::start() {
  if (m_state != State::Idle) return;

  std::error_code ec;
  fs::create_directories(m_request.destination.parent_path(), ec);
  if (ec) {
    fail(tr("Could not create %1: %2")
             .arg(m_request.destination.parent_path().c_str())
             .arg(ec.message().c_str()));
    return;
  }

  m_file.setFileName(QString::fromStdString(partialPath().string()));
  if (!m_file.open(QIODevice::ReadWrite | QIODevice::Append)) {
    fail(tr("Could not open %1 for writing: %2").arg(m_file.fileName(), m_file.errorString()));
    return;
  }

  m_offset = m_file.size();
  m_total = static_cast<qint64>(m_request.expectedSize);

  if (m_request.expectedSize > 0 && std::cmp_greater(m_offset, m_request.expectedSize)) truncatePartial();

  if (m_request.expectedSize > 0 && std::cmp_equal(m_offset, m_request.expectedSize)) {
    verifyAndCommit();
    return;
  }

  sendRequest();
}

void ModelDownload::cancel() {
  if (!isActive()) return;

  const auto previous = m_state;
  m_state = State::Cancelled;
  m_retryTimer.stop();

  if (previous == State::Downloading && m_reply) {
    m_reply->abort();
    return;
  }

  m_file.close();
  emit cancelled();
}

void ModelDownload::sendRequest() {
  if (m_state == State::Cancelled || m_state == State::Failed) return;

  m_state = State::Downloading;
  m_offset = m_file.size();

  QNetworkRequest req(m_request.url);
  req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
  req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
  req.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);
  req.setTransferTimeout(STALL_TIMEOUT);
  if (m_request.bearer) {
    req.setRawHeader("Authorization", QString("Bearer %1").arg(*m_request.bearer).toUtf8());
  }
  if (m_offset > 0) { req.setRawHeader("Range", QString("bytes=%1-").arg(m_offset).toUtf8()); }

  qInfo() << "[download]" << m_request.url.toString() << "offset" << m_offset << "attempt" << m_attempt + 1;

  m_reply = http::networkManager()->get(req);
  connect(m_reply, &QNetworkReply::metaDataChanged, this, &ModelDownload::handleMetaData);
  connect(m_reply, &QNetworkReply::readyRead, this, [this]() { m_file.write(m_reply->readAll()); });
  connect(m_reply, &QNetworkReply::downloadProgress, this, &ModelDownload::updateProgress);
  connect(m_reply, &QNetworkReply::finished, this, &ModelDownload::handleReplyFinished);
}

void ModelDownload::handleMetaData() {
  const int status = httpStatus(*m_reply);
  if (status == 0 || (status >= 300 && status < 400)) return;

  if (status == HTTP_OK && m_offset > 0) {
    qWarning() << "[download] server ignored range request, restarting from scratch";
    truncatePartial();
  }

  if (m_total <= 0 && (status == HTTP_OK || status == HTTP_PARTIAL_CONTENT)) {
    const auto length = m_reply->header(QNetworkRequest::ContentLengthHeader);
    if (length.isValid()) m_total = m_offset + length.toLongLong();
  }
}

void ModelDownload::truncatePartial() {
  m_file.resize(0);
  m_offset = 0;
  m_received = 0;
}

void ModelDownload::updateProgress(qint64 received, qint64 total) {
  m_received = m_offset + received;
  if (m_total <= 0 && total > 0) m_total = m_offset + total;
  emit progress(m_received, m_total > 0 ? m_total : -1);
}

void ModelDownload::handleReplyFinished() {
  auto *reply = m_reply;
  m_reply = nullptr;
  reply->deleteLater();

  if (m_state == State::Cancelled) {
    m_file.close();
    emit cancelled();
    return;
  }

  if (reply->error() == QNetworkReply::NoError) m_file.write(reply->readAll());

  if (m_file.error() != QFileDevice::NoError) {
    fail(tr("Could not write %1: %2").arg(m_file.fileName(), m_file.errorString()));
    return;
  }

  const int status = httpStatus(*reply);

  if (reply->error() != QNetworkReply::NoError) {
    if (status == HTTP_RANGE_NOT_SATISFIABLE) {
      truncatePartial();
      scheduleRetry(tr("Server could not resume the download"), std::chrono::seconds(0));
      return;
    }
    if (status == HTTP_TOO_MANY_REQUESTS) {
      scheduleRetry(tr("Rate limited by the server"), retryAfter(*reply));
      return;
    }
    if (status >= HTTP_SERVER_ERROR || isTransient(reply->error())) {
      scheduleRetry(reply->errorString(), retryAfter(*reply));
      return;
    }
    fail(reply->errorString());
    return;
  }

  verifyAndCommit();
}

void ModelDownload::scheduleRetry(const QString &reason, std::optional<std::chrono::seconds> delay) {
  ++m_attempt;
  if (m_attempt >= MAX_ATTEMPTS) {
    fail(tr("Gave up after %n attempt(s): %1", nullptr, m_attempt).arg(reason));
    return;
  }

  const auto backoff =
      std::chrono::seconds(static_cast<long>(BASE_RETRY_DELAY.count() * std::pow(2, m_attempt - 1)));
  const auto wait = std::min<std::chrono::seconds>(delay.value_or(backoff), MAX_RETRY_DELAY);

  qWarning() << "[download] retrying in" << wait.count() << "s:" << reason;
  m_state = State::WaitingRetry;
  emit retryScheduled(m_attempt, static_cast<int>(wait.count()), reason);
  m_retryTimer.start(wait);
}

void ModelDownload::verifyAndCommit() {
  m_state = State::Verifying;

  m_file.flush();
  const auto size = m_file.size();
  m_file.close();

  if (m_request.expectedSize > 0 && std::cmp_not_equal(size, m_request.expectedSize)) {
    std::error_code ec;
    fs::remove(partialPath(), ec);
    fail(tr("Downloaded file has the wrong size (%1 instead of %2 bytes)")
             .arg(size)
             .arg(m_request.expectedSize));
    return;
  }

  if (m_request.sha256.empty()) {
    commit();
    return;
  }

  QtConcurrent::run(sha256Hex, m_file.fileName()).then(this, [this](const QByteArray &digest) {
    if (m_state != State::Verifying) return;

    if (digest.compare(QByteArray::fromStdString(m_request.sha256), Qt::CaseInsensitive) != 0) {
      std::error_code ec;
      fs::remove(partialPath(), ec);
      fail(tr("Checksum mismatch, the downloaded file was discarded"));
      return;
    }

    commit();
  });
}

void ModelDownload::commit() {
  std::error_code ec;
  fs::remove(m_request.destination, ec);
  fs::rename(partialPath(), m_request.destination, ec);
  if (ec) {
    fail(tr("Could not move the download into place: %1").arg(ec.message().c_str()));
    return;
  }

  m_state = State::Finished;
  emit finished(m_request.destination);
}

void ModelDownload::fail(const QString &error) {
  if (m_state == State::Failed || m_state == State::Cancelled) return;
  m_retryTimer.stop();
  if (m_file.isOpen()) m_file.close();
  m_error = error;
  m_state = State::Failed;
  qWarning() << "[download] failed:" << error;
  emit failed(error);
}

bool ModelDownload::isTransient(QNetworkReply::NetworkError error) {
  switch (error) {
  case QNetworkReply::TimeoutError:
  case QNetworkReply::RemoteHostClosedError:
  case QNetworkReply::ConnectionRefusedError:
  case QNetworkReply::HostNotFoundError:
  case QNetworkReply::TemporaryNetworkFailureError:
  case QNetworkReply::NetworkSessionFailedError:
  case QNetworkReply::ProxyConnectionClosedError:
  case QNetworkReply::ProxyTimeoutError:
  case QNetworkReply::ContentReSendError:
  case QNetworkReply::ServiceUnavailableError:
  case QNetworkReply::InternalServerError:
  case QNetworkReply::UnknownNetworkError:
  case QNetworkReply::UnknownServerError:
    return true;
  default:
    return false;
  }
}

std::optional<std::chrono::seconds> ModelDownload::retryAfter(const QNetworkReply &reply) {
  bool ok = false;

  if (reply.hasRawHeader("Retry-After")) {
    const auto seconds = reply.rawHeader("Retry-After").trimmed().toLongLong(&ok);
    if (ok && seconds >= 0) return std::chrono::seconds(seconds);
  }

  // IETF draft format used by Hugging Face: RateLimit: "policy";r=0;t=<seconds until reset>
  if (reply.hasRawHeader("RateLimit")) {
    for (const auto &field : reply.rawHeader("RateLimit").split(';')) {
      const auto trimmed = field.trimmed();
      if (!trimmed.startsWith("t=")) continue;
      const auto seconds = trimmed.mid(2).toLongLong(&ok);
      if (ok && seconds >= 0) return std::chrono::seconds(seconds);
    }
  }

  return std::nullopt;
}
