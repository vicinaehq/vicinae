#pragma once
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
#include <stdexcept>

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

struct XdgCommandLine {
  enum TokenType {
    String,
    FieldCode,
  };

  struct Token {
    TokenType type;
    QString value;
  };

  struct Tokenizer {
    const QString &s;
    size_t cursor = 0;
    enum State { START, PERCENT, FIELD_CODE, STRING, WS, QUOTED, ESCAPED };
    State state = START;

    Tokenizer(const QString &s) : s(s) { qDebug() << "phrase" << s; }

    std::optional<XdgCommandLine::Token> next() {
      size_t i = cursor;
      QString arg;

      if (i >= s.size())
        return std::nullopt;

      while (true) {
        QChar c = i < s.size() ? s.at(i) : QChar(0);

        switch (state) {
        case START:
          if (c == '%')
            state = PERCENT;
          else if (c == '\\')
            state = ESCAPED;
          else if (c == '"')
            state = QUOTED;
          else if (c.isSpace())
            state = WS;
          else if (c.isPrint()) {
            arg += c;
            state = STRING;
          } else
            return std::nullopt;
          break;
        case STRING:
          if (c.isPrint() && !c.isSpace())
            arg += c;
          else {
            state = START;
            cursor = i + 1;
            return XdgCommandLine::Token{XdgCommandLine::TokenType::String,
                                         arg};
          }
          break;
        case WS:
          if (!c.isSpace()) {
            state = START;
          }
          break;
        case PERCENT:
          if (c == '%') {
            arg += c;
            state = START;
            break;
          }

          arg = c;
          state = FIELD_CODE;
          break;
        case FIELD_CODE:
          if (c.isLetterOrNumber()) {
            state = START;
            --i;
            break;
          }

          state = START;

          // ignore field code if not recognized
          if (!fieldCodeSet.contains(arg))
            break;

          cursor = i;
          return XdgCommandLine::Token{XdgCommandLine::TokenType::FieldCode,
                                       arg};

        case QUOTED:
          qDebug() << "Quoted";
          if (c == '"') {
            state = START;
            cursor = i + 1;
            return XdgCommandLine::Token{XdgCommandLine::TokenType::String,
                                         arg};
          } else {
            arg += c;
          }

          break;
        case ESCAPED:
          arg += c;
          state = START;
          break;
        }

        ++i;
      }

      return std::nullopt;
    }
  };
};

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
  }

  bool launch() { return launch({}); }
};

class DesktopAction;

struct DesktopEntry : public DesktopExecutable {
  QString path;
  QString icon_;

  bool terminal;
  QList<QString> categories;
  QList<QString> keywords;
  QList<QString> mimeTypes;

  bool noDisplay;
  bool hidden;

  QList<std::shared_ptr<DesktopAction>> actions;

  bool isTerminalApp() const override { return terminal; }
  const QString &iconName() const override { return icon_; }
  QIcon icon() const override { return QIcon::fromTheme(icon_); }
  bool displayable() const override { return !hidden && !noDisplay; };
};

struct DesktopAction : public DesktopExecutable {
  QString id_;
  std::shared_ptr<DesktopEntry> parent_;
  QString absolute_id_;
  QString icon_;
  QString fqn_;

  DesktopAction(std::shared_ptr<DesktopEntry> parent) : parent_(parent) {}

  DesktopAction(const QString &id, QSettings &ini,
                std::shared_ptr<DesktopEntry> &parent)
      : DesktopExecutable(parent->id + "." + id, ini.value("Name").toString(),
                          ini.value("Exec").toString()),
        parent_(parent), icon_(ini.value("Icon").toString()),
        fqn_(parent->name + ": " + name) {}

  const QString &iconName() const override {
    return icon_.isEmpty() ? parent_->icon_ : icon_;
  }
  QIcon icon() const override {
    auto icon = QIcon::fromTheme(icon_);

    if (icon.isNull())
      return parent_->icon();

    return icon;
  }

  const QString &fullyQualifiedName() const override { return fqn_; }
  bool displayable() const override { return parent_->displayable(); }
  bool isTerminalApp() const override { return parent_->terminal; }
};

class AppDatabase {

public:
  QList<QDir> paths;
  QHash<QString, std::shared_ptr<DesktopExecutable>> appMap;
  QHash<QString, QSet<QString>> mimeToApps;
  QHash<QString, QSet<QString>> appToMimes;
  QHash<QString, QString> mimeToDefaultApp;

  bool addDesktopFile(const QString &path) {
    QFileInfo info(path);
    QSettings ini(info.filePath(), QSettings::IniFormat);
    auto groups = ini.childGroups();
    std::shared_ptr<DesktopEntry> entry = nullptr;

    if (appMap.find(info.fileName()) != appMap.end())
      return false;

    for (const auto &group : groups) {
      auto ss = group.split(' ');

      if ((ss.size() == 2 && ss[0].toLower() == "desktop" &&
           ss[1].toLower() == "entry")) {

        entry = std::make_shared<DesktopEntry>();
        ini.beginGroup(group);
        entry->path = path;
        entry->path = path;
        entry->id = info.fileName();
        entry->name = ini.value("Name").toString();
        entry->exec = ini.value("Exec").toString();
        entry->icon_ = ini.value("Icon").toString();
        entry->terminal = ini.value("Terminal", false).toBool();
        entry->categories = ini.value("Categories").toString().split(' ');
        entry->keywords = ini.value("Keywords").toString().split(' ');
        entry->mimeTypes = ini.value("MimeType").toString().split(' ');
        entry->noDisplay = ini.value("NoDisplay", false).toBool();
        entry->hidden = ini.value("Hidden", false).toBool();
        ini.endGroup();
        break;
      }
    }

    if (!entry)
      return false;

    apps.push_back(entry);
    appMap.insert(entry->id, entry);

    for (const auto &group : groups) {
      auto ss = group.split(' ');

      if (ss.size() == 3 && ss[0].toLower() == "desktop" &&
          ss[1].toLower() == "action") {
        ini.beginGroup(group);
        auto action = std::make_shared<DesktopAction>(entry);

        action->id = entry->id + '.' + ss[2];
        action->name = ini.value("Name").toString();
        action->exec = ini.value("Exec").toString();
        action->icon_ = ini.value("Icon", entry->icon()).toString();
        action->fqn_ = entry->name + ": " + action->name;
        entry->actions.push_back(action);
        ini.endGroup();

        appMap.insert(action->id, action);
      }
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
        qDebug() << "add default app " << ini.value(key) << " for mime " << key;
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
  }
};
