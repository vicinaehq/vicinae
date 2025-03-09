#pragma once
#include <qwidget.h>

class Button : public QWidget {
  Q_OBJECT
  bool _hovered;
  QColor _bgColor;

protected:
  bool event(QEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

public:
  virtual void hovered(bool hovered);
  void setBackgroundColor(const QColor &color);

  Button();

signals:
  void clicked() const;
};
