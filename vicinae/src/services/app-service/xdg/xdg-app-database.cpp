#include "xdg-app-database.hpp"
#include "services/app-service/xdg/xdg-app.hpp"
#include "timer.hpp"
#include "xdgpp/desktop-entry/file.hpp"
#include "xdgpp/mime/iterator.hpp"
#include <filesystem>
#include <qlogging.h>
#include <qsettings.h>

#include <queue>
#include <qurl.h>
#include <set>
#include <QDir>
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

  auto openers = findAssociations(mime);

  return openers.empty() ? nullptr : openers.front();
}

AppPtr XdgAppDatabase::findDefaultOpener(const QString &target) const {
  if (QUrl url = target; url.isValid()) {
    if (auto scheme = url.scheme(); !scheme.isEmpty()) {
      if (scheme == "http" || scheme == "https") return webBrowser();
      if (scheme == "file") return fileBrowser();
    }
  }

  return defaultForMime(mimeNameForTarget(target));
}

bool XdgAppDatabase::scan(const std::vector<std::filesystem::path> &paths) {
  appMap.clear();
  m_apps.clear();
  m_mimeAppsLists.clear();
  m_dataDirToApps.clear();
  m_wmClassToApp.clear();
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

      if (file.deleted()) continue;
      if (file.errorMessage()) {
        qWarning() << "Desktop file" << file.path().c_str() << "is invalid" << *file.errorMessage();
        continue;
      }

      auto app = std::make_shared<XdgApplication>(file);

      m_apps.emplace_back(app);
      m_dataDirToApps[dir].emplace_back(app);

      if (auto wmClass = app->wmClass()) { m_wmClassToApp[wmClass->toLower()] = app; }

      appMap[app->id()] = app;

      for (const auto &action : app->actions()) {
        appMap[action->id()] = action;
      }
    }
  }

  return true;
}

AppPtr XdgAppDatabase::terminalEmulator() const {
  if (auto emulator = defaultForMime("x-scheme-handler/terminal")) { return emulator; }

  qWarning()
      << "No default terminal emulator could be found, will fallback on the first terminal emulator "
         "we find. To learn how to set one for vicinae to use: https://docs.vicinae.com/default-terminal";

  for (const auto &app : m_apps) {
    if (app->isTerminalEmulator()) return app;
  }

  return nullptr;
}

AppPtr XdgAppDatabase::fileBrowser() const {
  if (auto browser = defaultForMime("inode/directory")) { return browser; }
  if (auto browser = findByCategory("FileManager")) { return browser; }

  for (const auto &app : m_apps) {
    if (app->data().supportsMime("inode/directory")) { return app; }
  }

  return nullptr;
}

AppPtr XdgAppDatabase::genericTextEditor() const {
  if (auto editor = defaultForMime("text/plain")) { return editor; }
  if (auto editor = findByCategory("TextEditor")) { return editor; }

  for (const auto &app : m_apps) {
    if (app->data().supportsMime("text/plain")) { return app; }
  }

  return nullptr;
}

AppPtr XdgAppDatabase::webBrowser() const {
  if (auto browser = defaultForMime("x-scheme-handler/https")) { return browser; }
  if (auto browser = defaultForMime("x-scheme-handler/http")) { return browser; }
  if (auto browser = defaultForMime("text/html")) { return browser; }
  if (auto browser = findByCategory("WebBrowser")) { return browser; }

  for (const auto &app : m_apps) {
    if (app->data().supportsMime("x-scheme-handler/https") ||
        app->data().supportsMime("x-scheme-handler/http")) {
      return app;
    }
  }

  return nullptr;
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

  qDebug() << "find associations for" << mimeName;

  mimeStack.emplace(mimeName);

  while (!mimeStack.empty()) {
    auto mime = mimeStack.front();
    mimeStack.pop();

    for (const auto &parent : m_mimeDb.mimeTypeForName(mime).parentMimeTypes()) {
      mimeStack.emplace(parent);
    }

    std::set<std::string> removed;

    // perform a full file tour to find the default if there is one
    for (const auto &list : m_mimeAppsLists) {
      for (const auto &appId : list.defaultAssociations(mime.toStdString())) {
        if (auto appIt = appMap.find(appId.c_str()); appIt != appMap.end()) {
          seen.insert(appIt->second->id().toStdString());
          openers.emplace_back(appIt->second);
          break;
        }
      }
    }

    for (const auto &list : m_mimeAppsLists) {
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
  auto &xdgApp = static_cast<const XdgApplication &>(app);
  auto exec = xdgApp.parseExec(args);

  if (exec.empty()) {
    qWarning() << "No program to start the app";
    return false;
  }

  QProcess process;
  QStringList argv;

  if (xdgApp.isTerminalApp()) {
    if (auto emulator = terminalEmulator()) {
      process.setProgram(emulator->program());

      // because yes, gnome-terminal does not support -e properly
      if (emulator->program() == "gnome-terminal") {
        argv << "--";
      } else {
        argv << "-e";
      }

      for (const auto &part : exec) {
        argv << part;
      }
    }
  } else {
    process.setProgram(exec.at(0));
    for (int i = 1; i < exec.size(); ++i) {
      argv << exec[i];
    }
  }

  process.setArguments(argv);
  process.setStandardOutputFile(QProcess::nullDevice());
  process.setStandardErrorFile(QProcess::nullDevice());

  if (auto wd = xdgApp.data().workingDirectory(); wd && !wd->empty()) {
    process.setWorkingDirectory(wd->c_str());
  }

  if (!process.startDetached()) {
    qWarning() << "Failed to start app" << argv << process.errorString();
    return false;
  }

  return true;
}

QString XdgAppDatabase::mimeNameForTarget(const QString &target) const {
  QString source = target;

  {
    QUrl url(source);

    if (!url.scheme().isEmpty()) {
      if (url.scheme() != "file") { return "x-scheme-handler/" + url.scheme(); }
      source = url.toDisplayString(QUrl::RemoveScheme);
    }
  }

  if (m_mimeDb.mimeTypeForName(source).isValid()) { return source; }

  auto mime = m_mimeDb.mimeTypeForFile(source);

  return mime.isValid() ? mime.name() : source;
}

AppPtr XdgAppDatabase::findByCategory(const QString &category) const {
  auto pred = [&](const std::shared_ptr<XdgApplication> &app) {
    return app->data().hasCategory(category.toStdString());
  };
  if (auto it = std::ranges::find_if(m_apps, pred); it != m_apps.end()) { return *it; }
  return nullptr;
}

AppPtr XdgAppDatabase::findByClass(const QString &name) const {
  // TODO: if that's too slow we might need a map
  QString normalizedWmClass = name.toLower();

  // try direct wm class match first
  if (auto it = m_wmClassToApp.find(normalizedWmClass); it != m_wmClassToApp.end()) { return it->second; }

  auto pred = [&](const QString &str) { return str.toLower() == normalizedWmClass; };

  // try to find by id
  for (const auto &app : m_apps) {
    if (std::ranges::any_of(app->windowClasses(), pred)) { return app; }
  }

  return nullptr;
}

std::vector<AppPtr> XdgAppDatabase::list() const { return {m_apps.begin(), m_apps.end()}; }

XdgAppDatabase::XdgAppDatabase() { scan(defaultSearchPaths()); }
