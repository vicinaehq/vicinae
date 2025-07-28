#pragma once
#include <qscrollarea.h>

class QObject;
class QEvent;

class VerticalScrollArea : public QScrollArea {
public:
  bool eventFilter(QObject *o, QEvent *e) override;
  void resizeEvent(QResizeEvent *event) override;
  VerticalScrollArea(QWidget *parent = nullptr);
};
