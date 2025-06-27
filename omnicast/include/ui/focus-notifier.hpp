#pragma once
#include <qlogging.h>
#include <qobject.h>
#include <qdebug.h>
#include <qtmetamacros.h>

class FocusNotifier : public QObject {
  Q_OBJECT

public:
  FocusNotifier(QObject *parent) : QObject(parent) {}

signals:
  void focusChanged(bool value) const;
};
