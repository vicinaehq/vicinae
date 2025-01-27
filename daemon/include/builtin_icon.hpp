#pragma once
#include <QString>
#include <QIcon>

class BuiltinIconService {
public:
  static const QList<QString> &icons();
  static QString unknownIcon() { return ":icons/question-mark-circle.svg"; }
};
