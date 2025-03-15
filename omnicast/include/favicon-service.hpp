#pragma once

#include "favicon-fetcher.hpp"
#include <QSqlError>
#include <cassert>
#include <qbuffer.h>
#include <qimage.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qstringview.h>

class FaviconService;

class FaviconService : public QObject {
public:
  enum RequesterType { Google, DuckDuckGo };
  inline static FaviconService *_instance = nullptr;

private:
  QSqlDatabase _db;
  RequesterType _requesterType;

  void handleFetchedFavicon(const QString &domain, const QPixmap &favicon) {
    qDebug() << "cached favicon of size" << favicon.size() << "for domain" << domain;
    QPixmapCache::insert(domain, favicon);

    QSqlQuery query(_db);
    QByteArray serializedData;

    {
      QBuffer buffer(&serializedData);

      buffer.open(QIODevice::WriteOnly);

      if (!favicon.save(&buffer, "PNG")) {
        qDebug() << "favicon DB: failed to serialize data";
        return;
      }
    }

    query.prepare(R"(
		INSERT INTO favicon (id, size, data)
		VALUES (:id, :size, :data)
	)");
    query.bindValue(":id", domain);
    query.bindValue(":size", favicon.width());
    query.bindValue(":data", serializedData);

    if (!query.exec()) {
      qDebug() << "Favicon DB: failed to insert favicon: " << query.lastError();
      return;
    }
  }

  QPixmap retrieveFromCache(const QString &domain) {
    QPixmap pm;

    if (QPixmapCache::find(domain, &pm)) { return pm; }

    QSqlQuery query(_db);

    query.prepare("UPDATE favicon SET last_used_at = unixepoch() WHERE id = :domain RETURNING data");
    query.bindValue(":domain", domain);

    if (!query.exec()) {
      qDebug() << "Favicon DB: cache retrieval query failed" << query.lastError();
      return {};
    }

    if (query.next()) {
      qDebug() << "DB cache hit" << domain;
      pm.loadFromData(query.value(0).toByteArray());
      QPixmapCache::insert(domain, pm);
    }

    return pm;
  }

public:
  static void initialize(FaviconService *service) { _instance = service; }
  static FaviconService *instance() {
    assert(_instance && "FaviconService::instance() called before FaviconService::initialize()");
    return _instance;
  }

  FaviconRequest *makeRequest(const QString &domain, QObject *parent = nullptr) {
    if (auto pix = retrieveFromCache(domain); !pix.isNull()) {
      return new CachedFaviconRequest(domain, pix, parent);
    }

    FaviconRequest *requester = nullptr;

    switch (_requesterType) {
    case Google:
      requester = new GoogleFaviconRequester(domain, parent);
      break;
    default:
      requester = new DummyFaviconRequest(domain, parent);
    }

    connect(requester, &FaviconRequest::finished, this,
            [this, domain](const QPixmap &favicon) { handleFetchedFavicon(domain, favicon); });

    return requester;
  }

  FaviconService(const QString &path, QObject *parent = nullptr)
      : QObject(parent), _db(QSqlDatabase::addDatabase("QSQLITE", "favicon")),
        _requesterType(RequesterType::Google) {

    _db.setDatabaseName(path);

    if (!_db.open()) {
      qDebug() << "Failed to open favicon DB" << _db.lastError();
      return;
    }

    QSqlQuery query(_db);

    bool ok = query.exec(R"(
		CREATE TABLE IF NOT EXISTS favicon (
			id TEXT PRIMARY KEY,
			size INTEGER,
			data BLOB NOT NULL,
			created_at INTEGER DEFAULT (unixepoch()),
			last_used_at INTEGER DEFAULT (unixepoch()),
			updated_at INTEGER
		);
	)");

    if (!ok) { qDebug() << "Failed to init favicon database:" << query.lastError(); }
  }
};
