#pragma once
#include "db/database.hpp"
#include "ui/image/url.hpp"
#include <cassert>
#include <expected>
#include <QCache>
#include <QPromise>
#include <memory>
#include <vector>
#include <QThreadPool>
#include <qdir.h>
#include <qfileinfo.h>
#include <qfuture.h>
#include <qimage.h>
#include <qobject.h>
#include <qstringview.h>
#include <unordered_map>

class FaviconService : public QObject {
  static constexpr int MEMORY_CACHE_SIZE = 256;

public:
  using FaviconResponse = std::expected<QImage, QString>;
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
  QCache<QString, QImage> _cache{MEMORY_CACHE_SIZE};
  std::unordered_map<QString, std::vector<std::shared_ptr<QPromise<FaviconResponse>>>> _pending;
  // single thread: every database access and disk write after construction goes through it
  QThreadPool _ioPool;

  void fetch(const QString &domain, QObject *parent);
  void resolve(const QString &domain, const FaviconResponse &response);
  void store(const QString &domain, const QImage &favicon);
  void touch(const QString &domain);

public:
  static std::vector<FaviconServiceData> providers();
  static void initialize(FaviconService *service) { _instance = service; }
  static FaviconService *instance();

  void setService(RequesterType type);
  void setService(const QString &id);

  /**
   * Must be called from the main thread.
   */
  QFuture<FaviconResponse> makeRequest(const QString &domain, QObject *parent = nullptr);
  FaviconService(const std::filesystem::path &path, QObject *parent = nullptr);
};
