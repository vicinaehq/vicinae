#pragma once
#include <QDebug>
#include <QString>
#include <QStringView>
#include <cctype>
#include <clocale>
#include <qcontainerfwd.h>
#include <qdir.h>
#include <qhash.h>
#include <qlocale.h>
#include <qlogging.h>
#include <qregularexpression.h>
#include <qstringliteral.h>
#include <qstringview.h>

namespace fs = std::filesystem;

struct Locale {
  QStringView lang;
  QStringView country;
  QStringView encoding;
  QStringView variant;
  QStringView modifier;

  QString toString();
  Locale(QStringView data);
};

class XdgDesktopEntry {
  class ExecParser {
    enum State {
      START,
      WHITESPACE,
      DQUOTE,

      DQUOTE_ESCAPE,
    };

  public:
    static QStringList parse(const QString &key) {
      QStringList list;
      size_t ptr = 0;
      State state = START;
      QString token;

      while (ptr < key.size()) {
        auto ch = key.at(ptr);

        switch (state) {
        case START:
          if (ch == '"') {
            state = DQUOTE;
            ++ptr;
          }

          else if (ch.isSpace()) {
            list << token;
            token.clear();
            state = WHITESPACE;
          }

          else {
            token.push_back(ch);
            ++ptr;
          }

          break;
        case WHITESPACE:
          if (!ch.isSpace()) {
            state = START;
          } else {
            ++ptr;
          }
          break;
        case DQUOTE:
          if (ch == '\\') {
            state = DQUOTE_ESCAPE;
            ++ptr;
          } else if (ch == '"') {
            state = START;
            ++ptr;
          } else {
            token.push_back(ch);
            ++ptr;
          }
          break;
        case DQUOTE_ESCAPE:
          token.push_back(ch);
          state = DQUOTE;
          ++ptr;
          break;
        }
      }

      if (!token.isEmpty()) list << token;

      return list;
    }
  };

  class Parser {
    struct Entry {
      QStringView key;
      QString value;
      std::optional<Locale> locale;
    };

    QString rawLocale;
    QStringView data;
    size_t cursor;

    using Group = QHash<QStringView, QString>;

    Group top;
    QHash<QStringView, Group> groups;
    Group *currentGroup = &top;

    QStringView parseGroupHeader();
    QStringView parseEntryKey();
    QChar parseEscaped();
    QString parseEntryValue();
    Entry parseEntry();

    void skipWS();
    uint computeLocalePriority(Locale lhs, Locale rhs);

  public:
    XdgDesktopEntry parse();

    Parser(QStringView view) noexcept;

    void dump() {
      dumpGroup(top);
      for (const auto &entry : groups) {
        dumpGroup(entry);
      }
    }

    void dumpGroup(const Group &grp) {
      for (const auto &entry : grp.keys()) {
        qDebug() << entry << "=" << grp[entry];
      }
    }
  };

  XdgDesktopEntry() {}

public:
  XdgDesktopEntry(fs::path parentPath, fs::path childPath) {
    QFile file(parentPath / childPath);

    file.open(QIODevice::ReadOnly);

    QString data = file.readAll();

    *this = XdgDesktopEntry::Parser(data).parse();

    for (auto dir: childPath.parent_path()) {
      id += dir.c_str();
      id += '-';
    }
    id += childPath.stem().c_str();
  }

  struct Action {
    QString id;
    QString name;
    QString icon;
    QList<QString> exec;
  };

  QString id;
  QString type;
  QString version;
  QString name;
  QString genericName;
  bool noDisplay;
  QString comment;
  QString icon;
  bool hidden;
  QString tryExec;
  QList<QString> exec;
  QString path;
  bool terminal;
  QList<QString> mimeType;
  QList<QString> categories;
  QList<QString> keywords;
  QString startupWMClass;
  bool singleMainWindow;

  QList<Action> actions;
};
