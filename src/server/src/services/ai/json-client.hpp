#pragma once
#include <expected>
#include "common/types.hpp"
#include <qfuturewatcher.h>
#include <QPromise>
#include <QNetworkReply>
#include "glaze/json/write.hpp"

class NetworkManager : NonCopyable {
public:
  static QNetworkAccessManager *manager() {
    static QNetworkAccessManager mgr;
    return &mgr;
  }
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
        promise.addResult(std::unexpected(reply->errorString().toStdString()));
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

  template <typename T> QFuture<Result<T>> get(const QUrl &url) {
    qInfo() << "[GET]" << url;
    QPromise<Result<T>> promise;
    QNetworkRequest req;

    req.setUrl(url);

    auto reply = NetworkManager::manager()->get(req);

    return waitForRequest<T>(std::move(promise), reply);
  }

  template <typename T> QFuture<Result<T>> postMultipart(const QUrl &url, QHttpMultiPart *multipart) {
    QPromise<Result<T>> promise;
    auto future = promise.future();
    QNetworkRequest req;

    req.setUrl(url);
    req.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");

    auto reply = NetworkManager::manager()->post(req, multipart);

    return waitForRequest<T>(std::move(promise), reply);
  }

  template <typename T, typename U> QFuture<Result<T>> post(const QUrl &url, const U &data) {
    qInfo() << "[POST]" << url;
    std::string payload;
    QPromise<Result<T>> promise;
    auto future = promise.future();
    QNetworkRequest req;

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
};
