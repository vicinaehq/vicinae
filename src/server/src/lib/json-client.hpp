#pragma once
#include <algorithm>
#include <expected>
#include "common/types.hpp"
#include <glaze/core/reflect.hpp>
#include <QNetworkDiskCache>
#include <QStandardPaths>
#include <qdir.h>
#include <qfuture.h>
#include <qfuturewatcher.h>
#include <QPromise>
#include <QNetworkReply>
#include <qhttpmultipart.h>
#include <qnetworkrequest.h>
#include <qobject.h>
#include <qstringview.h>
#include <utility>
#include "glaze/json/write.hpp"

class NetworkManager : NonCopyable {
public:
  static QNetworkAccessManager *manager() {
    static QNetworkAccessManager mgr;
    return &mgr;
  }

  static void enableDiskCache(const QString &subdir) {
    auto *mgr = manager();
    if (mgr->cache()) return;

    auto *cache = new QNetworkDiskCache(mgr);
    const QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/" + subdir;
    QDir().mkpath(cacheDir);
    cache->setCacheDirectory(cacheDir);
    mgr->setCache(cache);
  }
};

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

class JsonClient {
public:
  template <typename T> using Watcher = QFutureWatcher<T>;
  template <typename T> using Result = std::expected<T, std::string>;

  QUrl makeUrl(const QString &path) const {
    if (!m_baseUrl) return QUrl(path);
    return QUrl(m_baseUrl.value() + path);
  }

  void setBearer(QString bearer) { m_bearer = std::move(bearer); }
  void setBaseUrl(QString url) { m_baseUrl = std::move(url); }

  template <glz::has_reflect T> QFuture<Result<T>> get(const QString &url, const RequestOptions &opts = {}) {
    QPromise<Result<T>> promise;
    auto req = createRequest(url, opts);
    qInfo() << "[GET]" << req.url();
    auto reply = NetworkManager::manager()->get(req);

    return waitForRequest<T>(std::move(promise), reply);
  }

  QFuture<Result<QByteArray>> getRaw(const QString &url, const RequestOptions &opts = {}) {
    QPromise<Result<QByteArray>> promise;
    auto future = promise.future();
    auto req = createRequest(url, opts);
    qInfo() << "[GET raw]" << req.url();
    auto reply = NetworkManager::manager()->get(req);

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

  template <glz::has_reflect T> QFuture<Result<T>> post(const QString &url, FormData *data) {
    QPromise<Result<T>> promise;
    auto future = promise.future();
    auto req = createRequest(url);
    qInfo() << "[POST]" << req.url();
    auto reply = NetworkManager::manager()->post(req, data->multipart());

    data->setParent(reply);

    return waitForRequest<T>(std::move(promise), reply);
  }

  template <glz::has_reflect T, glz::has_reflect U>
  QFuture<Result<T>> post(const QString &url, const U &data) {
    std::string payload;
    auto req = createRequest(url);

    if (auto const error = glz::write_json(data, payload)) {
      return QtFuture::makeReadyValueFuture<Result<T>>(
          std::unexpected(std::format("Failed to serialize request: {}", glz::format_error(error))));
    }

    QPromise<Result<T>> promise;
    auto future = promise.future();

    req.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");
    qInfo() << "[POST]" << req.url();

    auto reply = NetworkManager::manager()->post(req, payload.c_str());

    return waitForRequest<T>(std::move(promise), reply);
  }

private:
  QNetworkRequest createRequest(const QString &path, const RequestOptions &opts = {}) {
    QNetworkRequest req;

    req.setUrl(makeUrl(path));
    if (m_bearer) { req.setRawHeader("Authorization", QString("Bearer %1").arg(m_bearer.value()).toUtf8()); }

    req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, opts.cachePolicy);
    req.setAttribute(QNetworkRequest::Http2AllowedAttribute, opts.allowHttp2);
    if (opts.userAgent) req.setHeader(QNetworkRequest::UserAgentHeader, *opts.userAgent);
    if (opts.contentType) req.setHeader(QNetworkRequest::ContentTypeHeader, *opts.contentType);

    return req;
  }

  template <typename T>
  static QFuture<std::expected<T, std::string>> waitForRequest(QPromise<Result<T>> promise,
                                                               QNetworkReply *reply) {
    auto future = promise.future();

    QObject::connect(reply, &QNetworkReply::finished, [promise = std::move(promise), reply]() mutable {
      if (reply->error() != QNetworkReply::NoError) {
        const auto reason = reply->readAll();
        QString error = reply->errorString() + "\n" + reason;

        promise.addResult(std::unexpected(error.toStdString()));
        promise.finish();
        reply->deleteLater();
        return;
      }

      auto const data = reply->readAll();
      reply->deleteLater();
      T res;

      if (auto const error = glz::read<glz::opts{.error_on_unknown_keys = false}>(res, data.constData())) {
        promise.addResult(std::unexpected(std::string("Failed to parse response: ") +
                                          glz::format_error(error, data.constData())));
        promise.finish();
        return;
      }

      promise.addResult(std::move(res));
      promise.finish();
    });

    return future;
  }

  std::optional<QString> m_baseUrl;
  std::optional<QString> m_bearer;
};
