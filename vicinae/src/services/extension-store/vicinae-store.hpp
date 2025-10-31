#pragma once
#include <QDateTime>
#include <QFuture>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <expected>

namespace VicinaeStore {

struct ListPaginationOptions {
  int page = 1;
  int limit = 50;
};

struct Icons {
  QString light;
  QString dark;

  static Icons fromJson(const QJsonObject &json);
  QJsonObject toJson() const;
  QString themedIcon() const;
};

struct Author {
  QString handle;
  QString name;
  QString avatarUrl;
  QString profileUrl;

  static Author fromJson(const QJsonObject &json);
  QJsonObject toJson() const;
};

struct Category {
  QString id;
  QString name;

  static Category fromJson(const QJsonObject &json);
  QJsonObject toJson() const;
};

struct Command {
  QString id;
  QString name;
  QString title;
  QString subtitle;
  QString description;
  QStringList keywords;
  QString mode;
  bool disabledByDefault = false;
  bool beta = false;
  Icons icons;

  static Command fromJson(const QJsonObject &json);
  QJsonObject toJson() const;
  std::optional<QString> themedIcon() const;
};

struct Extension {
  QString id;
  QString storeId;
  QString name;
  QString title;
  QString description;
  Author author;
  int downloadCount = 0;
  QString apiVersion;
  QString checksum;
  bool trending = false;
  Icons icons;
  std::vector<Category> categories;
  std::vector<QString> platforms;
  std::vector<Command> commands;
  QString sourceUrl;
  QString readmeUrl;
  QString downloadUrl;
  QDateTime createdAt;
  QDateTime updatedAt;

  static Extension fromJson(const QJsonObject &json);
  QJsonObject toJson() const;
  QString themedIcon() const;
};

struct Pagination {
  int page = 1;
  int limit = 50;
  int total = 0;
  int totalPages = 0;
  bool hasNext = false;
  bool hasPrev = false;

  static Pagination fromJson(const QJsonObject &json);
  QJsonObject toJson() const;
};

struct ListResponse {
  QList<Extension> extensions;
  Pagination pagination;

  static ListResponse fromJson(const QJsonDocument &doc);
};

using ListResult = std::expected<ListResponse, QString>;
using DownloadExtensionResult = std::expected<QByteArray, QString>;

} // namespace VicinaeStore

class VicinaeStoreService : public QObject {
public:
  explicit VicinaeStoreService(QObject *parent = nullptr);
  ~VicinaeStoreService() override = default;

  QFuture<VicinaeStore::ListResult> fetchExtensions(const VicinaeStore::ListPaginationOptions &opts = {});

  QFuture<VicinaeStore::ListResult> search(const QString &query);

  QFuture<VicinaeStore::DownloadExtensionResult> downloadExtension(const QUrl &url);

  void setBaseUrl(const QString &url);
  QString baseUrl() const;

private:
  QFuture<std::expected<QByteArray, QString>> get(const QUrl &url);

  QNetworkAccessManager *m_networkManager;
  QString m_baseUrl;
};
