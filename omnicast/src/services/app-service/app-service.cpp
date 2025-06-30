#include "app-service.hpp"
#include "app/xdg-app-database.hpp"
#include "omni-database.hpp"

namespace fs = std::filesystem;

std::vector<std::filesystem::path> AppService::mergedPaths() const {
  std::vector<fs::path> paths;
  auto defaultPaths = defaultSearchPaths();

  paths.reserve(defaultPaths.size() + m_additionalSearchPaths.size());
  paths.insert(paths.end(), defaultPaths.begin(), defaultPaths.end());
  paths.insert(paths.end(), m_additionalSearchPaths.begin(), m_additionalSearchPaths.end());

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

void AppService::setAdditionalSearchPaths(const std::vector<std::filesystem::path> &paths) {
  m_additionalSearchPaths = paths;
}

std::vector<std::shared_ptr<Application>> AppService::findOpeners(const QString &target) const {
  return m_provider->findOpeners(target);
}

bool AppService::scanSync() {
  bool result = m_provider->scan(mergedPaths());

  emit appsChanged();

  return result;
}

AppService::AppService(OmniDatabase &db) : m_db(db), m_provider(createLocalProvider()) {}
