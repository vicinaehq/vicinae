#pragma once
#include "common.hpp"
#include "xdg/xdg-desktop.hpp"
#include <cctype>
#include <memory>
#include <qanystringview.h>
#include <qcontainerfwd.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qhash.h>
#include <qicon.h>
#include <qlist.h>
#include <qlogging.h>
#include <qmimedatabase.h>
#include <qmimetype.h>
#include <qprocess.h>
#include <qset.h>
#include <qsettings.h>
#include <qtenvironmentvariables.h>
#include <qurl.h>

// clang-format off
static const QList<QDir> defaultPaths = {
	QDir("/usr/share/applications"),
	QDir("/usr/local/share/applications"),
	QDir::homePath() + "/.local/share/applications"
};
// clang-format on

static const QString fieldCodeSet = "fFuUick";
static bool isLinkOpenerFieldCode(QString s) { return s == 'f' || s == 'F' || s == 'u' || s == 'U'; }

class DesktopExecutable {
public:
  QString id;
  QString name;
  QList<QString> exec;
  virtual QIcon icon() const = 0;
  virtual bool displayable() const = 0;
  virtual bool isTerminalApp() const = 0;
  virtual const QString &fullyQualifiedName() const { return name; }
  virtual const QString &iconName() const = 0;

  // whether the executable can open url(s) or file(s)
  bool isOpener() {
    for (const auto &arg : exec) {
      if (arg == "%u" || arg == "%U" || arg == "%f" || arg == "%F") return true;
    }

    return false;
  }

  DesktopExecutable() {}

  DesktopExecutable(const QString &id, const QString &name, const QList<QString> &exec)
      : id(id), name(name), exec(exec) {
    qDebug() << "dexecutable" << exec;
  }
};

class DesktopAction;

struct DesktopEntry : public DesktopExecutable {
  XdgDesktopEntry data;
  QString path;

  QList<std::shared_ptr<DesktopAction>> actions;

  DesktopEntry(const QString &path, const QString &id, const XdgDesktopEntry &data)
      : DesktopExecutable(id, data.name, data.exec), path(path), data(data) {}

  bool isTerminalApp() const override { return data.terminal; }
  const QString &iconName() const override { return data.icon; }
  QIcon icon() const override { return QIcon::fromTheme(data.icon); }
  bool displayable() const override { return !data.hidden && !data.noDisplay; };
};

struct DesktopAction : public DesktopExecutable {
  XdgDesktopEntry::Action data;
  std::shared_ptr<DesktopEntry> parent_;
  QString fqn_;

  DesktopAction(const XdgDesktopEntry::Action &action, std::shared_ptr<DesktopEntry> &parent)
      : DesktopExecutable(parent->id + "." + action.id, action.name, action.exec), data(action),
        parent_(parent), fqn_(parent->name + ": " + name) {}

  const QString &iconName() const override { return data.icon.isEmpty() ? parent_->iconName() : data.icon; }
  QIcon icon() const override {
    auto icon = QIcon::fromTheme(data.icon);

    if (icon.isNull()) return parent_->icon();

    return icon;
  }

  const QString &fullyQualifiedName() const override { return fqn_; }
  bool displayable() const override { return parent_->displayable(); }
  bool isTerminalApp() const override { return parent_->isTerminalApp(); }
};

class AppDatabase : public NonAssignable {

public:
  QList<QDir> paths;
  QHash<QString, std::shared_ptr<DesktopExecutable>> appMap;
  QHash<QString, QSet<QString>> mimeToApps;
  QHash<QString, QSet<QString>> appToMimes;
  QHash<QString, QString> mimeToDefaultApp;
  QMimeDatabase mimeDb;

  bool addDesktopFile(const QString &path) {
    QFileInfo info(path);
    XdgDesktopEntry ent(path);

    qDebug() << "exec" << ent.exec;

    auto entry = std::make_shared<DesktopEntry>(info.filePath(), info.fileName(), ent);

    for (const auto &mimeName : ent.mimeType) {
      mimeToApps[mimeName].insert(entry->id);
      appToMimes[entry->id].insert(mimeName);
    }

    apps.push_back(entry);
    appMap.insert(entry->id, entry);

    qDebug() << "add id " << entry->id;

    for (const auto &action : ent.actions) {
      auto ac = std::make_shared<DesktopAction>(action, entry);

      qDebug() << "add " << ac->id;

      entry->actions.push_back(ac);
      appMap.insert(ac->id, ac);
    }

    return true;
  }

public:
  QList<std::shared_ptr<DesktopEntry>> apps;

  // entry: firefox-esr.desktop
  // action: firefox-esr.desktop.open-in-private-window
  std::shared_ptr<DesktopExecutable> getById(const QString &id) {
    if (auto it = appMap.find(id); it != appMap.end()) return *it;

    return nullptr;
  }

  void launch(const DesktopExecutable &executable) { launch(executable, {}); }

  bool launch(const DesktopExecutable &executable, const QList<QString> &args) {
    if (executable.exec.isEmpty()) {
      qDebug() << "Empty Exec line, nothing to launch";
      return false;
    }

    QString program;
    QStringList argv;
    size_t offset = 0;

    if (executable.isTerminalApp()) {
      program = "alacritty";
      argv << "-e";
    } else {
      program = executable.exec.at(0);
      offset = 1;
    }

    for (size_t i = offset; i != executable.exec.size(); ++i) {
      auto &part = executable.exec.at(i);

      if (part == "%u" || part == "%f") {
        if (!args.isEmpty()) argv << args.at(0);
      } else if (part == "%U" || part == "%F")
        argv << args;
      else
        argv << part;
    }

    QProcess process;

    qDebug() << "launch" << program << argv;

    if (!process.startDetached(program, argv)) {
      qDebug() << executable.name << "failed to launch";
      return false;
    }

    return true;
  }

  std::shared_ptr<DesktopExecutable> findBestUrlOpener(const QUrl &url) {
    QString mime = "x-scheme-handler/" + url.scheme();

    if (auto appId = mimeToDefaultApp.value(url.scheme()); !appId.isEmpty()) {
      if (auto app = appMap.value(appId)) return app;
    }

    for (const auto &appId : mimeToApps.value(mime)) {
      if (auto app = appMap.value(appId)) return app;
    }

    return nullptr;
  }

  std::shared_ptr<DesktopExecutable> findBestOpenerForMime(QMimeType mime) {
    qDebug() << "find best opener for" << mime.name();
    if (auto app = defaultForMime(mime.name())) { return app; }

    for (const auto &mime : mime.parentMimeTypes()) {
      if (auto app = defaultForMime(mime)) return app;
    }

    if (auto it = mimeToApps.find(mime.name()); it != mimeToApps.end()) {
      for (const auto id : *it) {
        if (auto app = getById(id)) return app;
      }
    }

    return nullptr;
  }

  QList<std::shared_ptr<DesktopExecutable>> findMimeOpeners(const QString &mimeName) {
    QList<std::shared_ptr<DesktopExecutable>> apps;
    QSet<QString> seen;
    QList<QString> mimes = {mimeName};
    auto mime = mimeDb.mimeTypeForName(mimeName);

    mimes << mime.parentMimeTypes();

    for (const auto &name : mime.parentMimeTypes()) {
      qDebug() << "for mime name" << name;
      auto defaultApp = defaultForMime(name);

      if (defaultApp && !seen.contains(defaultApp->id)) {
        apps << defaultApp;
        seen.insert(defaultApp->id);
      }

      if (auto it = mimeToApps.find(name); it != mimeToApps.end()) {
        for (const auto id : *it) {
          if (seen.contains(id)) continue;
          if (auto app = getById(id)) {
            apps << app;
            seen.insert(id);
          }
        }
      }
    }

    return apps;
  }

  std::shared_ptr<DesktopExecutable> defaultBrowser() {
    static QList<QString> mimes{
        "x-scheme-handler/https", "x-scheme-handler/http", "text/html", "text/css", "text/javascript",
    };

    for (const auto &mime : mimes) {
      if (auto app = defaultForMime(mime)) return app;
    }

    return nullptr;
  }

  std::shared_ptr<DesktopExecutable> defaultFileBrowser() { return defaultForMime("inode/directory"); }

  std::shared_ptr<DesktopExecutable> defaultTextEditor() { return defaultForMime("text/plain"); }

  std::shared_ptr<DesktopExecutable> defaultForMime(const QString &mime) {
    if (auto it = mimeToDefaultApp.find(mime); it != mimeToDefaultApp.end()) {
      if (auto appIt = appMap.find(*it); appIt != appMap.end()) return *appIt;
    }

    return nullptr;
  }

  AppDatabase() {
    auto xdd = qgetenv("XDG_DATA_DIRS");

    if (!xdd.isEmpty()) {
      for (const auto &path : xdd.split(':')) {
        paths.push_back(QString(path) + "/applications");
      }
    } else {
      paths = defaultPaths;
    }

    QList<QDir> traversed;

    // scan dirs
    for (const auto &dir : paths) {
      if (traversed.contains(dir)) continue;

      traversed.push_back(dir);

      if (!dir.exists()) continue;

      for (const auto &entry : dir.entryList()) {
        if (!entry.endsWith(".desktop")) continue;

        QString fullpath = dir.path() + QDir::separator() + entry;

        addDesktopFile(fullpath);
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
        qDebug() << "add default app " << ini.value(key).toString() << " for mime " << key;
        mimeToDefaultApp.insert(key, ini.value(key).toString());
      }
      ini.endGroup();

      ini.beginGroup("Added Associations");
      for (const auto &mime : ini.childKeys()) {
        for (const auto app : ini.value(mime).toString().split(";")) {
          // add mime -> apps mapping
          if (auto it = mimeToApps.find(mime); it != mimeToApps.end()) {
            it->insert(app);
          } else {
            mimeToApps.insert(mime, {app});
          }

          // add app -> mimes mapping
          if (auto it = appToMimes.find(app); it != appToMimes.end()) {
            it->insert(mime);
          } else {
            appToMimes.insert(app, {mime});
          }
        }
      }
      ini.endGroup();

      ini.beginGroup("Removed Associations");
      for (const auto &mime : ini.childKeys()) {
        for (const auto app : ini.value(mime).toString().split(";")) {
          // add mime -> apps mapping
          if (auto it = mimeToApps.find(mime); it != mimeToApps.end()) { it->remove(app); }

          // add app -> mimes mapping
          if (auto it = appToMimes.find(app); it != appToMimes.end()) { it->remove(mime); }
        }
      }
      ini.endGroup();

      qDebug() << "scanning mimeapp" << path;
    }

    for (const auto &k : mimeToApps.keys()) {
      for (const auto &apps : mimeToApps[k]) {
        qDebug() << k << apps;
      }
    }

    for (const auto &apps : mimeToApps["text/plain"]) {
      qDebug() << "text/plain" << apps;
    }

    for (const auto &mime : mimeToDefaultApp.keys()) {
      qDebug() << mime << mimeToDefaultApp[mime];
    }
  }
};
