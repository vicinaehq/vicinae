#pragma once
#include <qobject.h>
#include <qtmetamacros.h>

class FocusNotifier : public QObject {
  Q_OBJECT

public:
  FocusNotifier(QObject *parent) : QObject(parent) {}

signals:
  void focusChanged(bool value) const;
};
