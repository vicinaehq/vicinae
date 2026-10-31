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
  if (const auto *image = _cache.object(domain))
    return QtFuture::makeReadyValueFuture(FaviconResponse(*image));

  // a QFuture supports a single continuation, so concurrent callers each get their own promise
  auto promise = std::make_shared<QPromise<FaviconResponse>>();
  auto future = promise->future();

  promise->start();

  auto [it, inserted] = _pending.try_emplace(domain);
  it->second.emplace_back(std::move(promise));

  if (!inserted) return future;

  QtConcurrent::run([path = _dataDir.filePath(domain)]() {
    return QImage(path);
  }).then(this, [this, domain, parent](QImage image) {
    if (image.isNull()) {
      fetch(domain, parent);
      return;
    }
    touch(domain);
    resolve(domain, std::move(image));
  });

  return future;
}

void FaviconService::resolve(const QString &domain, const FaviconResponse &response) {
  if (response) _cache.insert(domain, new QImage(*response));

  auto node = _pending.extract(domain);

  if (node.empty()) return;

  for (const auto &promise : node.mapped()) {
    promise->addResult(response);
    promise->finish();
  }
}

void FaviconService::fetch(const QString &domain, QObject *parent) {
  AbstractFaviconRequest *requester = nullptr;

  switch (_requesterType) {
  case Google:
    requester = new GoogleFaviconRequester(domain, parent);
    break;
  case Twenty:
    requester = new TwentyFaviconRequester(domain, parent);
    break;
  case None:
    resolve(domain, std::unexpected("Favicon fetching is disabled"));
    return;
  default:
    requester = new DummyFaviconRequest(domain, parent);
  }

  connect(requester, &AbstractFaviconRequest::finished, this,
          [this, requester, domain](const QByteArray &data) {
            requester->deleteLater();
            QtConcurrent::run([data]() {
              return QImage::fromData(data);
            }).then(this, [this, domain](QImage image) {
              if (image.isNull()) {
                resolve(domain, std::unexpected("Invalid favicon data"));
                return;
              }
              store(domain, image);
              resolve(domain, std::move(image));
            });
          });

  connect(requester, &AbstractFaviconRequest::failed, this, [this, requester, domain]() {
    requester->deleteLater();
    resolve(domain, std::unexpected("Failed to fetch favicon"));
  });

  requester->start();
}

void FaviconService::store(const QString &domain, const QImage &favicon) {
  _ioPool.start([this, domain, favicon, path = _dataDir.filePath(domain)]() {
    if (!favicon.save(path, "PNG")) {
      qWarning() << "Failed to save favicon on disk";
      return;
    }

    auto stmt = _db.prepare(R"(
      INSERT INTO favicon (id, created_at, size)
      VALUES (:id, :epoch, :size)
    )");
    stmt.bind(":id", domain);
    stmt.bind(":epoch", static_cast<int64_t>(QDateTime::currentSecsSinceEpoch()));
    stmt.bind(":size", favicon.width());

    if (!stmt.exec()) { qWarning() << "Favicon DB: failed to insert favicon:" << stmt.lastError().c_str(); }
  });
}

void FaviconService::touch(const QString &domain) {
  _ioPool.start([this, domain]() {
    auto stmt = _db.prepare("UPDATE favicon SET last_used_at = :epoch WHERE id = :domain;");
    stmt.bind(":domain", domain);
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
