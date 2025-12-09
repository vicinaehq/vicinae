#include "vicinae-store.hpp"
#include "version.h"
#include "environment.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include <QDir>
#include <QJsonParseError>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPromise>
#include <QStandardPaths>
#include <QtConcurrent/QtConcurrent>

namespace VicinaeStore {

// Icons implementation
Icons Icons::fromJson(const QJsonObject &json) {
  Icons icons;
  icons.light = json["light"].toString();
  icons.dark = json["dark"].toString();
  return icons;
}

QJsonObject Icons::toJson() const {
  QJsonObject json;
  json["light"] = light;
  json["dark"] = dark;
  return json;
}

QString Icons::themedIcon() const {
  const auto &theme = ThemeService::instance().theme();
  if (theme.variant() == ThemeVariant::Dark && !dark.isEmpty()) { return dark; }
  return light.isEmpty() ? dark : light;
}

// Author implementation
Author Author::fromJson(const QJsonObject &json) {
  Author author;
  author.handle = json["handle"].toString();
  author.name = json["name"].toString();
  author.avatarUrl = json["avatarUrl"].toString();
  author.profileUrl = json["profileUrl"].toString();
  return author;
}

QJsonObject Author::toJson() const {
  QJsonObject json;
  json["handle"] = handle;
  json["name"] = name;
  json["avatarUrl"] = avatarUrl;
  json["profileUrl"] = profileUrl;
  return json;
}

// Category implementation
Category Category::fromJson(const QJsonObject &json) {
  Category category;
  category.id = json["id"].toString();
  category.name = json["name"].toString();
  return category;
}

QJsonObject Category::toJson() const {
  QJsonObject json;
  json["id"] = id;
  json["name"] = name;
  return json;
}

// Command implementation
Command Command::fromJson(const QJsonObject &json) {
  Command command;
  command.id = json["id"].toString();
  command.name = json["name"].toString();
  command.title = json["title"].toString();
  command.subtitle = json["subtitle"].toString();
  command.description = json["description"].toString();

  const QJsonArray keywordsArray = json["keywords"].toArray();
  for (const auto &keyword : keywordsArray) {
    command.keywords.append(keyword.toString());
  }

  command.mode = json["mode"].toString();
  command.disabledByDefault = json["disabledByDefault"].toBool(false);
  command.beta = json["beta"].toBool(false);
  command.icons = Icons::fromJson(json["icons"].toObject());

  return command;
}

QJsonObject Command::toJson() const {
  QJsonObject json;
  json["id"] = id;
  json["name"] = name;
  json["title"] = title;
  json["subtitle"] = subtitle;
  json["description"] = description;

  QJsonArray keywordsArray;
  for (const auto &keyword : keywords) {
    keywordsArray.append(keyword);
  }
  json["keywords"] = keywordsArray;

  json["mode"] = mode;
  json["disabledByDefault"] = disabledByDefault;
  json["beta"] = beta;
  json["icons"] = icons.toJson();

  return json;
}

std::optional<QString> Command::themedIcon() const {
  const auto &theme = ThemeService::instance().theme();
  if (theme.variant() == ThemeVariant::Dark && !icons.dark.isEmpty()) { return icons.dark; }
  if (theme.variant() == ThemeVariant::Light && !icons.light.isEmpty()) { return icons.light; }

  return {};
}

// Extension implementation
Extension Extension::fromJson(const QJsonObject &json) {
  Extension extension;
  extension.storeId = json["id"].toString();
  extension.name = json["name"].toString();
  extension.title = json["title"].toString();
  extension.description = json["description"].toString();
  extension.author = Author::fromJson(json["author"].toObject());
  extension.downloadCount = json["downloadCount"].toInt(0);
  extension.apiVersion = json["apiVersion"].toString();
  extension.checksum = json["checksum"].toString();
  extension.trending = json["trending"].toBool(false);
  extension.icons = Icons::fromJson(json["icons"].toObject());
  extension.id = QString("store.vicinae.%1").arg(extension.name);

  const QJsonArray categoriesArray = json["categories"].toArray();
  extension.categories.reserve(categoriesArray.size());
  for (const auto &categoryValue : categoriesArray) {
    extension.categories.emplace_back(Category::fromJson(categoryValue.toObject()));
  }

  const QJsonArray platformsArray = json["platforms"].toArray();
  extension.platforms.reserve(platformsArray.size());
  for (const auto &platform : platformsArray) {
    extension.platforms.emplace_back(platform.toString());
  }

  const QJsonArray commandsArray = json["commands"].toArray();
  extension.commands.reserve(commandsArray.size());
  for (const auto &commandValue : commandsArray) {
    extension.commands.emplace_back(Command::fromJson(commandValue.toObject()));
  }

  extension.sourceUrl = json["sourceUrl"].toString();
  extension.readmeUrl = json["readmeUrl"].toString();
  extension.downloadUrl = json["downloadUrl"].toString();
  extension.createdAt = QDateTime::fromString(json["createdAt"].toString(), Qt::ISODate);
  extension.updatedAt = QDateTime::fromString(json["updatedAt"].toString(), Qt::ISODate);

  return extension;
}

QJsonObject Extension::toJson() const {
  QJsonObject json;
  json["id"] = id;
  json["name"] = name;
  json["title"] = title;
  json["description"] = description;
  json["author"] = author.toJson();
  json["downloadCount"] = downloadCount;
  json["apiVersion"] = apiVersion;
  json["checksum"] = checksum;
  json["trending"] = trending;
  json["icons"] = icons.toJson();

  QJsonArray categoriesArray;
  for (const auto &category : categories) {
    categoriesArray.append(category.toJson());
  }
  json["categories"] = categoriesArray;

  QJsonArray platformsArray;
  for (const auto &platform : platforms) {
    platformsArray.append(platform);
  }
  json["platforms"] = platformsArray;

  QJsonArray commandsArray;
  for (const auto &command : commands) {
    commandsArray.append(command.toJson());
  }
  json["commands"] = commandsArray;

  json["sourceUrl"] = sourceUrl;
  json["readmeUrl"] = readmeUrl;
  json["downloadUrl"] = downloadUrl;
  json["createdAt"] = createdAt.toString(Qt::ISODate);
  json["updatedAt"] = updatedAt.toString(Qt::ISODate);

  return json;
}

QString Extension::themedIcon() const { return icons.themedIcon(); }

// Pagination implementation
Pagination Pagination::fromJson(const QJsonObject &json) {
  Pagination pagination;
  pagination.page = json["page"].toInt(1);
  pagination.limit = json["limit"].toInt(50);
  pagination.total = json["total"].toInt(0);
  pagination.totalPages = json["totalPages"].toInt(0);
  pagination.hasNext = json["hasNext"].toBool(false);
  pagination.hasPrev = json["hasPrev"].toBool(false);
  return pagination;
}

QJsonObject Pagination::toJson() const {
  QJsonObject json;
  json["page"] = page;
  json["limit"] = limit;
  json["total"] = total;
  json["totalPages"] = totalPages;
  json["hasNext"] = hasNext;
  json["hasPrev"] = hasPrev;
  return json;
}

// ListResponse implementation
ListResponse ListResponse::fromJson(const QJsonDocument &doc) {
  ListResponse response;
  const QJsonObject root = doc.object();

  const QJsonArray extensionsArray = root["extensions"].toArray();
  for (const auto &extensionValue : extensionsArray) {
    response.extensions.append(Extension::fromJson(extensionValue.toObject()));
  }

  response.pagination = Pagination::fromJson(root["pagination"].toObject());

  return response;
}

} // namespace VicinaeStore

// VicinaeStoreService implementation
//
VicinaeStoreService::VicinaeStoreService(QObject *parent)
    : QObject(parent), m_networkManager(new QNetworkAccessManager(this)),
      m_baseUrl(Environment::vicinaeApiBaseUrl()) {

  // Set up disk cache for better performance
  auto cache = new QNetworkDiskCache(this);
  const QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/vicinae-store";
  QDir().mkpath(cacheDir);
  cache->setCacheDirectory(cacheDir);
  m_networkManager->setCache(cache);

  // Disable HTTP/2 to avoid timeout issues (similar to Raycast implementation)
  m_networkManager->setStrictTransportSecurityEnabled(false);
}

void VicinaeStoreService::setBaseUrl(const QString &url) { m_baseUrl = url; }

QString VicinaeStoreService::baseUrl() const { return m_baseUrl; }

QFuture<std::expected<QByteArray, QString>> VicinaeStoreService::get(const QUrl &url) {
  QPromise<std::expected<QByteArray, QString>> promise;
  QFuture<std::expected<QByteArray, QString>> future = promise.future();
  promise.start();

  QNetworkRequest request(url);
  request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
  request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader, userAgent());

  QNetworkReply *reply = m_networkManager->get(request);

  connect(reply, &QNetworkReply::finished, this, [reply, promise = std::move(promise)]() mutable {
    if (reply->error() != QNetworkReply::NoError) {
      promise.addResult(
          std::unexpected(QString("Network error: %1 (%2)").arg(reply->errorString()).arg(reply->error())));
    } else {
      promise.addResult(reply->readAll());
    }
    promise.finish();
    reply->deleteLater();
  });

  return future;
}

QFuture<VicinaeStore::ListResult>
VicinaeStoreService::fetchExtensions(const VicinaeStore::ListPaginationOptions &opts) {
  const QString urlString =
      QString("%1/store/list?page=%2&limit=%3").arg(m_baseUrl).arg(opts.page).arg(opts.limit);
  const QUrl url(urlString);

  return get(url).then([](std::expected<QByteArray, QString> result) -> VicinaeStore::ListResult {
    if (!result.has_value()) { return std::unexpected(result.error()); }

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(*result, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
      return std::unexpected(QString("JSON parse error: %1").arg(parseError.errorString()));
    }

    return VicinaeStore::ListResponse::fromJson(doc);
  });
}

QFuture<VicinaeStore::ListResult> VicinaeStoreService::search(const QString &query) {
  const QString urlString = QString("%1/store/search?q=%2").arg(m_baseUrl).arg(query);
  const QUrl url(urlString);

  return get(url).then([](std::expected<QByteArray, QString> result) -> VicinaeStore::ListResult {
    if (!result.has_value()) { return std::unexpected(result.error()); }

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(*result, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
      return std::unexpected(QString("JSON parse error: %1").arg(parseError.errorString()));
    }

    return VicinaeStore::ListResponse::fromJson(doc);
  });
}

QFuture<VicinaeStore::DownloadExtensionResult> VicinaeStoreService::downloadExtension(const QUrl &url) {
  return get(url).then(
      [](std::expected<QByteArray, QString> result) -> VicinaeStore::DownloadExtensionResult {
        if (!result.has_value()) { return std::unexpected(result.error()); }
        return *result;
      });
}

QString VicinaeStoreService::userAgent() { return QString("vicinae/%1").arg(VICINAE_GIT_TAG); }
