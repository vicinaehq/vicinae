#pragma once
#include <qwidget.h>

class ButtonBase : public QWidget {
  Q_OBJECT
  bool _hovered;
  QColor _bgColor;

protected:
  bool event(QEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

public:
  virtual void hoverChanged(bool hovered);
  bool hovered() const { return _hovered; }
  void setBackgroundColor(const QColor &color);

  ButtonBase();

signals:
  void clicked() const;
};
