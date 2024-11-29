#pragma once
#include <cctype>
#include <qdir.h>
#include <qhash.h>
#include <qlist.h>
#include <qlogging.h>
#include <qset.h>
#include <qsettings.h>
#include <qtenvironmentvariables.h>

struct DesktopAction {
  QString id;
  QString name;
  QString exec;
};

struct DesktopFile {
  QString id;
  QString name;
  QString exec;
  QString comment;
  QString icon;
  bool noDisplay;
  bool isLinkOpener;
  QString wmClass;
  QList<QString> keywords;
  QList<QString> categories;
  QList<std::shared_ptr<DesktopFile>> actions;
};

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

class AppDatabase {
  QList<QDir> paths;
  QHash<QString, std::shared_ptr<DesktopFile>> appMap;

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

      Tokenizer(const QString &s) : s(s) {}

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
            if (c == '"') {
              state = START;
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

public:
  QList<std::shared_ptr<DesktopFile>> apps;

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
        QSettings ini(fullpath, QSettings::IniFormat);

        auto groups = ini.childGroups();
        std::shared_ptr<DesktopFile> file(nullptr);

        for (const auto &group : groups) {
          auto parts = group.split(' ');

          if (parts.size() == 2 && parts[0] == "Desktop" &&
              parts[1] == "Entry") {
            ini.beginGroup(group);
            file = std::make_shared<DesktopFile>(DesktopFile{
                .id = entry,
                .name = ini.value("Name").toString(),
                .exec = ini.value("Exec").toString(),
                .comment = ini.value("Comment").toString(),
                .icon = ini.value("Icon").toString(),
                .noDisplay = ini.value("NoDisplay").toString() == "true",
                .isLinkOpener = false,
                .wmClass = ini.value("StartupWMClass").toString(),
                .keywords = ini.value("Keywords").toString().split(";"),
                .categories = ini.value("Categories").toString().split(";"),
            });
            ini.endGroup();
            break;
          }
        }

        if (!file)
          continue;

        XdgCommandLine::Tokenizer exec(file->exec);
        std::optional<XdgCommandLine::Token> arg;

        while ((arg = exec.next())) {
          if (arg->type == XdgCommandLine::TokenType::FieldCode) {
            qDebug() << "field=" << arg->value;
          }
          if (arg->type == XdgCommandLine::TokenType::FieldCode &&
              isLinkOpenerFieldCode(arg->value)) {
            file->isLinkOpener = true;
            break;
          }
          qDebug() << arg->value;
        }

        apps.push_back(file);
        appMap.insert(fullpath, file);

        for (const auto &group : groups) {
          auto parts = group.split(' ');

          if (parts.size() == 3 && parts[0] == "Desktop" &&
              parts[1] == "Action") {
            ini.beginGroup(group);

            auto icon = ini.value("Icon", "e");

            if (icon.isNull())
              icon = file->icon;

            auto action = std::make_shared<DesktopFile>(DesktopFile{
                .id = parts[2],
                .name = ini.value("Name").toString(),
                .exec = ini.value("Exec").toString(),
                .icon = ini.value("Icon", file->icon).toString(),
                .isLinkOpener = false,
            });

            XdgCommandLine::Tokenizer exec(action->exec);
            std::optional<XdgCommandLine::Token> arg;

            while ((arg = exec.next())) {
              if (arg->type == XdgCommandLine::TokenType::FieldCode) {
                qDebug() << "field=" << arg->value;
              }
              if (arg->type == XdgCommandLine::TokenType::FieldCode &&
                  isLinkOpenerFieldCode(arg->value)) {
                action->isLinkOpener = true;
                break;
              }
              qDebug() << arg->value;
            }

            file->actions.push_back(action);

            ini.endGroup();
          }
        }
      }
    }
  }
};
