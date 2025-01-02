#pragma once
#include <QDebug>
#include <QString>
#include <QStringView>
#include <clocale>
#include <qdir.h>
#include <qhash.h>
#include <qlocale.h>
#include <qlogging.h>
#include <qregularexpression.h>
#include <qstringliteral.h>
#include <qstringview.h>

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
  XdgDesktopEntry(const QString &path) {
    QFile file(path);

    file.open(QIODevice::ReadOnly);

    QString data = file.readAll();

    *this = XdgDesktopEntry::Parser(data).parse();
  }

  struct Action {
    QString id;
    QString name;
    QString icon;
    QList<QString> exec;
  };

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
