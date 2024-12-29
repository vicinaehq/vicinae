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
#include <qmimetype.h>
#include <qprocess.h>
#include <qset.h>
#include <qsettings.h>
#include <qtenvironmentvariables.h>

// clang-format off
static const QList<QDir> defaultPaths = {
	QDir("/usr/share/applications"),
	QDir("/usr/local/share/applications"),
	QDir::homePath() + "/.local/share/applications"
};
// clang-format on

static const QString fieldCodeSet = "fFuUick";
static bool isLinkOpenerFieldCode(QString s) {
  return s == 'f' || s == 'F' || s == 'u' || s == 'U';
}

class DesktopExecutable {
public:
  QString id;
  QString name;
  QString exec;
  virtual QIcon icon() const = 0;
  virtual bool displayable() const = 0;
  virtual bool isTerminalApp() const = 0;
  virtual const QString &fullyQualifiedName() const { return name; }
  virtual const QString &iconName() const = 0;

  DesktopExecutable() {}

  DesktopExecutable(const QString &id, const QString &name, const QString &exec)
      : id(id), name(name), exec(exec) {}

  bool launch(const QList<QString> &args) {
    /*
XdgCommandLine::Tokenizer tokenizer(exec);
QProcess proc;
std::optional<XdgCommandLine::Token> token;

QStringList cmdArgs;
size_t expandedCount = 0;

while ((token = tokenizer.next())) {
if (token->type == XdgCommandLine::TokenType::FieldCode) {
  if (token->value == 'f' || token->value == 'u' && !args.isEmpty()) {
    cmdArgs << args.at(0);
    ++expandedCount;
  } else if (token->value == 'F' || token->value == 'U') {
    cmdArgs << args;
    expandedCount = args.size();
  }
  continue;
}

cmdArgs << token->value;
}

// if some arguments have not been expanded, add them at the end of the
// command line
for (size_t i = expandedCount; i < args.size(); ++i) {
cmdArgs << args.at(i);
}

QStringList argv;

if (isTerminalApp()) {
argv << "/bin/alacritty";
argv << "-e";
argv << "/bin/sh";
argv << "-c";
argv << cmdArgs.join(" ");
} else {
argv << cmdArgs;
}

qDebug() << "Execute" << argv;

return proc.startDetached(argv.at(0), argv.sliced(1));
  */

    return false;
  }

  bool launch() { return launch({}); }
};

class DesktopAction;

struct DesktopEntry : public DesktopExecutable {
  XdgDesktopEntry data;
  QString path;

  QList<std::shared_ptr<DesktopAction>> actions;

  DesktopEntry(const QString &path, const QString &id,
               const XdgDesktopEntry &data)
      : DesktopExecutable(id, data.name, data.exec.join(' ')), path(path),
        data(data) {}

  bool isTerminalApp() const override { return data.terminal; }
  const QString &iconName() const override { return data.icon; }
  QIcon icon() const override { return QIcon::fromTheme(data.icon); }
  bool displayable() const override { return !data.hidden && !data.noDisplay; };
};

struct DesktopAction : public DesktopExecutable {
  XdgDesktopEntry::Action data;
  std::shared_ptr<DesktopEntry> parent_;
  QString fqn_;

  DesktopAction(const XdgDesktopEntry::Action &action,
                std::shared_ptr<DesktopEntry> &parent)
      : DesktopExecutable(parent->id + "." + action.id, action.name,
                          action.exec.join(' ')),
        data(action), parent_(parent), fqn_(parent->name + ": " + name) {}

  const QString &iconName() const override {
    return data.icon.isEmpty() ? parent_->iconName() : data.icon;
  }
  QIcon icon() const override {
    auto icon = QIcon::fromTheme(data.icon);

    if (icon.isNull())
      return parent_->icon();

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

  bool addDesktopFile(const QString &path) {
    QFileInfo info(path);
    XdgDesktopEntry ent(path);

    auto entry =
        std::make_shared<DesktopEntry>(info.filePath(), info.fileName(), ent);

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
    if (auto it = appMap.find(id); it != appMap.end())
      return *it;

    return nullptr;
  }

  std::shared_ptr<DesktopExecutable> findBestOpenerForMime(QMimeType mime) {
    if (auto app = defaultForMime(mime.name()))
      return app;

    for (const auto &mime : mime.parentMimeTypes()) {
      if (auto app = defaultForMime(mime))
        return app;
    }

    if (auto it = mimeToApps.find(mime.name()); it != mimeToApps.end()) {
      for (const auto id : *it) {
        if (auto app = getById(id))
          return app;
      }
    }

    return nullptr;
  }

  std::shared_ptr<DesktopExecutable> defaultBrowser() {
    static QList<QString> mimes{
        "x-scheme-handler/https",
        "x-scheme-handler/http",
        "text/html",
        "text/css",
        "text/javascript",
    };

    for (const auto &mime : mimes) {
      if (auto app = defaultForMime(mime))
        return app;
    }

    return nullptr;
  }

  std::shared_ptr<DesktopExecutable> defaultFileBrowser() {
    return defaultForMime("inode/directory");
  }

  std::shared_ptr<DesktopExecutable> defaultTextEditor() {
    return defaultForMime("text/plain");
  }

  std::shared_ptr<DesktopExecutable> defaultForMime(const QString &mime) {
    if (auto it = mimeToDefaultApp.find(mime); it != mimeToDefaultApp.end()) {
      if (auto appIt = appMap.find(*it); appIt != appMap.end())
        return *appIt;
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
      if (traversed.contains(dir))
        continue;

      traversed.push_back(dir);

      if (!dir.exists())
        continue;

      for (const auto &entry : dir.entryList()) {
        if (!entry.endsWith(".desktop"))
          continue;

        QString fullpath = dir.path() + QDir::separator() + entry;

        addDesktopFile(fullpath);
      }
    }

    QString configHome = qgetenv("XDG_CONFIG_HOME");

    if (configHome.isEmpty())
      configHome = QDir::homePath() + QDir::separator() + ".config";

    QList<QDir> mimeappDirs;

    mimeappDirs.push_back(configHome);
    mimeappDirs.push_back(QDir("/etc/xdg"));

    for (const auto &dir : mimeappDirs) {
      QString path = dir.path() + QDir::separator() + "mimeapps.list";
      QSettings ini(path, QSettings::IniFormat);

      ini.beginGroup("Default Applications");
      for (const auto &key : ini.allKeys()) {
        qDebug() << "add default app " << ini.value(key).toString()
                 << " for mime " << key;
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
          if (auto it = mimeToApps.find(mime); it != mimeToApps.end()) {
            it->remove(app);
          }

          // add app -> mimes mapping
          if (auto it = appToMimes.find(app); it != appToMimes.end()) {
            it->remove(mime);
          }
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
