#include "app/xdg-app-database.hpp"
#include "app/app-database.hpp"
#include <filesystem>
#include <qlogging.h>
#include <qsettings.h>
#include <ranges>

namespace fs = std::filesystem;

using AppPtr = XdgAppDatabase::AppPtr;

static const std::vector<fs::path> wellKnownPaths = {"/usr/share/applications",
                                                     "/usr/local/share/applications"};

// QDir::homePath() + "/.local/share/applications"};

std::shared_ptr<Application> XdgAppDatabase::defaultForMime(const QString &mime) const {
  if (auto it = mimeToDefaultApp.find(mime); it != mimeToDefaultApp.end()) {
    if (auto appIt = appMap.find(it->second); appIt != appMap.end()) { return appIt->second; }
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

bool XdgAppDatabase::scan(const std::vector<std::filesystem::path> &paths) {
  appMap.clear();
  mimeToApps.clear();
  appToMimes.clear();
  mimeToDefaultApp.clear();
  apps.clear();

  qCritical() << "XdgAppDatabase::scan";

  std::vector<fs::path> traversed;

  // scan dirs
  for (const auto &dir : paths) {
    if (std::ranges::any_of(traversed, [&](auto &&path) { return path == dir; })) continue;

    traversed.emplace_back(dir);

    if (!fs::is_directory(dir)) continue;

    std::error_code ec;

    for (const auto &entry : fs::directory_iterator(dir, ec)) {
      if (ec) continue;
      std::string filename = entry.path().filename().string();
      if (!filename.ends_with(".desktop")) continue;

      addDesktopFile(entry.path().c_str());
    }
  }

  QString configHome = qgetenv("XDG_CONFIG_HOME");

  if (configHome.isEmpty()) configHome = QDir::homePath() + QDir::separator() + ".config";

  QList<QDir> mimeappDirs;

  mimeappDirs.push_back(configHome);
  mimeappDirs.push_back(QDir("/etc/xdg"));

  for (const auto &dir : mimeappDirs) {
    QString path = dir.path() + QDir::separator() + "mimeapps.list";
    QSettings ini(path, QSettings::IniFormat);

    ini.beginGroup("Default Applications");
    for (const auto &key : ini.allKeys()) {
      auto appId = ini.value(key).toString();

      mimeToDefaultApp[key] = appId;
    }
    ini.endGroup();

    ini.beginGroup("Added Associations");
    for (const auto &mime : ini.childKeys()) {
      for (const auto app : ini.value(mime).toString().split(";")) {
        // add mime -> apps mapping
        if (auto it = mimeToApps.find(mime); it != mimeToApps.end()) {
          it->second.insert(app);
        } else {
          mimeToApps.insert({mime, {app}});
        }

        // add app -> mimes mapping
        if (auto it = appToMimes.find(app); it != appToMimes.end()) {
          it->second.insert(mime);
        } else {
          appToMimes.insert({app, {mime}});
        }
      }
    }
    ini.endGroup();

    ini.beginGroup("Removed Associations");
    for (const auto &mime : ini.childKeys()) {
      for (const auto app : ini.value(mime).toString().split(";")) {
        // add mime -> apps mapping
        if (auto it = mimeToApps.find(mime); it != mimeToApps.end()) { mimeToApps.erase(it); }

        // add app -> mimes mapping
        if (auto it = appToMimes.find(app); it != appToMimes.end()) { appToMimes.erase(it); }
      }
    }
    ini.endGroup();
  }

  return true;
}

std::vector<fs::path> XdgAppDatabase::defaultSearchPaths() const {
  char *ddir = std::getenv("XDG_DATA_DIRS");

  if (!ddir) { return wellKnownPaths; }

  std::string s = ddir;
  std::vector<fs::path> paths;

  for (const auto p : std::views::split(s, std::string_view(":"))) {
    fs::path appDir = fs::path(std::string_view(p)) / "applications";

    paths.emplace_back(appDir);
  }

  return paths;
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

  if (auto app = defaultForMime(mime.name())) { return app; }

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

AppPtr XdgAppDatabase::findById(const QString &id) const {
  if (auto it = appMap.find(id); it != appMap.end()) { return it->second; }
  if (auto it = appMap.find(id + ".desktop"); it != appMap.end()) { return it->second; }

  return nullptr;
}

std::vector<AppPtr> XdgAppDatabase::findOpeners(const QString &mimeName) const {
  QUrl url(mimeName);

  if (!url.scheme().isEmpty()) {
    std::vector<AppPtr> apps;
    QString mime = "x-scheme-handler/" + url.scheme();

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

  for (const auto &name : mime.parentMimeTypes()) {
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
}

bool XdgAppDatabase::launch(const Application &app, const std::vector<QString> &args) const {
  auto &xdgApp = static_cast<const XdgApplicationBase &>(app);
  auto exec = xdgApp.exec();

  if (exec.empty()) { return false; }

  QString program;
  QStringList argv;
  size_t offset = 0;

  if (xdgApp.isTerminalApp()) {
    if (auto emulator = findBestOpenerForMime("x-scheme-handler/terminal")) {
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

  qDebug() << "args" << args.size();

  for (size_t i = offset; i != exec.size(); ++i) {
    auto &part = exec.at(i);

    if (part == "%u" || part == "%f") {
      if (!args.empty()) argv << args.at(0);
    } else if (part == "%U" || part == "%F") {
      for (const auto &arg : args) {
        argv.push_back(arg);
      }
    } else {
      argv << part;
    }
  }

  QProcess process;

  process.setProgram(program);
  process.setArguments(argv);
  process.setStandardOutputFile(QProcess::nullDevice());
  process.setStandardErrorFile(QProcess::nullDevice());

  if (!process.startDetached()) {
    qDebug() << xdgApp.name() << "failed to launch";
    return false;
  }

  return true;
}

AppPtr XdgAppDatabase::findByClass(const QString &name) const {
  QString normalizedWmClass = name.toLower();

  for (const auto &app : apps) {
    if (app->name().toLower() == normalizedWmClass) return app;
  }

  return nullptr;
}

std::vector<AppPtr> XdgAppDatabase::list() const { return {apps.begin(), apps.end()}; }

bool XdgAppDatabase::addDesktopFile(const QString &path) {
  QFileInfo info(path);
  XdgDesktopEntry ent(path);

  auto entry = std::make_shared<XdgApplication>(info, ent);

  for (const auto &mimeName : ent.mimeType) {
    mimeToApps[mimeName].insert(entry->id());
    appToMimes[entry->id()].insert(mimeName);
  }

  apps.push_back(entry);
  appMap.insert({entry->id(), entry});

  for (const auto &action : entry->actions()) {
    appMap.insert({action->id(), action});
  }

  return true;
}

XdgAppDatabase::XdgAppDatabase() { scan(defaultSearchPaths()); }
