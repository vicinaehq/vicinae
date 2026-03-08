#pragma once
#include <algorithm>
#include <expected>
#include "common/types.hpp"
#include <qdir.h>
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

  void setBearer(QString bearer) { m_bearer = std::move(bearer); }
  void setBaseUrl(QString url) { m_baseUrl = std::move(url); }

  template <typename T> QFuture<Result<T>> get(const QUrl &url) {
    qInfo() << "[GET]" << url;
    QPromise<Result<T>> promise;
    auto req = createRequest();

    req.setUrl(url);

    auto reply = NetworkManager::manager()->get(req);

    return waitForRequest<T>(std::move(promise), reply);
  }

  template <typename T> QFuture<Result<T>> post(const QUrl &url, FormData *data) {
    QPromise<Result<T>> promise;
    auto future = promise.future();
    auto req = createRequest();

    req.setUrl(url);

    auto reply = NetworkManager::manager()->post(req, data->multipart());
    data->setParent(reply);

    return waitForRequest<T>(std::move(promise), reply);
  }

  template <typename T, typename U> QFuture<Result<T>> post(const QUrl &url, const U &data) {
    qInfo() << "[POST]" << url;
    std::string payload;
    QPromise<Result<T>> promise;
    auto future = promise.future();
    auto req = createRequest();

    if (auto const error = glz::write_json(data, payload)) {
      promise.addResult(std::unexpected(std::string("Failed to serialize request")));
      promise.finish();
      return future;
    }

    req.setUrl(url);
    req.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");

    auto reply = NetworkManager::manager()->post(req, payload.c_str());

    return waitForRequest<T>(std::move(promise), reply);
  }

private:
  QNetworkRequest createRequest() {
    QNetworkRequest req;

    if (m_bearer) { req.setRawHeader("Authorization", QString("Bearer %1").arg(m_bearer.value()).toUtf8()); }

    return req;
  }

  std::optional<QString> m_baseUrl;
  std::optional<QString> m_bearer;
};
