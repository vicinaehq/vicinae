#pragma once
#include <QObject>
#include <QString>
#include <QUrl>
#include <optional>
#include <unordered_map>
#include <vector>

class QNetworkReply;

class UrlMetadataService : public QObject {
  Q_OBJECT

public:
  struct UrlMetadata {
    std::optional<QString> ogTitle;
    std::optional<QString> ogDescription;
    std::optional<QString> ogImage;
  };

  explicit UrlMetadataService(QObject *parent = nullptr);
  ~UrlMetadataService() override;

  void fetchMetadata(const QString &selectionId, const QUrl &url);

signals:
  void metadataReady(const QString &selectionId, const QString &ogTitle, const QString &ogDescription,
                     const QString &ogImage);

private:
  static constexpr size_t MAX_CACHE_SIZE = 200;
  static constexpr qint64 MAX_BUFFER_SIZE = 64 * 1024;
  static constexpr int TIMEOUT_MS = 10000;

  static UrlMetadata parseMetaTags(const QByteArray &html);
  void abortAll();

  struct InFlightRequest {
    QNetworkReply *reply;
    QByteArray buffer;
    std::vector<QString> selectionIds;
  };

  std::unordered_map<QString, InFlightRequest> m_inFlight;
  std::unordered_map<QString, UrlMetadata> m_cache;
};
