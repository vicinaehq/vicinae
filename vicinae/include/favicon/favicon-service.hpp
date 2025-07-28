#pragma once
#include "favicon/favicon-request.hpp"
#include <QSqlError>
#include <cassert>
#include <qbuffer.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qimage.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qstringview.h>
#include <unordered_map>

class FaviconService : public QObject {
  // TODO: limit cache based on storage size not number of elements
  static constexpr size_t maxCacheCount = 50;

public:
  enum RequesterType { Google, DuckDuckGo };

private:
  inline static FaviconService *_instance = nullptr;

  QSqlDatabase _db;
  RequesterType _requesterType;
  QDir _dataDir;
  std::unordered_map<QString, QPixmap> _cache;

  void handleFetchedFavicon(const QString &domain, const QPixmap &favicon);
  void insertCache(const QString &key, const QPixmap &favicon);
  QPixmap retrieveFromCache(const QString &domain);

public:
  static void initialize(FaviconService *service) { _instance = service; }
  static FaviconService *instance();

  AbstractFaviconRequest *makeRequest(const QString &domain, QObject *parent = nullptr);
  FaviconService(const QString &path, QObject *parent = nullptr);
};
