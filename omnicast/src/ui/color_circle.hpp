#pragma once
#include <QPainter>
#include <QWidget>

class ColorCircle : public QWidget {
  QColor color;
  QSize size;
  QColor strokeColor;
  size_t strokeWidth;

protected:
  void paintEvent(QPaintEvent *event) override;

  QSize sizeHint() const override;

public:
  ColorCircle(QColor color, QSize size, QWidget *parent = nullptr);

  ColorCircle &setStroke(QColor color, size_t width = 3);
};
