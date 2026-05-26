#pragma once
#include "db/database.hpp"
#include "ui/image/url.hpp"
#include <cassert>
#include <expected>
#include <qbuffer.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qfuture.h>
#include <qimage.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qstringview.h>
#include <unordered_map>

class FaviconService : public QObject {
  static constexpr size_t maxCacheCount = 50;

public:
  using FaviconResponse = std::expected<QPixmap, QString>;
  enum RequesterType { None, Google, Twenty, DuckDuckGo };

  struct FaviconServiceData {
    QString id;
    QString name;
    ImageURL icon;
    RequesterType type;
  };

private:
  inline static FaviconService *_instance = nullptr;

  db::Database _db;
  RequesterType _requesterType;
  QDir _dataDir;
  std::unordered_map<QString, QPixmap> _cache;

  void handleFetchedFavicon(const QString &domain, const QPixmap &favicon);
  void insertCache(const QString &key, const QPixmap &favicon);
  QPixmap retrieveFromCache(const QString &domain);

public:
  static std::vector<FaviconServiceData> providers();
  static void initialize(FaviconService *service) { _instance = service; }
  static FaviconService *instance();

  void setService(RequesterType type);
  void setService(const QString &id);

  QFuture<FaviconResponse> makeRequest(const QString &domain, QObject *parent = nullptr);
  FaviconService(const std::filesystem::path &path, QObject *parent = nullptr);
};
