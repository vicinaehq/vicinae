#include "xdg-app-database.hpp"
#include "xdgpp/desktop-entry/entry.hpp"
#include "xdgpp/desktop-entry/file.hpp"
#include <filesystem>
#include <qlogging.h>
#include <qsettings.h>
#include <ranges>
#include <set>
#include <QDir>
#include <xdgpp/desktop-entry/iterator.hpp>
#include <xdgpp/env/env.hpp>
#include <xdgpp/mime/mime-db.hpp>
#include <xdgpp/mime/mime-apps-list.hpp>

namespace fs = std::filesystem;

using AppPtr = XdgAppDatabase::AppPtr;

std::shared_ptr<Application> XdgAppDatabase::defaultForMime(const QString &mime) const {

  for (const auto &list : m_mimeAppsLists) {
    for (const auto &appId : list.defaultAssociations(mime.toStdString())) {
      if (auto appIt = appMap.find(appId.c_str()); appIt != appMap.end()) { return appIt->second; }
    }
  }

  return nullptr;
}

AppPtr XdgAppDatabase::findBestOpenerForMime(const QString &mimeName) const {
  QMimeType mime = mimeDb.mimeTypeForName(mimeName);

  if (auto app = defaultForMime(mimeName)) { return app; }

  for (const auto &mime : mime.parentMimeTypes()) {
    if (auto app = defaultForMime(mime)) return app;
  }

  if (auto it = mimeToApps.find(mime.name()); it != mimeToApps.end()) {
    for (const auto id : it->second) {
      if (auto app = findById(id)) return app;
    }
  }

  return nullptr;
}

AppPtr XdgAppDatabase::findBestTerminalEmulator() const {
  if (auto emulator = findBestOpenerForMime("x-scheme-handler/terminal")) { return emulator; }

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

  QMimeDatabase db;

  for (const auto &path : xdgpp::MimeDatabase::mimeappsPaths()) {
    m_mimeAppsLists.emplace_back(xdgpp::MimeAppsList::fromFile(path));
  }

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

std::vector<fs::path> XdgAppDatabase::defaultSearchPaths() const {
  return xdgpp::appDirs();

  /*
  std::vector<fs::path> paths;

  // First, add XDG_DATA_HOME (highest priority after manually added paths)
  char *dataHome = std::getenv("XDG_DATA_HOME");
  if (dataHome) {
    fs::path appDir = fs::path(dataHome) / "applications";
    paths.emplace_back(appDir);
  } else {
    // Default to $HOME/.local/share/applications if XDG_DATA_HOME is not set
    QString homeDir = QDir::homePath();
    fs::path appDir = fs::path(homeDir.toStdString()) / ".local" / "share" / "applications";
    paths.emplace_back(appDir);
  }

  // Then add XDG_DATA_DIRS
  char *ddir = std::getenv("XDG_DATA_DIRS");
  if (ddir) {
    std::string s = ddir;
    for (const auto p : std::views::split(s, std::string_view(":"))) {
      fs::path appDir = fs::path(std::string_view(p.begin(), p.end())) / "applications";
      paths.emplace_back(appDir);
    }
  } else {
    // Fallback to well-known paths if XDG_DATA_DIRS is not set
    paths.insert(paths.end(), wellKnownPaths.begin(), wellKnownPaths.end());
  }

  return paths;
  */
}

XdgAppDatabase::AppPtr XdgAppDatabase::findBestOpener(const QString &target) const {
  QUrl url(target);

  if (!url.scheme().isEmpty()) {
    QString mime = "x-scheme-handler/" + url.scheme();

    if (auto it = mimeToDefaultApp.find(mime); it != mimeToDefaultApp.end()) {
      if (auto it2 = appMap.find(it->second); it2 != appMap.end()) return it2->second;
    }

    if (auto it = mimeToApps.find(mime); it != mimeToApps.end()) {
      for (const auto &appId : it->second) {
        if (auto it2 = appMap.find(appId); it2 != appMap.end()) return it2->second;
      }
    }
  }

  QMimeType mime = mimeDb.mimeTypeForFile(target);

  if (mime.isValid()) {
    if (auto app = defaultForMime(mime.name())) { return app; }

    for (const auto &mime : mime.parentMimeTypes()) {
      if (auto app = defaultForMime(mime)) return app;
    }

    if (auto it = mimeToApps.find(mime.name()); it != mimeToApps.end()) {
      for (const auto id : it->second) {
        if (auto app = findById(id)) return app;
      }
    }
  }

  if (auto app = findBestOpenerForMime(target)) { return app; }

  return nullptr;
}

AppPtr XdgAppDatabase::findById(const QString &id) const {
  if (auto it = appMap.find(id); it != appMap.end()) { return it->second; }
  if (auto it = appMap.find(id + ".desktop"); it != appMap.end()) { return it->second; }

  return nullptr;
}

// https://specifications.freedesktop.org/mime-apps-spec/latest/associations.html
std::vector<AppPtr> XdgAppDatabase::findOpeners(const QString &mimeName) const {
  std::set<std::string> seen;
  std::vector<AppPtr> openers;
  QMimeDatabase db;

  auto mime = db.mimeTypeForName(mimeName);
  auto parentMimes = mime.parentMimeTypes();
  std::vector<QString> mimes;

  mimes.emplace_back(mimeName);
  mimes.insert(mimes.end(), parentMimes.begin(), parentMimes.end());

  for (const auto &mime : mimes) {
    std::set<std::string> removed;

    size_t i = 0;
    auto mimeappsPaths = xdgpp::MimeDatabase::mimeappsPaths();

    for (const auto &list : m_mimeAppsLists) {
      auto dataDir = mimeappsPaths.at(i++).parent_path();

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

  /*
  QUrl url(mimeName);

  if (!url.scheme().isEmpty()) {
    std::vector<AppPtr> apps;
    QString mime = url.scheme() == "file" ? "inode/directory" : "x-scheme-handler/" + url.scheme();

    if (auto it = mimeToDefaultApp.find(mime); it != mimeToDefaultApp.end()) {
      if (auto it2 = appMap.find(it->second); it2 != appMap.end()) { apps.emplace_back(it2->second); }
    }

    if (auto it = mimeToApps.find(mime); it != mimeToApps.end()) {
      for (const auto &appId : it->second) {
        if (auto it2 = appMap.find(appId); it2 != appMap.end()) {
          bool alreadyIn = std::ranges::any_of(apps, [&](auto &&app) { return app->id() == appId; });
          if (!alreadyIn) { apps.emplace_back(it2->second); }
        }
      }
    }

    return apps;
  }

  std::vector<AppPtr> apps;
  std::set<QString> seen;
  std::vector<QString> mimes = {mimeName};
  auto mime = mimeDb.mimeTypeForName(mimeName);

  for (const auto &mime : mime.parentMimeTypes()) {
    mimes.push_back(mime);
  }

  for (const auto &name : mimes) {
    auto defaultApp = defaultForMime(name);

    if (defaultApp && !seen.contains(defaultApp->id())) {
      apps.push_back(defaultApp);
      seen.insert(defaultApp->id());
    }

    if (auto it = mimeToApps.find(name); it != mimeToApps.end()) {
      for (const auto id : it->second) {
        if (seen.contains(id)) continue;
        if (auto app = findById(id)) {
          apps.push_back(app);
          seen.insert(id);
        }
      }
    }
  }

  return apps;
  */
}

bool XdgAppDatabase::launch(const Application &app, const std::vector<QString> &args) const {
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
