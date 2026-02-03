#include "raycast-store.hpp"
#include <qfuture.h>
#include <qnetworkdiskcache.h>
#include <qnetworkreply.h>
#include <expected>
#include <qstandardpaths.h>

QNetworkRequest RaycastStoreService::createJsonApiRequest(const QUrl &url) {
  QNetworkRequest request(url);

  // WORKAROUND: Explicitly disable HTTP/2.
  // Qt's default HTTP/2 implementation hangs and causes a ~25-second timeout on the first
  // request. Forcing HTTP/1.1 resolves it.
  request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);

  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
  return request;
}

QFuture<Raycast::ListResult> RaycastStoreService::search(const QString &query) {

  QUrl endpoint = QString("%1/store_listings/search?q=%2").arg(BASE_URL).arg(query);
  QPromise<Raycast::ListResult> promise;
  auto future = promise.future();
  QNetworkRequest request = createJsonApiRequest(endpoint);

  auto reply = m_net->get(request);

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

QFuture<Raycast::DownloadExtensionResult> RaycastStoreService::downloadExtension(const QUrl &url) {
  QNetworkRequest request(url);
  QPromise<Raycast::DownloadExtensionResult> promise;
  auto future = promise.future();
  auto reply = m_net->get(request);

  request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
  request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);

  connect(reply, &QNetworkReply::finished, this, [this, reply, promise = std::move(promise)]() mutable {
    if (reply->error() != QNetworkReply::NoError) {
      promise.addResult(std::unexpected("Failed to fetch"));
    } else {
      promise.addResult(reply->readAll());
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
  const QNetworkRequest request = createJsonApiRequest(endpoint);

  if (auto it = m_cachedPages.find(opts.page); it != m_cachedPages.end()) {
    promise.addResult(it->second);
    promise.finish();
    return future;
  }

  auto reply = m_net->get(request);

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

RaycastStoreService::RaycastStoreService() {
  auto diskCache = new QNetworkDiskCache;
  auto cacheDir =
      QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QString("/omnicast/raycast-store");

  diskCache->setCacheDirectory(cacheDir);
  m_net->setCache(diskCache);
}
