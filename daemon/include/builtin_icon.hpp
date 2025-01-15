#pragma once
#include <QString>
#include <QIcon>

class BuiltinIconService {
public:
  static const QList<QString> &icons();
  static QIcon fromName(const QString &name);
};
