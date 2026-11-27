#include "favicon/favicon-service.hpp"
#include "favicon/dummy-favicon-request.hpp"
#include "favicon/google-favicon-request.hpp"
#include "favicon/twenty-favicon-request.hpp"
#include <QPromise>
#include <QtConcurrent/QtConcurrent>
#include <memory>
#include <qlogging.h>

static const std::vector<FaviconService::FaviconServiceData> faviconProviders = {
    {.id = "google",
     .name = "Google",
     .icon = ImageURL::builtin(BuiltinIcon::Google),
     .type = FaviconService::Google},
    {.id = "twenty",
     .name = "Twenty",
     .icon = ImageURL::builtin(BuiltinIcon::Twenty),
     .type = FaviconService::Twenty},
    {.id = "none",
     .name = "None",
     .icon = ImageURL::builtin(BuiltinIcon::Image),
     .type = FaviconService::None}};

std::vector<FaviconService::FaviconServiceData> FaviconService::providers() { return faviconProviders; }

FaviconService *FaviconService::instance() {
  assert(_instance && "FaviconService::instance() called before FaviconService::initialize()");
  return _instance;
}

QString FaviconService::serviceIdFor(RequesterType type) {
  if (auto it = std::ranges::find_if(faviconProviders, [&](const auto &item) { return item.type == type; });
      it != faviconProviders.end()) {
    return it->id;
  }
  return QStringLiteral("none");
}

QString FaviconService::cacheKey(const QString &providerId, const QString &domain) {
  return providerId + QLatin1Char('/') + domain;
}

QString FaviconService::serviceId() const { return serviceIdFor(_requesterType); }

QString FaviconService::currentServiceId() { return _instance ? _instance->serviceId() : serviceIdFor(None); }

void FaviconService::setService(const QString &id) {
  if (auto it = std::ranges::find_if(faviconProviders, [&](auto &&item) { return item.id == id; });
      it != faviconProviders.end()) {
    setService(it->type);
    return;
  }

  qCritical() << "no favicon provider for id" << id;
}

void FaviconService::setService(RequesterType type) {
  if (_requesterType == type) return;
  _requesterType = type;
  emit serviceChanged();
}

QFuture<FaviconService::FaviconResponse> FaviconService::makeRequest(const QString &domain, QObject *parent) {
  if (_requesterType == None)
    return QtFuture::makeReadyValueFuture(FaviconResponse(std::unexpected("Favicon fetching is disabled")));

  if (domain.isEmpty())
    return QtFuture::makeReadyValueFuture(FaviconResponse(std::unexpected("Empty domain")));

  const RequesterType type = _requesterType;
  const QString key = cacheKey(serviceIdFor(type), domain);

  if (const auto *image = _cache.object(key)) return QtFuture::makeReadyValueFuture(FaviconResponse(*image));

  // a QFuture supports a single continuation, so concurrent callers each get their own promise
  auto promise = std::make_shared<QPromise<FaviconResponse>>();
  auto future = promise->future();

  promise->start();

  auto [it, inserted] = _pending.try_emplace(key);
  it->second.emplace_back(std::move(promise));

  if (!inserted) return future;

  QtConcurrent::run([path = _dataDir.filePath(key)]() {
    return QImage(path);
  }).then(this, [this, type, domain, key, parent](QImage image) {
    if (image.isNull()) {
      fetch(type, key, domain, parent);
      return;
    }
    touch(key);
    resolve(key, std::move(image));
  });

  return future;
}

void FaviconService::resolve(const QString &key, const FaviconResponse &response) {
  if (response) _cache.insert(key, new QImage(*response));

  auto node = _pending.extract(key);

  if (node.empty()) return;

  for (const auto &promise : node.mapped()) {
    promise->addResult(response);
    promise->finish();
  }
}

void FaviconService::fetch(RequesterType type, const QString &key, const QString &domain, QObject *parent) {
  AbstractFaviconRequest *requester = nullptr;

  switch (type) {
  case Google:
    requester = new GoogleFaviconRequester(domain, parent);
    break;
  case Twenty:
    requester = new TwentyFaviconRequester(domain, parent);
    break;
  case None:
    resolve(key, std::unexpected("Favicon fetching is disabled"));
    return;
  default:
    requester = new DummyFaviconRequest(domain, parent);
  }

  connect(requester, &AbstractFaviconRequest::finished, this, [this, requester, key](const QByteArray &data) {
    requester->deleteLater();
    QtConcurrent::run([data]() { return QImage::fromData(data); }).then(this, [this, key](QImage image) {
      if (image.isNull()) {
        resolve(key, std::unexpected("Invalid favicon data"));
        return;
      }
      store(key, image);
      resolve(key, std::move(image));
    });
  });

  connect(requester, &AbstractFaviconRequest::failed, this, [this, requester, key]() {
    requester->deleteLater();
    resolve(key, std::unexpected("Failed to fetch favicon"));
  });

  requester->start();
}

void FaviconService::store(const QString &key, const QImage &favicon) {
  _ioPool.start([this, key, favicon, path = _dataDir.filePath(key)]() {
    QFileInfo(path).dir().mkpath(QStringLiteral("."));

    if (!favicon.save(path, "PNG")) {
      qWarning() << "Failed to save favicon on disk";
      return;
    }

    auto stmt = _db.prepare(R"(
      INSERT OR REPLACE INTO favicon (id, created_at, size)
      VALUES (:id, :epoch, :size)
    )");
    stmt.bind(":id", key);
    stmt.bind(":epoch", static_cast<int64_t>(QDateTime::currentSecsSinceEpoch()));
    stmt.bind(":size", favicon.width());

    if (!stmt.exec()) { qWarning() << "Favicon DB: failed to insert favicon:" << stmt.lastError().c_str(); }
  });
}

void FaviconService::touch(const QString &key) {
  _ioPool.start([this, key]() {
    auto stmt = _db.prepare("UPDATE favicon SET last_used_at = :epoch WHERE id = :domain;");
    stmt.bind(":domain", key);
    stmt.bind(":epoch", static_cast<int64_t>(QDateTime::currentSecsSinceEpoch()));

    if (!stmt.exec()) {
      qWarning() << "Favicon DB: failed to update last_used_at" << stmt.lastError().c_str();
    }
  });
}

FaviconService::FaviconService(const std::filesystem::path &path, QObject *parent)
    : QObject(parent), _requesterType(RequesterType::Google) {
  _ioPool.setMaxThreadCount(1);
  _dataDir = QFileInfo(path).dir().filePath("favicon-data");
  _dataDir.mkpath(_dataDir.path());

  auto result = db::Database::open(path);

  if (!result) {
    qDebug() << "Failed to open favicon DB" << result.error().c_str();
    return;
  }

  _db = std::move(*result);

  if (!_db.exec(R"(
    CREATE TABLE IF NOT EXISTS favicon (
      id TEXT PRIMARY KEY,
      size INTEGER,
      created_at INTEGER NOT NULL,
      last_used_at INTEGER,
      updated_at INTEGER
    );
  )")) {
    qDebug() << "Failed to init favicon database:" << _db.lastError().c_str();
  }
}
