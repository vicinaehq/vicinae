#include "favicon/favicon-service.hpp"
#include "favicon/cached-favicon-request.hpp"
#include "favicon/dummy-favicon-request.hpp"
#include "favicon/google-favicon-request.hpp"
#include "favicon/twenty-favicon-request.hpp"
#include <qlogging.h>

static const std::vector<FaviconService::FaviconServiceData> faviconProviders = {
    {.id = "google", .name = "Google", .icon = ImageURL::builtin("google"), .type = FaviconService::Google},
    {.id = "twenty", .name = "Twenty", .icon = ImageURL::builtin("twenty"), .type = FaviconService::Twenty},
    {.id = "none", .name = "None", .icon = ImageURL::builtin("image"), .type = FaviconService::None}};

std::vector<FaviconService::FaviconServiceData> FaviconService::providers() { return faviconProviders; }

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
		INSERT INTO favicon (id, created_at, size)
		VALUES (:id, :epoch, :size)
	)");
  query.bindValue(":id", domain);
  query.bindValue(":epoch", QDateTime::currentSecsSinceEpoch());
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

  query.prepare("UPDATE favicon SET last_used_at = :epoch WHERE id = :domain;");
  query.bindValue(":domain", domain);
  query.bindValue(":epoch", QDateTime::currentSecsSinceEpoch());

  if (!query.exec()) { qDebug() << "Favicon DB: failed to update last_used_at" << query.lastError(); }

  QFile favicon(_dataDir.filePath(domain));

  if (favicon.open(QIODevice::ReadOnly)) {
    pm.loadFromData(favicon.readAll());
    insertCache(domain, pm);
  }

  return pm;
}

void FaviconService::setService(const QString &id) {
  if (auto it = std::ranges::find_if(faviconProviders, [&](auto &&item) { return item.id == id; });
      it != faviconProviders.end()) {
    setService(it->type);
    return;
  }

  qCritical() << "no favicon provider for id" << id;
}

void FaviconService::setService(RequesterType type) { _requesterType = type; }

QFuture<FaviconService::FaviconResponse> FaviconService::makeRequest(const QString &domain, QObject *parent) {
  QPromise<FaviconResponse> promise;
  auto future = promise.future();

  if (auto pix = retrieveFromCache(domain); !pix.isNull()) {
    promise.addResult(pix);
    promise.finish();

    return future;
  }

  AbstractFaviconRequest *requester = nullptr;

  switch (_requesterType) {
  case Google:
    requester = new GoogleFaviconRequester(domain, parent);
    break;
  case Twenty:
    requester = new TwentyFaviconRequester(domain, parent);
    break;
  case None:
    promise.addResult(std::unexpected("Favicon fetching is disabled"));
    promise.finish();
    return future;
  default:
    requester = new DummyFaviconRequest(domain, parent);
  }

  qDebug() << "request start !";

  connect(requester, &AbstractFaviconRequest::finished, this,
          [this, requester, domain, promise = std::move(promise)](const QPixmap &favicon) mutable {
            handleFetchedFavicon(domain, favicon);
            promise.addResult(favicon);
            promise.finish();
            requester->deleteLater();
          });

  requester->start();

  return future;
}

FaviconService::FaviconService(const std::filesystem::path &path, QObject *parent)
    : QObject(parent), _db(QSqlDatabase::addDatabase("QSQLITE", "favicon")),
      _requesterType(RequesterType::Google) {
  _dataDir = QFileInfo(path).dir().filePath("favicon-data");
  _dataDir.mkpath(_dataDir.path());
  _db.setDatabaseName(path.c_str());

  if (!_db.open()) {
    qDebug() << "Failed to open favicon DB" << _db.lastError();
    return;
  }

  QSqlQuery query(_db);

  bool ok = query.exec(R"(
		CREATE TABLE IF NOT EXISTS favicon (
			id TEXT PRIMARY KEY,
			size INTEGER,
			created_at INTEGER NOT NULL,
			last_used_at INTEGER,
			updated_at INTEGER
		);
	)");

  if (!ok) { qDebug() << "Failed to init favicon database:" << query.lastError(); }
}
