#pragma once
#include "app/app-database.hpp"
#include "app/xdg-app-database.hpp"
#include "common.hpp"
#include "omni-database.hpp"
#include "ranking-service.hpp"
#include <memory>
#include <qlogging.h>
#include <qobject.h>
#include <qobjectdefs.h>
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

  std::vector<std::filesystem::path> m_additionalSearchPaths;

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

public:
  AbstractAppDatabase *appProvider() const { return m_provider.get(); }

  const std::vector<std::shared_ptr<AppEntry>> &listEntries() const { return m_entries; }
  std::vector<std::shared_ptr<Application>> list() const { return m_provider->list(); }

  bool launch(const Application &app, const std::vector<QString> &args = {}) const {
    return m_provider->launch(app, args);
  }

  auto defaultSearchPaths() const { return m_provider->defaultSearchPaths(); }

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
  std::shared_ptr<Application> terminalEmulator() const {
    return m_provider->findBestOpenerForMime("x-scheme-handler/terminal");
  }
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

  void setAdditionalSearchPaths(const std::vector<std::filesystem::path> &paths) {
    m_additionalSearchPaths = paths;
  }

  /**
   * Returns all the apps that are marked as being terminal emulators.
   */
  std::vector<std::shared_ptr<Application>> terminalEmulators() const {
    return m_provider->list() | std::views::filter([&](auto &&app) { return app->isTerminalEmulator(); }) |
           std::ranges::to<std::vector>();
  }

  /**
   * Scan application directories synchronously.
   * This is usually very fast.
   */
  bool scanSync() {
    std::vector<std::filesystem::path> paths = m_provider->defaultSearchPaths();

    paths.insert(paths.end(), m_additionalSearchPaths.begin(), m_additionalSearchPaths.end());

    return m_provider->scan(paths);
  }

  std::vector<std::shared_ptr<Application>> findOpeners(const QString &target) const {
    return m_provider->findOpeners(target);
  }

  AppService(OmniDatabase &db, RankingService &ranking) : m_db(db), m_ranking(ranking) {
    m_provider = createLocalProvider();
  }
};
