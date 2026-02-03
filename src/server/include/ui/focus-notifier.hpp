#pragma once
#include <qcoreevent.h>
#include <qlogging.h>
#include <qobject.h>
#include <qdebug.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class FocusNotifier : public QObject {
  Q_OBJECT

  bool eventFilter(QObject *watched, QEvent *event) override {
    if (event->type() == QEvent::FocusIn) {
      emit focusChanged(true);
    } else if (event->type() == QEvent::FocusOut) {
      emit focusChanged(false);
    }

    return false;
  }

public:
  void track(QWidget *w) { w->installEventFilter(this); }

  FocusNotifier(QObject *parent) : QObject(parent) {}

signals:
  void focusChanged(bool value) const;
};
