#include "xdg-app-database.hpp"
#include "vicinae.hpp"
#include <exception>
#include <filesystem>
#include <qlogging.h>
#include <qsettings.h>
#include <ranges>
#include <set>
#include <QDir>

namespace fs = std::filesystem;

using AppPtr = XdgAppDatabase::AppPtr;

static const std::vector<fs::path> wellKnownPaths = {"/usr/share/applications",
                                                     "/usr/local/share/applications"};

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
  mimeToApps.clear();
  appToMimes.clear();
  mimeToDefaultApp.clear();
  apps.clear();

  std::vector<fs::path> traversed;
  std::set<QString> processedIds; // Track which .desktop ids we've already processed

  // scan dirs
  for (const auto &dir : paths) {
    if (std::ranges::any_of(traversed, [&](auto &&path) { return path == dir; })) continue;

    traversed.emplace_back(dir);

    if (!fs::is_directory(dir)) continue;

    std::error_code ec;

    for (const auto &entry : fs::recursive_directory_iterator(dir, ec)) {
      if (ec) continue;
      if (entry.path().extension() != ".desktop") continue;

      try {
        XdgDesktopEntry desktopEntry(dir, entry.path().lexically_relative(dir));

        if (processedIds.contains(desktopEntry.id)) continue;
        processedIds.insert(desktopEntry.id);

        addDesktopFile(entry.path(), desktopEntry);
      } catch (std::exception &except) {
        qWarning() << "Failed to parse app at" << entry.path().c_str() << except.what();
      }
    }
  }

  auto toMimeApp = [](const fs::path &path) { return path / "mimeapps.list"; };
  auto isFile = [](const fs::path &path) {
    std::error_code ec;
    return fs::is_regular_file(path, ec);
  };
  // we reverse the config dir order to scan the directories with the least priority first
  auto mimeAppPaths = Omnicast::xdgConfigDirs() | std::views::reverse | std::views::transform(toMimeApp) |
                      std::views::filter(isFile);

  for (const auto &path : mimeAppPaths) {
    QSettings ini(path.c_str(), QSettings::IniFormat);

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

std::vector<AppPtr> XdgAppDatabase::findOpeners(const QString &mimeName) const {
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

  for (const auto &app : apps) {
    if (std::ranges::any_of(app->windowClasses(), pred)) { return app; }
  }

  return nullptr;
}

std::vector<AppPtr> XdgAppDatabase::list() const { return {apps.begin(), apps.end()}; }

void XdgAppDatabase::addDesktopFile(const fs::path &path, const XdgDesktopEntry &ent) {

  // we should not track hidden apps as they are explictly removed, unlike apps with NoDisplay
  // see: https://specifications.freedesktop.org/desktop-entry-spec/latest/recognized-keys.html
  if (ent.hidden) return;

  auto entry = std::make_shared<XdgApplication>(path, ent);

  for (const auto &mimeName : ent.mimeType) {
    mimeToApps[mimeName].insert(entry->id());
    appToMimes[entry->id()].insert(mimeName);
  }

  apps.push_back(entry);
  appMap.insert({entry->id(), entry});

  for (const auto &action : entry->actions()) {
    appMap.insert({action->id(), action});
  }
}

XdgAppDatabase::XdgAppDatabase() { scan(defaultSearchPaths()); }
