#pragma once
#include "app/app-database.hpp"
#include "app/xdg-app-database.hpp"
#include "omni-database.hpp"
#include <iterator>
#include <qlogging.h>
#include <qobject.h>
#include <qsqlquery.h>
#include <ranges>

class AppService : public QObject {
public:
  struct AppEntry {
    std::shared_ptr<Application> app;
    int openCount;
    bool disabled;
    std::optional<std::chrono::time_point<std::chrono::high_resolution_clock>> lastOpenedAt;
  };

private:
  OmniDatabase &m_db;
  std::unique_ptr<AbstractAppDatabase> m_provider;
  std::vector<AppEntry> m_entries;

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
			open_count INT DEFAULT 0,
			disabled INT DEFAULT 0,
			last_opened_at INT
		);
	  )")) {
      qCritical() << "Failed to write table";
    }
  }

  std::optional<AppEntry> syncApp(std::shared_ptr<Application> app) {
    auto query = m_db.createQuery();

    query.prepare(R"(
	  	INSERT INTO app_metadata (id) 
		VALUES (:id)
		ON CONFLICT (id)
		DO UPDATE SET open_count = open_count, disabled = disabled, last_opened_at = last_opened_at
		RETURNING open_count, disabled, last_opened_at
	  )");
    query.bindValue(":id", app->id());

    if (!query.exec() || !query.next()) {
      qCritical() << "Failed to sync app" << app->id() << query.lastError();
      return {};
    }

    AppEntry entry;

    entry.openCount = query.value(0).toInt();
    entry.disabled = query.value(1).toBool();
    entry.app = app;

    QVariant lastOpenedAtField = query.value(2).toInt();

    if (!lastOpenedAtField.isNull()) {
      std::time_t epoch = lastOpenedAtField.toULongLong();
      entry.lastOpenedAt = std::chrono::system_clock::from_time_t(epoch);
    }

    qDebug() << "registered" << entry.app->id();

    return entry;
  }

  void syncApps() {
    auto apps = m_provider->list();

    m_entries.reserve(apps.size());
    m_entries.clear();
    m_db.db().transaction();
    auto entries = apps | std::views::transform([this](auto app) { return syncApp(app); }) |
                   std::views::filter([](const auto &a) { return a.has_value(); }) |
                   std::views::transform([](const auto &a) { return a.value(); });
    std::ranges::copy(entries, std::back_inserter(m_entries));
    qDebug() << "Transformed" << m_entries.size() << "apps";
    m_db.db().commit();
  }

public:
  AbstractAppDatabase *appProvider() const { return m_provider.get(); }

  const std::vector<AppEntry> &listEntries() const { return m_entries; }
  std::vector<std::shared_ptr<Application>> list() const { return m_provider->list(); }

  bool launch(const Application &app, const std::vector<QString> &args = {}) const {
    return m_provider->launch(app, args);
  }

  std::shared_ptr<Application> webBrowser() const { return m_provider->webBrowser(); }
  std::shared_ptr<Application> fileBrowser() const { return m_provider->fileBrowser(); }

  std::shared_ptr<Application> findById(const QString &id) const { return m_provider->findById(id); }

  std::shared_ptr<Application> findByClass(const QString &wmClass) const {
    return m_provider->findByClass(wmClass);
  }

  std::shared_ptr<Application> findBestOpener(const QString &target) const {
    return m_provider->findBestOpener(target);
  }

  AppService(OmniDatabase &db) : m_db(db) {
    createTables();
    m_provider = createLocalProvider();
    syncApps();
  }
};
