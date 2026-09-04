#include "app-service.hpp"
#include "timer.hpp"
#include <chrono>
#include <qlogging.h>
#include <qnumeric.h>
#ifdef Q_OS_MACOS
#include "services/app-service/macos/mac-app-database.hpp"
#elif defined(Q_OS_WIN)
#include "services/app-service/windows/win-app-database.hpp"
#else
#include "services/app-service/xdg/xdg-app-database.hpp"
#endif
#include "omni-database.hpp"
#include <QProcess>
#include <filesystem>
#include <qcontainerfwd.h>
#include <qfilesystemwatcher.h>
#include <ranges>

namespace fs = std::filesystem;

AbstractAppDatabase *AppService::provider() const { return m_provider.get(); }

bool AppService::launch(const AbstractApplication &app, const std::vector<QString> &args) const {
  return m_provider->launch(app, args);
}

bool AppService::launchTerminalCommand(const std::vector<QString> &cmdLine,
                                       const LaunchTerminalCommandOptions &opts) {
  return m_provider->launchTerminalCommand(cmdLine, opts);
}

std::unique_ptr<AbstractAppDatabase> AppService::createLocalProvider() {
#ifdef Q_OS_MACOS
  return std::make_unique<MacAppDatabase>();
#elif defined(Q_OS_WIN)
  return std::make_unique<WindowsAppDatabase>();
#else
  return std::make_unique<XdgAppDatabase>();
#endif
}

std::unique_ptr<QProcess> AppService::shellProcess(const QString &code) const {
  return m_provider->shellProcess(code);
}

std::shared_ptr<AbstractApplication> AppService::terminalEmulator() const {
  return m_provider->terminalEmulator();
}

bool AppService::launchRaw(const std::vector<QString> &args) {
  if (args.empty()) { return false; }

  QProcess process;
  const QString &prog = args.at(0);

  process.setProgram(prog);
  process.setArguments(args | std::views::drop(1) | std::ranges::to<QStringList>());
  process.setStandardOutputFile(QProcess::nullDevice());
  process.setStandardErrorFile(QProcess::nullDevice());

  if (!process.startDetached()) {
    qWarning() << "Failed to start app" << prog << args << process.errorString();
    return false;
  }

  return true;
}

std::shared_ptr<AbstractApplication> AppService::findById(const QString &id) const {
  return m_provider->findById(id);
}

std::shared_ptr<AbstractApplication> AppService::findByClass(const QString &wmClass) const {
  return m_provider->findByClass(wmClass);
}

bool AppService::openTarget(const QString &target) const {
  if (auto app = findDefaultOpener(target)) { return launch(*app, {target}); }
  return false;
}

bool AppService::openTarget(const QUrl &target) const { return openTarget(target.toString()); }

std::shared_ptr<AbstractApplication> AppService::textEditor() const {
  return m_provider->genericTextEditor();
}

std::shared_ptr<AbstractApplication> AppService::webBrowser() const { return m_provider->webBrowser(); }
std::shared_ptr<AbstractApplication> AppService::fileBrowser() const { return m_provider->fileBrowser(); }

std::vector<std::shared_ptr<AbstractApplication>> AppService::list(const AppListOptions &opts) const {
  auto apps = m_provider->list();
  if (opts.sortAlphabetically) {
    std::ranges::sort(apps, [](const auto &a, const auto &b) {
      return a->displayName().compare(b->displayName(), Qt::CaseInsensitive) < 0;
    });
  }
  return apps;
}

std::shared_ptr<AbstractApplication> AppService::findDefaultOpener(const QString &target) const {
  return m_provider->findDefaultOpener(target);
}

std::shared_ptr<AbstractApplication> AppService::find(const QString &target) const {
  if (auto app = m_provider->findById(target)) { return app; }
  if (auto app = m_provider->findByClass(target)) { return app; }

  return nullptr;
}

bool AppService::showInFileBrowser(const std::filesystem::path &path, bool select) const {
  return m_provider->showInFileBrowser(path, select);
}

bool AppService::openLocation(const AbstractApplication &app) const { return m_provider->openLocation(app); }

void AppService::handleDirectoryChanged(const QString &path) {
  (void)path;
  m_rescanDebounce->start();
}

std::vector<std::shared_ptr<AbstractApplication>> AppService::findOpeners(const QString &target) const {
  return m_provider->findOpeners(target);
}

std::vector<std::shared_ptr<AbstractApplication>>
AppService::findCuratedOpeners(const QString &target) const {
  std::set<QString> seenNames;
  std::vector<std::shared_ptr<AbstractApplication>> results;
  auto openers = findOpeners(target);

  results.reserve(openers.size());

  for (const auto &opener : openers) {
    if (seenNames.contains(opener->displayName())) continue;
    seenNames.insert(opener->displayName());
    results.emplace_back(opener);
  }

  return results;
}

bool AppService::reinstallWatches(const std::vector<fs::path> &paths) {
  QStringList current = m_watcher->directories();
  QStringList desired;
  std::error_code ec{};

  for (const auto &path : paths) {
    if (fs::is_directory(path, ec)) { desired.append(QString::fromStdString(path.string())); }
  }

  desired.sort();
  desired.removeDuplicates();
  current.sort();

  // It's important that we don't reinstall the watches if the directories haven't changed.
  // On some systems it's a no-op, on others like macOS it can trigger a new flood
  // of events, which can create some performance problems.
  if (current == desired) return true;
  if (!current.isEmpty()) m_watcher->removePaths(current);
  if (!desired.isEmpty()) m_watcher->addPaths(desired);

  return true;
}

bool AppService::scanSync() {
  reinstallWatches(m_provider->searchPaths());
  bool const result = m_provider->scan();
  emit appsChanged();
  return result;
}

AppService::AppService(OmniDatabase &db) : m_db(db), m_provider(createLocalProvider()) {
  m_rescanDebounce->setSingleShot(true);
  m_rescanDebounce->setInterval(500);
  connect(m_rescanDebounce, &QTimer::timeout, this, [this] {
    qInfo() << "Scanning apps again, following a directory change...";
    auto elapsed = timer::time([&]() { scanSync(); });
    qInfo() << "Done scanning apps, took" << elapsed.count() / 1e6 << "ms";
  });

  reinstallWatches(m_provider->searchPaths());
  connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, &AppService::handleDirectoryChanged);
  connect(m_provider.get(), &AbstractAppDatabase::changed, this, [this] { m_rescanDebounce->start(); });
}
