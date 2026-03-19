#pragma once
#include <algorithm>
#include <expected>
#include "common/types.hpp"
#include <glaze/core/reflect.hpp>
#include <QNetworkDiskCache>
#include <QStandardPaths>
#include <qbitarray.h>
#include <qbytearrayview.h>
#include <qcontainerfwd.h>
#include <qdir.h>
#include <qfuture.h>
#include <qfuturewatcher.h>
#include <QPromise>
#include <QNetworkReply>
#include <qhashfunctions.h>
#include <qhttpmultipart.h>
#include <qlogging.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qobject.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <utility>
#include "glaze/json/write.hpp"

namespace http {

inline QNetworkAccessManager *networkManager() {
  static auto *mgr = [] {
    auto *m = new QNetworkAccessManager;
    auto *cache = new QNetworkDiskCache(m);
    const QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/http";
    QDir().mkpath(cacheDir);
    cache->setCacheDirectory(cacheDir);
    m->setCache(cache);
    return m;
  }();
  return mgr;
}

struct RequestOptions {
  QNetworkRequest::CacheLoadControl cachePolicy = QNetworkRequest::AlwaysNetwork;
  bool allowHttp2 = true;
  std::optional<QString> userAgent;
  std::optional<QString> contentType;
};

/**
 * QHttpMultiPart convenience wrapper
 */
class FormData : public QObject, NonCopyable {
public:
  explicit FormData() : m_mp(new QHttpMultiPart(QHttpMultiPart::FormDataType, this)) {}

  void addFile(QFile *file, const QString &contentType, const QString &fieldName = "file") {
    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentTypeHeader, contentType);
    part.setHeader(QNetworkRequest::ContentDispositionHeader,
                   QString("form-data; name=\"%1\"; filename=\"%2\"")
                       .arg(fieldName)
                       .arg(file->filesystemFileName().filename().c_str()));
    part.setBodyDevice(file);
    file->setParent(m_mp);
    m_mp->append(part);
  }

  void addField(const QString &name, const QByteArray &data) {
    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"%1\"").arg(name));
    part.setBody(data);
    m_mp->append(part);
  }

  QHttpMultiPart *multipart() const { return m_mp; }

private:
  QHttpMultiPart *m_mp;
};

class EventSource : public QObject, NonCopyable {
  Q_OBJECT

signals:
  void dataReceived(const QString &event, QByteArrayView data) const;
  void lineReceived(QByteArrayView line) const;
  void errorOccured(const QString &error) const;
  void finished() const;

public:
  explicit EventSource(QNetworkReply *reply) : m_reply(reply) {
    m_buf.reserve(1024);
    m_reply->setParent(this);
    connect(reply, &QNetworkReply::readyRead, this, &EventSource::handleRead);
    connect(reply, &QNetworkReply::finished, this, &EventSource::handleFinished);
    connect(reply, &QNetworkReply::errorOccurred, this, [this](QNetworkReply::NetworkError error) {
      qDebug() << "completion error";
      emit errorOccured(m_reply->errorString());
    });
  }

  void abort() { m_reply->abort(); }

  /**
   * Underlying response. This class owns the reply.
   */
  QNetworkReply *reply() { return m_reply; }

private:
  void processLines() {
    int index = -1;
    int pos = 0;

    qDebug() << "reading data" << m_buf.toStdString();

    while (pos < m_buf.size()) {
      index = m_buf.indexOf("\n", pos);

      if (index == -1) break;

      auto line = QByteArrayView{m_buf}.slice(pos, index - pos);
      pos = index + 1;

      if (line.empty()) continue;

      qDebug() << "[EVENT-STREAM]" << line;

      emit lineReceived(line);

      if (line.startsWith("event:")) {
        event = QString::fromUtf8(line.slice(7));
      } else if (line.startsWith("data:")) {
        emit dataReceived(event, line.slice(6));
      }
    }

    m_buf.slice(pos);
  }

  void handleFinished() {
    processLines();
    emit finished();
  }

  void handleRead() {
    m_buf.append(m_reply->readAll());
    processLines();
  }

  QString event;
  QByteArray m_buf;
  QNetworkReply *m_reply = nullptr;
};

class Client {
public:
  template <typename T> using Watcher = QFutureWatcher<T>;
  template <typename T> using Result = std::expected<T, std::string>;

  QUrl makeUrl(const QString &path) const {
    if (!m_baseUrl || path.startsWith("http://") || path.startsWith("https://")) return QUrl(path);
    return QUrl(m_baseUrl.value() + path);
  }

  void setBearer(QString bearer) { m_bearer = std::move(bearer); }
  void setBaseUrl(QString url) { m_baseUrl = std::move(url); }
  void setUserAgent(QString agent) { m_userAgent = std::move(agent); }

  template <glz::has_reflect T> QFuture<Result<T>> get(const QString &url, const RequestOptions &opts = {}) {
    auto req = createRequest(url, opts);
    qInfo() << "[GET]" << req.url();
    return parseResponse<T>(waitForReply(networkManager()->get(req)));
  }

  QFuture<Result<QByteArray>> getRaw(const QString &url, const RequestOptions &opts = {}) {
    auto req = createRequest(url, opts);
    qInfo() << "[GET raw]" << req.url();
    return waitForReply(networkManager()->get(req));
  }

  template <glz::has_reflect T> QFuture<Result<T>> post(const QString &url, FormData *data) {
    auto req = createRequest(url);
    qInfo() << "[POST]" << req.url();
    auto *reply = networkManager()->post(req, data->multipart());
    data->setParent(reply);
    return parseResponse<T>(waitForReply(reply));
  }

  template <glz::has_reflect T, glz::has_reflect U>
  QFuture<Result<T>> post(const QString &url, const U &data) {
    std::string payload;
    auto req = createRequest(url);

    if (auto const error = glz::write_json(data, payload)) {
      return QtFuture::makeReadyValueFuture<Result<T>>(
          std::unexpected(std::format("failed to serialize request: {}", glz::format_error(error))));
    }

    req.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");
    qInfo() << "[POST]" << req.url();

    return parseResponse<T>(waitForReply(networkManager()->post(req, payload.c_str())));
  }

  template <glz::has_reflect U> EventSource *postEventSource(const QString &url, const U &payload) {
    auto req = createRequest(url);
    std::string buf;
    qInfo() << "[POST]" << req.url();

    if (auto const error = glz::write_json(payload, buf)) {
      qWarning() << std::format("failed to serialize request: {}", glz::format_error(error));
      return nullptr;
    }

    return new EventSource{networkManager()->post(req, buf.c_str())};
  }

private:
  QNetworkRequest createRequest(const QString &path, const RequestOptions &opts = {}) {
    QNetworkRequest req;

    req.setUrl(makeUrl(path));
    if (m_bearer) { req.setRawHeader("Authorization", QString("Bearer %1").arg(m_bearer.value()).toUtf8()); }

    req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, opts.cachePolicy);
    req.setAttribute(QNetworkRequest::Http2AllowedAttribute, opts.allowHttp2);
    if (opts.userAgent)
      req.setHeader(QNetworkRequest::UserAgentHeader, *opts.userAgent);
    else if (m_userAgent)
      req.setHeader(QNetworkRequest::UserAgentHeader, *m_userAgent);
    if (opts.contentType) req.setHeader(QNetworkRequest::ContentTypeHeader, *opts.contentType);

    return req;
  }

  static QFuture<Result<QByteArray>> waitForReply(QNetworkReply *reply) {
    QPromise<Result<QByteArray>> promise;
    auto future = promise.future();

    QObject::connect(reply, &QNetworkReply::finished, [promise = std::move(promise), reply]() mutable {
      if (reply->error() != QNetworkReply::NoError) {
        const auto reason = reply->readAll();
        QString error = reply->errorString() + "\n" + reason;
        promise.addResult(std::unexpected(error.toStdString()));
      } else {
        promise.addResult(reply->readAll());
      }
      promise.finish();
      reply->deleteLater();
    });

    return future;
  }

  template <typename T> static QFuture<Result<T>> parseResponse(QFuture<Result<QByteArray>> raw) {
    return raw.then([](Result<QByteArray> result) -> Result<T> {
      if (!result) return std::unexpected(result.error());
      T res;
      if (auto const error = glz::read<glz::opts{.error_on_unknown_keys = false}>(res, result->constData())) {
        return std::unexpected(std::string("Failed to parse response: ") +
                               glz::format_error(error, result->constData()));
      }
      return res;
    });
  }

  std::optional<QString> m_baseUrl;
  std::optional<QString> m_bearer;
  std::optional<QString> m_userAgent;
};

} // namespace http
