#pragma once
#include <qscrollarea.h>
#include <qtmetamacros.h>

class QObject;
class QEvent;

class VerticalScrollArea : public QScrollArea {
  Q_OBJECT

public:
  bool eventFilter(QObject *o, QEvent *e) override;
  void resizeEvent(QResizeEvent *event) override;
  VerticalScrollArea(QWidget *parent = nullptr);

signals:
  void widgetResized() const;
};
