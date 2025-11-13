#include "xdg-app-database.hpp"
#include "environment.hpp"
#include "services/app-service/xdg/xdg-app.hpp"
#include "timer.hpp"
#include "xdgpp/desktop-entry/file.hpp"
#include "xdgpp/mime/iterator.hpp"
#include <algorithm>
#include <qtenvironmentvariables.h>
#include <ranges>
#include "xdgpp/desktop-entry/exec.hpp"
#include <filesystem>
#include <memory>
#include <qcontainerfwd.h>
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

  auto terms = m_apps | std::views::filter([](auto &&app) { return app->isTerminalEmulator(); });
  auto findWithProg = [&](const QString &prog) -> std::optional<AppPtr> {
    if (auto it = std::ranges::find_if(terms, [&](auto &&app) { return app->program() == prog; });
        it != terms.end()) {
      return *it;
    }
    return std::nullopt;
  };

  if (Environment::isGnomeEnvironment()) {
    if (auto target = findWithProg("kgx")) { return *target; }
    if (auto target = findWithProg("gnome-terminal")) { return *target; }
  }

  else if (Environment::isPlasmaDesktop()) {
    if (auto target = findWithProg("konsole")) { return *target; }
  }

  else if (Environment::isCosmicDesktop()) {
    if (auto target = findWithProg("cosmic-term")) { return *target; }
  }

  if (!terms.empty()) { return terms.front(); }

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

xdgpp::DesktopEntry::TerminalExec XdgAppDatabase::getTermExec(const XdgApplication &app) const {
  using TExec = xdgpp::DesktopEntry::TerminalExec;

  if (app.program() == "gnome-terminal") { return TExec{.exec = "--"}; }
  // new gnome terminal
  if (app.program() == "kgx") { return TExec{.exec = "--", .title = "--title", .dir = "working-directory"}; }
  if (app.program() == "alacritty") {
    return TExec{
        .exec = "-e",
        .appId = "--class",
        .title = "--title",
        .dir = "--working-directory",
        .hold = "--hold",
    };
  }
  if (app.program() == "cosmic-term") { return TExec{.exec = "-e"}; }
  if (app.program() == "konsole") {
    return TExec{
        .exec = "-e",
        .dir = "--workdir",
        .hold = "--hold",
    };
  }
  if (app.program() == "foot") {
    return TExec{
        .exec = "-e",
        .appId = "--app-id",
        .title = "--title",
        .dir = "--working-directory",
        .hold = "--hold",
    };
  }
  if (app.program() == "mate-terminal") { return TExec{.exec = "-x"}; }
  if (app.program() == "xfce4-terminal") { return TExec{.exec = "-x"}; }

  return {.exec = "-e"};
}

bool XdgAppDatabase::launchTerminalCommand(const std::vector<QString> &cmdline,
                                           const LaunchTerminalCommandOptions &opts,
                                           const std::optional<QString> &prefix) const {
  if (cmdline.empty()) return false;

  auto terminal = std::static_pointer_cast<XdgApplication>(terminalEmulator());
  auto exec = terminal->parseExec({}, prefix);

  if (exec.empty()) return false;

  QStringList argv;
  std::ranges::for_each(exec | std::views::drop(1), [&](auto &&arg) { argv << arg; });
  auto texec = getTermExec(*terminal);

  if (texec.appId && opts.appId) { argv << texec.appId->c_str() << opts.appId.value(); }
  if (texec.title && opts.title) { argv << texec.title->c_str() << opts.title.value(); }
  if (texec.dir && opts.workingDirectory) { argv << texec.dir->c_str() << opts.workingDirectory.value(); }
  if (texec.hold && opts.hold) { argv << texec.hold->c_str(); }
  if (texec.exec) { argv << texec.exec->c_str(); }

  for (const auto &arg : cmdline) {
    argv << arg;
  }

  return launchProcess(exec.front(), argv, terminal->data().workingDirectory());
}

bool XdgAppDatabase::launchProcess(const QString &prog, const QStringList args,
                                   const std::optional<std::filesystem::path> &workingDirectory) const {
  QProcess process;
  process.setProgram(prog);
  process.setArguments(args);
  process.setStandardOutputFile(QProcess::nullDevice());
  process.setStandardErrorFile(QProcess::nullDevice());

  if (auto dir = workingDirectory) { process.setWorkingDirectory(workingDirectory->c_str()); }

  QStringList cmdline;
  cmdline << prog << args;
  qInfo() << "App started with command line" << cmdline.join(' ');

  if (!process.startDetached()) {
    qWarning() << "Failed to start app:" << process.errorString();
    return false;
  }

  return true;
}

bool XdgAppDatabase::launch(const AbstractApplication &app, const std::vector<QString> &args,
                            const std::optional<QString> &launchPrefix) const {
  auto &xdgApp = static_cast<const XdgApplication &>(app);

  if (auto url = xdgApp.data().url().transform(QString::fromStdString)) {
    auto opener = findDefaultOpener(*url);

    if (!opener) {
      qWarning() << "No opener for link entry with url" << url;
      return false;
    }

    return launch(*opener, {*url}, launchPrefix);
  }

  if (xdgApp.isTerminalApp()) return launchTerminalCommand(xdgApp.parseExec(args), {}, launchPrefix);

  auto exec = xdgApp.parseExec(args, launchPrefix);

  if (exec.empty()) {
    qWarning() << "Failed to launch app" << app.id() << "exec command line empty";
    return false;
  }

  auto argv = exec | std::views::drop(1) | std::ranges::to<QStringList>();

  return launchProcess(exec.front(), argv, xdgApp.data().workingDirectory());
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
