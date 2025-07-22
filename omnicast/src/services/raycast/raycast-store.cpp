#include "raycast-store.hpp"
#include <qfuture.h>
#include <qnetworkreply.h>
#include <expected>

QFuture<Raycast::ListResult> RaycastStoreService::search(const QString &query) {

  QUrl endpoint = QString("%1/store_listings/search?q=%2").arg(BASE_URL).arg(query);
  QPromise<Raycast::ListResult> promise;
  auto future = promise.future();
  QNetworkRequest request(endpoint);

  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  auto reply = m_net->get(QNetworkRequest(endpoint));

  connect(reply, &QNetworkReply::finished, this, [this, reply, promise = std::move(promise)]() mutable {
    if (reply->error() != QNetworkReply::NoError) {
      promise.addResult(std::unexpected(""));
    } else {
      std::vector<Raycast::Extension> extensions;
      auto data = reply->readAll();
      QJsonParseError error;
      QJsonDocument doc = QJsonDocument::fromJson(data, &error);

      if (error.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << error.errorString();
        promise.addResult(std::unexpected("Failed to parse response"));
        promise.finish();
        reply->deleteLater();
        return;
      }

      auto jsonList = doc.object().value("data").toArray();

      extensions.reserve(jsonList.size());

      for (const auto &result : doc.object().value("data").toArray()) {
        extensions.emplace_back(Raycast::Extension::fromJson(result.toObject()));
      }

      promise.addResult(extensions);
    }

    promise.finish();
    reply->deleteLater();
  });

  return future;
}

QFuture<Raycast::ListResult>
RaycastStoreService::fetchExtensions(const Raycast::ListPaginationOptions &opts) {

  QUrl endpoint =
      QString("%1/store_listings?page=%2&per_page=%3").arg(BASE_URL).arg(opts.page).arg(opts.perPage);
  QPromise<Raycast::ListResult> promise;
  auto future = promise.future();
  QNetworkRequest request(endpoint);

  if (auto it = m_cachedPages.find(opts.page); it != m_cachedPages.end()) {
    qDebug() << "cached page" << opts.page;
    promise.addResult(it->second);
    promise.finish();
    return future;
  }

  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  auto reply = m_net->get(QNetworkRequest(endpoint));

  connect(reply, &QNetworkReply::finished, this, [this, opts, reply, promise = std::move(promise)]() mutable {
    if (reply->error() != QNetworkReply::NoError) {
      promise.addResult(std::unexpected(""));
    } else {
      std::vector<Raycast::Extension> extensions;
      auto data = reply->readAll();
      QJsonParseError error;
      QJsonDocument doc = QJsonDocument::fromJson(data, &error);

      if (error.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << error.errorString();
        promise.addResult(std::unexpected("Failed to parse response"));
        promise.finish();
        reply->deleteLater();
        return;
      }

      auto jsonList = doc.object().value("data").toArray();

      extensions.reserve(jsonList.size());

      for (const auto &result : doc.object().value("data").toArray()) {
        extensions.emplace_back(Raycast::Extension::fromJson(result.toObject()));
      }

      m_cachedPages.insert({opts.page, extensions});
      promise.addResult(extensions);
    }

    promise.finish();
    reply->deleteLater();
  });

  return future;
}

RaycastStoreService::RaycastStoreService() {}
