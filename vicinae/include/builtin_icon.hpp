#pragma once
#include <QString>
#include <QIcon>
#include <qpixmapcache.h>

class BuiltinIconService {
public:
  static const QList<QString> &icons();
  static QString unknownIcon() { return ":icons/question-mark-circle.svg"; }
  static QString pathForName(const QString &name) { return QString(":icons/%1").arg(name); }
};
