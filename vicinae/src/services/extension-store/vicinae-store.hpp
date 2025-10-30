#pragma once

#include <QDateTime>
#include <QFuture>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <expected>

namespace VicinaeStore {

/**
 * @brief Represents pagination options for listing extensions
 */
struct ListPaginationOptions {
  qint32 page = 1;
  qint32 limit = 50;
};

/**
 * @brief Represents icon URLs for an extension or command
 */
struct Icons {
  QString light;
  QString dark;

  static Icons fromJson(const QJsonObject &json);
  QJsonObject toJson() const;

  /**
   * @brief Returns the appropriate icon URL based on current theme
   */
  QString themedIcon() const;
};

/**
 * @brief Represents an author/user of an extension
 */
struct Author {
  QString handle;
  QString name;
  QString avatarUrl;
  QString profileUrl;

  static Author fromJson(const QJsonObject &json);
  QJsonObject toJson() const;
};

/**
 * @brief Represents a category with ID and name
 */
struct Category {
  QString id;
  QString name;

  static Category fromJson(const QJsonObject &json);
  QJsonObject toJson() const;
};

/**
 * @brief Represents a command within an extension
 */
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

  /**
   * @brief Returns the appropriate icon URL based on current theme
   */
  QString themedIcon() const;
};

/**
 * @brief Represents a Vicinae extension from the store
 */
struct Extension {
  QString id;
  QString name;
  QString title;
  QString description;
  Author author;
  qint32 downloadCount = 0;
  QString apiVersion;
  QString checksum;
  bool trending = false;
  Icons icons;
  QList<Category> categories;
  QStringList platforms;
  QList<Command> commands;
  QString sourceUrl;
  QString readmeUrl;
  QString downloadUrl;
  QDateTime createdAt;
  QDateTime updatedAt;

  static Extension fromJson(const QJsonObject &json);
  QJsonObject toJson() const;

  /**
   * @brief Returns the appropriate icon URL based on current theme
   */
  QString themedIcon() const;
};

/**
 * @brief Represents pagination metadata from API response
 */
struct Pagination {
  qint32 page = 1;
  qint32 limit = 50;
  qint32 total = 0;
  qint32 totalPages = 0;
  bool hasNext = false;
  bool hasPrev = false;

  static Pagination fromJson(const QJsonObject &json);
  QJsonObject toJson() const;
};

/**
 * @brief Response structure for listing extensions
 */
struct ListResponse {
  QList<Extension> extensions;
  Pagination pagination;

  static ListResponse fromJson(const QJsonDocument &doc);
};

// Result types using std::expected for error handling
using ListResult = std::expected<ListResponse, QString>;
using DownloadExtensionResult = std::expected<QByteArray, QString>;

} // namespace VicinaeStore

/**
 * @brief Service for interacting with the Vicinae extension store
 *
 * Provides methods to browse, search, and download extensions from the
 * Vicinae store. Uses Qt's networking stack with disk caching for improved
 * performance.
 */
class VicinaeStoreService : public QObject {
  Q_OBJECT

public:
  explicit VicinaeStoreService(QObject *parent = nullptr);
  ~VicinaeStoreService() override = default;

  /**
   * @brief Fetches extensions from the store with pagination
   * @param opts Pagination options (page, limit)
   * @return QFuture with ListResult containing extensions and pagination info
   */
  QFuture<VicinaeStore::ListResult>
  fetchExtensions(const VicinaeStore::ListPaginationOptions &opts = {});

  /**
   * @brief Searches extensions by query term
   * @param query Search term
   * @return QFuture with ListResult containing matching extensions
   */
  QFuture<VicinaeStore::ListResult> search(const QString &query);

  /**
   * @brief Downloads an extension from the given URL
   * @param url Download URL for the extension
   * @return QFuture with DownloadExtensionResult containing the ZIP data
   */
  QFuture<VicinaeStore::DownloadExtensionResult>
  downloadExtension(const QUrl &url);

  /**
   * @brief Sets the base URL for the Vicinae store API
   * @param url Base URL (e.g., "http://localhost:3000/v1")
   */
  void setBaseUrl(const QString &url);

  /**
   * @brief Gets the current base URL
   * @return Current base URL
   */
  QString baseUrl() const;

private:
  QNetworkAccessManager *m_networkManager;
  QString m_baseUrl;

  // Default base URL - can be changed via setBaseUrl()
  static constexpr const char *DEFAULT_BASE_URL = "http://localhost:3000/v1";

  /**
   * @brief Performs a GET request and returns the response
   */
  QFuture<std::expected<QByteArray, QString>> get(const QUrl &url);
};
