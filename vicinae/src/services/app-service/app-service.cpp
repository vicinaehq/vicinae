#include "app-service.hpp"
#include "services/app-service/xdg/xdg-app-database.hpp"
#include "omni-database.hpp"
#include <filesystem>
#include <qfilesystemwatcher.h>

namespace fs = std::filesystem;

std::vector<std::filesystem::path> AppService::mergedPaths() const {
  std::vector<fs::path> paths;
  auto defaultPaths = defaultSearchPaths();

  paths.reserve(defaultPaths.size() + m_additionalSearchPaths.size());
  // Manually added paths have highest priority, so they come first
  paths.insert(paths.end(), m_additionalSearchPaths.begin(), m_additionalSearchPaths.end());
  // Then add default system paths (XDG_DATA_HOME, XDG_DATA_DIRS)
  paths.insert(paths.end(), defaultPaths.begin(), defaultPaths.end());

  return paths;
}

AbstractAppDatabase *AppService::provider() const { return m_provider.get(); }

bool AppService::launch(const Application &app, const std::vector<QString> &args) const {
  return m_provider->launch(app, args);
}

std::unique_ptr<AbstractAppDatabase> AppService::createLocalProvider() {
#ifdef Q_OS_DARWIN
  return nullptr;
#endif

#if defined(Q_OS_UNIX) && not defined(Q_OS_DARWIN)
  return std::make_unique<XdgAppDatabase>();
#endif
}

std::shared_ptr<Application> AppService::terminalEmulator() const {
  return m_provider->findBestOpenerForMime("x-scheme-handler/terminal");
}

std::shared_ptr<Application> AppService::findById(const QString &id) const {
  return m_provider->findById(id);
}

std::shared_ptr<Application> AppService::findByClass(const QString &wmClass) const {
  return m_provider->findByClass(wmClass);
}

bool AppService::openTarget(const QString &target) const {
  if (auto app = findBestOpener(target)) { return launch(*app, {target}); }
  return false;
}

std::vector<fs::path> AppService::defaultSearchPaths() const { return m_provider->defaultSearchPaths(); }

std::shared_ptr<Application> AppService::textEditor() const { return m_provider->textEditor(); }

std::shared_ptr<Application> AppService::webBrowser() const { return m_provider->webBrowser(); }
std::shared_ptr<Application> AppService::fileBrowser() const { return m_provider->fileBrowser(); }

std::vector<std::shared_ptr<Application>> AppService::list() const { return m_provider->list(); }

std::shared_ptr<Application> AppService::findBestOpener(const QString &target) const {
  return m_provider->findBestOpener(target);
}

std::shared_ptr<Application> AppService::find(const QString &target) const {
  if (auto app = m_provider->findById(target)) { return app; }
  if (auto app = m_provider->findByClass(target)) { return app; }

  return nullptr;
}

void AppService::handleDirectoryChanged(const QString &path) {
  // This event can fire multiple times for a single change.
  // Since scanning apps is fast we don't really need to batch events, at least for now.

  qInfo() << "app directory" << path << "changed, launching a new scan";
  scanSync();
}

void AppService::setAdditionalSearchPaths(const std::vector<std::filesystem::path> &paths) {
  m_additionalSearchPaths = paths;
  reinstallWatches(mergedPaths());
}

std::vector<std::shared_ptr<Application>> AppService::findOpeners(const QString &target) const {
  return m_provider->findOpeners(target);
}

bool AppService::reinstallWatches(const std::vector<fs::path> &paths) {
  for (const auto &path : m_watcher->directories()) {
    m_watcher->removePath(path);
  }

  auto isDir = [](auto &&path) { return fs::is_directory(path); };

  for (const auto &path : paths | std::views::filter(isDir)) {
    m_watcher->addPath(path.c_str());
  }

  return true;
}

bool AppService::scanSync() {
  bool result = m_provider->scan(mergedPaths());

  emit appsChanged();

  return result;
}

AppService::AppService(OmniDatabase &db) : m_db(db), m_provider(createLocalProvider()) {
  reinstallWatches(mergedPaths());
  connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, &AppService::handleDirectoryChanged);
}
