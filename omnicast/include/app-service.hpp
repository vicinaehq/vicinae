#pragma once
#include "app/app-database.hpp"
#include "app/xdg-app-database.hpp"
#include "common.hpp"
#include "libtrie/trie.hpp"
#include "omni-database.hpp"
#include "ranking-service.hpp"
#include <memory>
#include <qlogging.h>
#include <qobject.h>
#include <qsqlquery.h>

class AppService : public QObject, public NonAssignable {
public:
  struct AppEntry {
    std::shared_ptr<Application> app;
    QList<QString> searchTokens;
    int openCount;
    bool disabled;
    bool hidden;
    double frecency;
    std::optional<std::chrono::time_point<std::chrono::high_resolution_clock>> lastOpenedAt;

    bool operator==(const AppEntry &rhs) const { return app->id() == rhs.app->id(); }
  };

  struct AppEntryHash {
    size_t operator()(const std::shared_ptr<AppEntry> &app) const { return qHash(app->app->id()); }
  };

  Trie<std::shared_ptr<AppEntry>, AppEntryHash> m_trie;

private:
  OmniDatabase &m_db;
  RankingService &m_ranking;
  QSqlQuery m_syncAppQuery;
  std::unique_ptr<AbstractAppDatabase> m_provider;
  std::vector<std::shared_ptr<AppEntry>> m_entries;

  static std::unique_ptr<AbstractAppDatabase> createLocalProvider() {
#ifdef Q_OS_DARWIN
    return nullptr;
#endif

#if defined(Q_OS_UNIX) && not defined(Q_OS_DARWIN)
    return std::make_unique<XdgAppDatabase>();
#endif
  }

  void createTables() {
    auto query = m_db.createQuery();

    if (!query.exec(R"(
	  	CREATE TABLE IF NOT EXISTS app_metadata (
			id TEXT PRIMARY KEY,
			disabled INT DEFAULT 0
		);
	  )")) {
      qCritical() << "Failed to write table";
    }
  }

  std::optional<std::shared_ptr<AppEntry>> syncApp(std::shared_ptr<Application> app) {
    m_syncAppQuery.bindValue(":id", app->id());

    if (!m_syncAppQuery.exec() || !m_syncAppQuery.next()) {
      qCritical() << "Failed to sync app" << app->id() << m_syncAppQuery.lastError();
      return {};
    }

    auto &record = m_ranking.findRecord(QString("application:%1").arg(app->id()));
    AppEntry entry;

    entry.openCount = record.visitedCount;
    entry.lastOpenedAt = record.lastVisitedAt;
    entry.frecency = m_ranking.computeFrecency(record);
    entry.disabled = m_syncAppQuery.value(0).toBool();
    entry.searchTokens = app->name().split(' ');
    entry.app = app;

    auto ptr = std::make_shared<AppEntry>(entry);

    m_trie.indexLatinText(entry.app->name().toStdString(), ptr);

    for (const auto &keyword : app->keywords()) {
      m_trie.index(keyword.toStdString(), ptr);
    }

    return ptr;
  }

  void syncApps() {
    auto apps = m_provider->list();

    m_entries.reserve(apps.size());
    m_entries.clear();
    m_db.db().transaction();

    for (const auto &app : apps) {
      if (auto entry = syncApp(app)) { m_entries.emplace_back(*entry); }
    }

    qDebug() << "Transformed" << m_entries.size() << "apps";

    m_syncAppQuery.finish();

    if (!m_db.db().commit()) {
      qCritical() << "Failed to commit to DB" << m_db.db().lastError();
      m_db.db().rollback();
    }
  }

public:
  AbstractAppDatabase *appProvider() const { return m_provider.get(); }

  const std::vector<std::shared_ptr<AppEntry>> &listEntries() const { return m_entries; }
  std::vector<std::shared_ptr<Application>> list() const { return m_provider->list(); }

  bool launch(const Application &app, const std::vector<QString> &args = {}) const {
    return m_provider->launch(app, args);
  }

  void registerVisit(const std::shared_ptr<Application> &app) { registerVisit(app->id()); }

  void registerVisit(const QString &appId) {
    auto record = m_ranking.registerVisit("application", appId);
    auto it =
        std::ranges::find_if(m_entries, [&appId](const auto &entry) { return entry->app->id() == appId; });

    if (it != m_entries.end()) {
      (*it)->lastOpenedAt = record.lastVisitedAt;
      (*it)->openCount = record.visitedCount;
    }
  }

  std::shared_ptr<Application> webBrowser() const { return m_provider->webBrowser(); }
  std::shared_ptr<Application> fileBrowser() const { return m_provider->fileBrowser(); }
  std::shared_ptr<Application> textEditor() const { return m_provider->textEditor(); }

  std::shared_ptr<Application> findById(const QString &id) const { return m_provider->findById(id); }

  std::shared_ptr<Application> findByClass(const QString &wmClass) const {
    return m_provider->findByClass(wmClass);
  }

  /**
   * Attempts to find an application from the provided string
   * The following fields are searched, in order:
   * - application id (for non-apple UNIXes, with or without the .desktop extension)
   * - window manager class, if applicable
   */
  std::shared_ptr<Application> find(const QString &target) const {
    if (auto app = m_provider->findById(target)) { return app; }
    if (auto app = m_provider->findByClass(target)) { return app; }

    return nullptr;
  }

  std::shared_ptr<Application> findBestOpener(const QString &target) const {
    return m_provider->findBestOpener(target);
  }

  AppService(OmniDatabase &db, RankingService &ranking) : m_db(db), m_ranking(ranking) {
    createTables();
    m_ranking.loadRecords("application");
    m_syncAppQuery = db.createQuery();
    m_syncAppQuery.prepare(R"(
	  	INSERT INTO app_metadata (id) 
		VALUES (:id)
		ON CONFLICT (id)
		DO UPDATE SET disabled = disabled 
		RETURNING disabled
	)");
    m_provider = createLocalProvider();
    syncApps();
  }
};
