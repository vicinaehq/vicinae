#include "favicon/favicon-service.hpp"
#include "favicon/cached-favicon-request.hpp"
#include "favicon/dummy-favicon-request.hpp"
#include "favicon/google-favicon-request.hpp"

FaviconService *FaviconService::instance() {
  assert(_instance && "FaviconService::instance() called before FaviconService::initialize()");
  return _instance;
}

void FaviconService::insertCache(const QString &key, const QPixmap &favicon) {
  if (_cache.size() > maxCacheCount) { _cache.erase(_cache.begin()); }

  _cache.insert({key, favicon});
}

void FaviconService::handleFetchedFavicon(const QString &domain, const QPixmap &favicon) {
  insertCache(domain, favicon);

  if (!favicon.save(_dataDir.filePath(domain), "PNG")) {
    qDebug() << "Failed to save favicon on disk";
    return;
  }

  QSqlQuery query(_db);

  query.prepare(R"(
		INSERT INTO favicon (id, size)
		VALUES (:id, :size)
	)");
  query.bindValue(":id", domain);
  query.bindValue(":size", favicon.width());

  if (!query.exec()) {
    qDebug() << "Favicon DB: failed to insert favicon: " << query.lastError();
    return;
  }
}

QPixmap FaviconService::retrieveFromCache(const QString &domain) {
  if (auto it = _cache.find(domain); it != _cache.end()) { return it->second; }

  QPixmap pm;
  QSqlQuery query(_db);

  query.prepare("UPDATE favicon SET last_used_at = unixepoch() WHERE id = :domain;");
  query.bindValue(":domain", domain);

  if (!query.exec()) { qDebug() << "Favicon DB: failed to update last_used_at" << query.lastError(); }

  QFile favicon(_dataDir.filePath(domain));

  if (favicon.open(QIODevice::ReadOnly)) {
    pm.loadFromData(favicon.readAll());
    insertCache(domain, pm);
  }

  return pm;
}

AbstractFaviconRequest *FaviconService::makeRequest(const QString &domain, QObject *parent) {
  if (auto pix = retrieveFromCache(domain); !pix.isNull()) {
    return new CachedFaviconRequest(domain, pix, parent);
  }

  AbstractFaviconRequest *requester = nullptr;

  switch (_requesterType) {
  case Google:
    requester = new GoogleFaviconRequester(domain, parent);
    break;
  default:
    requester = new DummyFaviconRequest(domain, parent);
  }

  connect(requester, &AbstractFaviconRequest::finished, this,
          [this, domain](const QPixmap &favicon) { handleFetchedFavicon(domain, favicon); });

  return requester;
}

FaviconService::FaviconService(const QString &path, QObject *parent)
    : QObject(parent), _db(QSqlDatabase::addDatabase("QSQLITE", "favicon")),
      _requesterType(RequesterType::Google) {
  _dataDir = QFileInfo(path).dir().filePath("favicon-data");
  _dataDir.mkpath(_dataDir.path());
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
			created_at INTEGER DEFAULT (unixepoch()),
			last_used_at INTEGER DEFAULT (unixepoch()),
			updated_at INTEGER
		);
	)");

  if (!ok) { qDebug() << "Failed to init favicon database:" << query.lastError(); }
}
