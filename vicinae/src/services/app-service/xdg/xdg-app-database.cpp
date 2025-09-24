#include "xdg-app-database.hpp"
#include "timer.hpp"
#include "xdgpp/desktop-entry/file.hpp"
#include "xdgpp/mime/iterator.hpp"
#include <filesystem>
#include <qlogging.h>
#include <qsettings.h>
#include <queue>
#include <ranges>
#include <set>
#include <QDir>
#include <stack>
#include <xdgpp/desktop-entry/iterator.hpp>
#include <xdgpp/env/env.hpp>
#include <xdgpp/mime/mime-apps-list.hpp>

namespace fs = std::filesystem;

using AppPtr = XdgAppDatabase::AppPtr;

std::shared_ptr<AbstractApplication> XdgAppDatabase::defaultForMime(const QString &mime) const {

  for (const auto &list : m_mimeAppsLists) {
    for (const auto &appId : list.defaultAssociations(mime.toStdString())) {
      if (auto appIt = appMap.find(appId.c_str()); appIt != appMap.end()) { return appIt->second; }
    }
  }

  auto openers = findOpeners(mime);

  return openers.empty() ? nullptr : openers.front();
}

AppPtr XdgAppDatabase::findDefaultOpener(const QString &target) const {
  return defaultForMime(mimeNameForTarget(target));
}

AppPtr XdgAppDatabase::findBestTerminalEmulator() const {
  if (auto emulator = defaultForMime("x-scheme-handler/terminal")) { return emulator; }

  qWarning()
      << "Vicinae was unable to find a default terminal emulator by looking for a x-scheme-handler/terminal "
         "mime association. We may fallback on whatever emulator is available, but you should set a default "
         "terminal to use if you want predictable behavior. On Linux, you should be able to use the "
         "following: 'xdg-mime default <desktop_file_name>.desktop x-scheme-handler/terminal'. Note that "
         "this warning can also be shown if you added an invalid association, typically with an invalid "
         "desktop file.";

  auto isTerminal = [](auto &&app) { return app->isTerminalEmulator(); };
  auto result = list() | std::views::filter(isTerminal) | std::views::take(1);

  if (result.empty()) return nullptr;

  return *result.begin();
}

bool XdgAppDatabase::scan(const std::vector<std::filesystem::path> &paths) {
  appMap.clear();
  m_apps.clear();
  m_mimeAppsLists.clear();
  m_dataDirToApps.clear();
  m_mimeAppsLists = xdgpp::getAllMimeAppsLists();

  std::set<std::string> seen;

  for (const auto &dir : xdgpp::appDirs()) {
    std::error_code ec;

    for (const auto &entry : fs::recursive_directory_iterator(dir, ec)) {
      if (!entry.is_regular_file(ec)) continue;
      if (!entry.path().filename().string().ends_with(".desktop")) continue;

      std::string id = xdgpp::DesktopFile::relativeId(entry.path(), dir);

      if (seen.contains(id)) continue;
      seen.insert(id);

      auto file = xdgpp::DesktopFile::fromFile(entry.path(), dir);

      if (file.hidden()) continue;

      auto app = std::make_shared<XdgApplication>(file);

      m_apps.emplace_back(app);
      m_dataDirToApps[dir].emplace_back(app);

      appMap.insert({app->id(), app});
    }
  }

  return true;
}

std::vector<fs::path> XdgAppDatabase::defaultSearchPaths() const { return xdgpp::appDirs(); }

AppPtr XdgAppDatabase::findById(const QString &id) const {
  if (auto it = appMap.find(id); it != appMap.end()) { return it->second; }
  if (auto it = appMap.find(id + ".desktop"); it != appMap.end()) { return it->second; }

  return nullptr;
}

std::vector<AppPtr> XdgAppDatabase::findOpeners(const QString &target) const {
  return findAssociations(mimeNameForTarget(target));
}

// https://specifications.freedesktop.org/mime-apps-spec/latest/associations.html
std::vector<AppPtr> XdgAppDatabase::findAssociations(const QString &mimeName) const {
  std::set<std::string> seen;
  std::vector<AppPtr> openers;
  std::queue<QString> mimeStack;

  mimeStack.emplace(mimeName);

  while (!mimeStack.empty()) {
    auto mime = mimeStack.front();
    mimeStack.pop();

    for (const auto &parent : m_mimeDb.mimeTypeForName(mime).parentMimeTypes()) {
      mimeStack.emplace(parent);
    }

    std::set<std::string> removed;

    for (const auto &list : m_mimeAppsLists) {
      for (const auto &appId : list.defaultAssociations(mime.toStdString())) {
        if (auto appIt = appMap.find(appId.c_str()); appIt != appMap.end()) {
          seen.insert(appIt->second->id().toStdString());
          openers.emplace_back(appIt->second);
        }
      }

      for (const auto &appId : list.addedAssociations(mime.toStdString())) {
        if (removed.contains(appId) || seen.contains(appId)) continue;

        seen.insert(appId);
        if (auto appIt = appMap.find(appId.c_str()); appIt != appMap.end()) {
          openers.emplace_back(appIt->second);
        }
      }

      for (const auto &appId : list.removedAssociations(mime.toStdString())) {
        removed.insert(appId);
      }

      auto dataDir = list.path().parent_path();

      if (auto it = m_dataDirToApps.find(dataDir); it != m_dataDirToApps.end()) {
        for (const auto &app : it->second) {
          std::string appId = app->id().toStdString();

          if (removed.contains(appId) || seen.contains(appId)) continue;
          if (app->data().supportsMime(mime.toStdString())) {
            seen.insert(appId);
            openers.emplace_back(app);
          }
        }
      }
    }
  }

  return openers;
}

bool XdgAppDatabase::launch(const AbstractApplication &app, const std::vector<QString> &args) const {
  auto &xdgApp = static_cast<const XdgApplicationBase &>(app);
  auto exec = xdgApp.exec();

  if (exec.empty()) { return false; }

  QString program;
  QStringList argv;
  size_t offset = 0;

  if (xdgApp.isTerminalApp()) {
    if (auto emulator = findBestTerminalEmulator()) {
      auto xdgEmulator = static_cast<const XdgApplicationBase *>(emulator.get());
      if (auto exec = xdgEmulator->exec(); !exec.empty()) { program = exec.at(0); }
    }

    if (program.isEmpty()) {
      qWarning() << "XdgAppDatabase::launch: no default terminal could be found, we will default on the "
                    "generic 'xterm'";
      program = "xterm";
    }
    argv << "-e";
  } else {
    program = exec.at(0);
    offset = 1;
  }

  bool injected = false;

  for (size_t i = offset; i != exec.size(); ++i) {
    auto &part = exec.at(i);

    if (part == "%u" || part == "%f") {
      if (!args.empty()) argv << args.at(0);
      injected = true;
    } else if (part == "%U" || part == "%F") {
      for (const auto &arg : args) {
        argv.push_back(arg);
      }
      injected = true;
    } else {
      argv << part;
    }
  }

  // if no injection was possible, we simply append the args
  if (!injected) {
    for (const auto &arg : args) {
      argv << arg;
    }
  }

  QProcess process;

  process.setProgram(program);
  process.setArguments(argv);
  process.setStandardOutputFile(QProcess::nullDevice());
  process.setStandardErrorFile(QProcess::nullDevice());

  if (!process.startDetached()) {
    qWarning() << "Failed to start app" << argv << process.errorString();
    return false;
  }

  return true;
}

QString XdgAppDatabase::mimeNameForTarget(const QString &target) const {
  QString source = target;

  if (m_mimeDb.mimeTypeForName(source).isValid()) { return source; }

  {
    QUrl url(source);

    if (!url.scheme().isEmpty()) {
      if (url.scheme() != "file") { return "x-scheme-handler/" + url.scheme(); }
      source = url.toDisplayString(QUrl::RemoveScheme);
    }
  }

  auto mime = m_mimeDb.mimeTypeForFile(source);

  return mime.isValid() ? mime.name() : source;
}

AppPtr XdgAppDatabase::findByClass(const QString &name) const {
  // TODO: if that's too slow we might need a map
  QString normalizedWmClass = name.toLower();
  auto pred = [&](const QString &wmClass) { return wmClass.toLower() == normalizedWmClass; };

  for (const auto &app : m_apps) {
    if (std::ranges::any_of(app->windowClasses(), pred)) { return app; }
  }

  return nullptr;
}

std::vector<AppPtr> XdgAppDatabase::list() const { return {m_apps.begin(), m_apps.end()}; }

XdgAppDatabase::XdgAppDatabase() { scan(defaultSearchPaths()); }
